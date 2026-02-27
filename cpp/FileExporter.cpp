// src/FileExporter.cpp
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "FileExporter.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

bool FileExporter::exportTreeStructure(TreeBase* tree, const std::string& filename) {
    if (!tree) return false;

    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "Структура дерева\n";
    file << "Тип: " << static_cast<int>(tree->getType()) << "\n";
    file << "Высота: " << tree->getHeight() << "\n";
    file << "Количество узлов: " << tree->getNodeCount() << "\n";
    file << "\nСтруктура:\n";
    file << tree->toString() << "\n";

    file.close();
    return true;
}

bool FileExporter::exportTraversalResults(TreeBase* tree, TraversalType type,
    const std::string& filename) {

    if (!tree) return false;

    std::vector<int> traversal = tree->traverse(type);
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    std::string traversal_name;
    switch (type) {
    case TraversalType::INORDER:
        traversal_name = "Центрированный обход";
        break;
    case TraversalType::PREORDER:
        traversal_name = "Прямой обход";
        break;
    case TraversalType::POSTORDER:
        traversal_name = "Обратный обход";
        break;
    case TraversalType::LEVELORDER:
        traversal_name = "Обход в ширину";
        break;
    default:
        traversal_name = "Неизвестный обход";
    }

    file << traversal_name << "\n";
    file << "Количество элементов: " << traversal.size() << "\n";
    file << "\nЭлементы:\n";

    for (size_t i = 0; i < traversal.size(); i++) {
        file << traversal[i];
        if (i < traversal.size() - 1) {
            if ((i + 1) % 10 == 0) file << "\n";
            else file << ", ";
        }
    }

    file.close();
    return true;
}

bool FileExporter::exportStatisticsToCSV(const StatisticsCollector& stats,
    const std::string& filename) {

    std::vector<DetailedStatistics> all_stats = stats.getAllStatistics();
    std::ofstream file(filename);

    if (!file.is_open()) return false;

    // Заголовок CSV
    file << "TreeType;InsertTime;SearchTime;DeleteTime;TraversalTime;"
        << "MaxHeight;MinHeight;AvgHeight;NodeCount;LeafCount;BalanceFactor\n";

    for (const auto& stat : all_stats) {
        auto metrics = stat.getMetricsMap();

        file << stat.treeName << ";"
            << metrics.at("insert_time") << ";"
            << metrics.at("search_time") << ";"
            << metrics.at("delete_time") << ";"
            << stat.timeMetrics.traversalTime << ";"
            << metrics.at("max_height") << ";"
            << metrics.at("min_height") << ";"
            << metrics.at("avg_height") << ";"
            << metrics.at("node_count") << ";"
            << metrics.at("leaf_count") << ";"
            << metrics.at("balance_factor") << "\n";
    }

    file.close();
    return true;
}

bool FileExporter::exportComparativeTest(
    const std::vector<TreeType>& treeTypes,
    const std::vector<std::string>& treeNames,
    const std::vector<std::vector<long long>>& results,
    const std::string& filename) {

    if (treeTypes.size() != treeNames.size() || treeTypes.size() != results.size()) {
        return false;
    }

    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Заголовок CSV
    file << "TreeType;TreeName;Iteration;TimeNs\n";

    for (size_t i = 0; i < treeTypes.size(); i++) {
        for (size_t j = 0; j < results[i].size(); j++) {
            file << static_cast<int>(treeTypes[i]) << ";"
                << treeNames[i] << ";"
                << j + 1 << ";"
                << results[i][j] << "\n";
        }
    }

    file.close();
    return true;
}

bool FileExporter::exportForPlotting(
    const std::vector<std::pair<std::string, std::vector<long long>>>& data,
    const std::string& filename,
    const std::string& xlabel,
    const std::string& ylabel) {

    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Формат для gnuplot/matplotlib
    file << "# Данные для построения графиков\n";
    file << "# X: " << xlabel << "\n";
    file << "# Y: " << ylabel << "\n\n";

    // Находим максимальную длину среди всех наборов данных
    size_t max_size = 0;
    for (const auto& pair : data) {
        const std::vector<long long>& values = pair.second;
        max_size = std::max(max_size, values.size());
    }

    // Заголовки столбцов
    file << "Index";
    for (const auto& pair : data) {
        const std::string& label = pair.first;
        file << ";" << label;
    }
    file << "\n";

    // Данные
    for (size_t i = 0; i < max_size; i++) {
        file << i + 1;

        for (const auto& pair : data) {
            const std::vector<long long>& values = pair.second;
            file << ";";
            if (i < values.size()) {
                file << values[i];
            }
            else {
                file << "0";
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}

std::string FileExporter::generateMarkdownReport(
    const std::vector<DetailedStatistics>& stats,
    const std::string& title) {

    std::stringstream ss;
    ss << "# " << title << "\n\n";
    ss << "Отчет сгенерирован: " << getCurrentDateTime() << "\n\n";

    for (const auto& stat : stats) {
        ss << "## " << stat.treeName << "\n\n";
        ss << stat.generateReport() << "\n\n";
    }

    return ss.str();
}

std::string FileExporter::getCurrentDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm tm; 
    localtime_s(&tm, &t);

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string FileExporter::escapeCSV(const std::string& str) {
    std::string result = str;

    // Экранируем кавычки
    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\"\"");
        pos += 2;
    }

    // Если строка содержит разделитель, оборачиваем в кавычки
    if (result.find(';') != std::string::npos ||
        result.find('\n') != std::string::npos ||
        result.find('"') != std::string::npos) {
        result = "\"" + result + "\"";
    }

    return result;
}