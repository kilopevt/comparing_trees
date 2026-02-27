// src/Statistics.cpp
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Statistics.h"

void DetailedStatistics::reset() {
    treeType = TreeType::UNKNOWN;
    treeName = "";
    timeMetrics = TimeMetrics();
    structureMetrics = StructureMetrics();

    // Сброс специфических метрик
    avlRotations = 0;
    avlBalanceOperations = 0;
    aaSkewOperations = 0;
    aaSplitOperations = 0;
    treapSplitOperations = 0;
    treapMergeOperations = 0;
    treapPriorityChanges = 0;
    twoThreeSplits = 0;
    twoThreeMerges = 0;
    twoThreeBorrows = 0;

    insertTimes.clear();
    searchTimes.clear();
    deleteTimes.clear();
}

void DetailedStatistics::addInsertTime(long long time) {
    insertTimes.push_back(time);
    timeMetrics.insertTime += time;
    timeMetrics.operationCount++;
}

void DetailedStatistics::addSearchTime(long long time) {
    searchTimes.push_back(time);
    timeMetrics.searchTime += time;
    timeMetrics.operationCount++;
}

void DetailedStatistics::addDeleteTime(long long time) {
    deleteTimes.push_back(time);
    timeMetrics.deleteTime += time;
    timeMetrics.operationCount++;
}

std::string DetailedStatistics::generateReport() const {
    std::stringstream ss;
    ss << "Отчет для: " << treeName << "\n";
    ss << "================================\n";

    ss << "Метрики производительности:\n";
    ss << "  Вставка: " << timeMetrics.insertTime << " нс (среднее: "
        << (timeMetrics.operationCount > 0 ? timeMetrics.insertTime / timeMetrics.operationCount : 0) << " нс)\n";
    ss << "  Поиск: " << timeMetrics.searchTime << " нс (среднее: "
        << (timeMetrics.operationCount > 0 ? timeMetrics.searchTime / timeMetrics.operationCount : 0) << " нс)\n";
    ss << "  Удаление: " << timeMetrics.deleteTime << " нс (среднее: "
        << (timeMetrics.operationCount > 0 ? timeMetrics.deleteTime / timeMetrics.operationCount : 0) << " нс)\n";

    ss << "\nМетрики структуры:\n";
    ss << "  Макс. высота: " << structureMetrics.maxHeight << "\n";
    ss << "  Мин. высота: " << structureMetrics.minHeight << "\n";
    ss << "  Средняя высота: " << std::fixed << std::setprecision(2) << structureMetrics.avgHeight << "\n";
    ss << "  Количество узлов: " << structureMetrics.nodeCount << "\n";
    ss << "  Количество листьев: " << structureMetrics.leafCount << "\n";
    ss << "  Коэффициент балансировки: " << std::fixed << std::setprecision(3) << structureMetrics.balanceFactor << "\n";

    // Вывод специфических метрик в зависимости от типа дерева
    switch (treeType) {
    case TreeType::AVL:
        ss << "\nСпецифичные метрики AVL дерева:\n";
        ss << "  Повороты: " << avlRotations << "\n";
        ss << "  Операции балансировки: " << avlBalanceOperations << "\n";
        break;
    case TreeType::AA:
        ss << "\nСпецифичные метрики AA дерева:\n";
        ss << "  Skew операции: " << aaSkewOperations << "\n";
        ss << "  Split операции: " << aaSplitOperations << "\n";
        break;
    case TreeType::TREAP:
        ss << "\nСпецифичные метрики Treap:\n";
        ss << "  Split операции: " << treapSplitOperations << "\n";
        ss << "  Merge операции: " << treapMergeOperations << "\n";
        ss << "  Изменения приоритета: " << treapPriorityChanges << "\n";
        break;
    case TreeType::TWO_THREE:
        ss << "\nСпецифичные метрики 2-3 дерева:\n";
        ss << "  Расщепления: " << twoThreeSplits << "\n";
        ss << "  Слияния: " << twoThreeMerges << "\n";
        ss << "  Заимствования: " << twoThreeBorrows << "\n";
        break;
    default:
        break;
    }

    return ss.str();
}

std::map<std::string, double> DetailedStatistics::getMetricsMap() const {
    std::map<std::string, double> metrics;
    metrics["insert_time"] = static_cast<double>(timeMetrics.insertTime);
    metrics["search_time"] = static_cast<double>(timeMetrics.searchTime);
    metrics["delete_time"] = static_cast<double>(timeMetrics.deleteTime);
    metrics["max_height"] = static_cast<double>(structureMetrics.maxHeight);
    metrics["min_height"] = static_cast<double>(structureMetrics.minHeight);
    metrics["avg_height"] = structureMetrics.avgHeight;
    metrics["node_count"] = static_cast<double>(structureMetrics.nodeCount);
    metrics["leaf_count"] = static_cast<double>(structureMetrics.leafCount);
    metrics["balance_factor"] = structureMetrics.balanceFactor;

    return metrics;
}

// StatisticsCollector реализация
StatisticsCollector::StatisticsCollector() {
    // Инициализация статистики для всех типов деревьев
    DetailedStatistics avlStats;
    avlStats.treeType = TreeType::AVL;
    avlStats.treeName = "AVL Tree";
    statistics[TreeType::AVL] = avlStats;

    DetailedStatistics aaStats;
    aaStats.treeType = TreeType::AA;
    aaStats.treeName = "AA Tree";
    statistics[TreeType::AA] = aaStats;

    DetailedStatistics treapStats;
    treapStats.treeType = TreeType::TREAP;
    treapStats.treeName = "Treap";
    statistics[TreeType::TREAP] = treapStats;

    DetailedStatistics twoThreeStats;
    twoThreeStats.treeType = TreeType::TWO_THREE;
    twoThreeStats.treeName = "2-3 Tree";
    statistics[TreeType::TWO_THREE] = twoThreeStats;
}

void StatisticsCollector::registerInsert(TreeBase* tree, long long time) {
    if (!tree) return;
    TreeType type = tree->getType();
    auto it = statistics.find(type);
    if (it != statistics.end()) {
        it->second.addInsertTime(time);
    }
}

void StatisticsCollector::registerSearch(TreeBase* tree, long long time) {
    if (!tree) return;
    TreeType type = tree->getType();
    auto it = statistics.find(type);
    if (it != statistics.end()) {
        it->second.addSearchTime(time);
    }
}

void StatisticsCollector::registerDelete(TreeBase* tree, long long time) {
    if (!tree) return;
    TreeType type = tree->getType();
    auto it = statistics.find(type);
    if (it != statistics.end()) {
        it->second.addDeleteTime(time);
    }
}

void StatisticsCollector::registerTraversal(TreeBase* tree, long long time) {
    if (!tree) return;
    TreeType type = tree->getType();
    auto it = statistics.find(type);
    if (it != statistics.end()) {
        it->second.timeMetrics.traversalTime += time;
        it->second.timeMetrics.operationCount++;
    }
}

void StatisticsCollector::updateStructureMetrics(TreeBase* tree) {
    if (!tree) return;
    TreeType type = tree->getType();
    auto it = statistics.find(type);
    if (it == statistics.end()) return;

    auto& stats = it->second;

    int height = tree->getHeight();
    int nodeCount = tree->getNodeCount();

    if (height > stats.structureMetrics.maxHeight) {
        stats.structureMetrics.maxHeight = height;
    }
    if (stats.structureMetrics.minHeight == 0 || height < stats.structureMetrics.minHeight) {
        stats.structureMetrics.minHeight = height;
    }

    // Обновляем среднюю высоту
    if (stats.timeMetrics.operationCount > 0) {
        stats.structureMetrics.avgHeight =
            (stats.structureMetrics.avgHeight * (stats.timeMetrics.operationCount - 1) + height) /
            stats.timeMetrics.operationCount;
    }

    stats.structureMetrics.nodeCount = nodeCount;
    // Для упрощения считаем, что листьев примерно nodeCount/2
    stats.structureMetrics.leafCount = nodeCount > 0 ? (nodeCount + 1) / 2 : 0;
}

const DetailedStatistics& StatisticsCollector::getStatistics(TreeType type) const {
    auto it = statistics.find(type);
    if (it != statistics.end()) {
        return it->second;
    }

    // Возвращаем пустую статистику если не нашли
    static DetailedStatistics emptyStats;
    return emptyStats;
}

std::vector<DetailedStatistics> StatisticsCollector::getAllStatistics() const {
    std::vector<DetailedStatistics> result;
    for (const auto& pair : statistics) {
        result.push_back(pair.second);
    }
    return result;
}

void StatisticsCollector::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    // Заголовок CSV
    file << "TreeType,TreeName,InsertTime,SearchTime,DeleteTime,TraversalTime,"
        << "MaxHeight,MinHeight,AvgHeight,NodeCount,LeafCount,BalanceFactor,"
        << "AVLRotations,AASkew,AASplit,TreapSplits,TreapMerges,TwoThreeSplits,TwoThreeMerges\n";

    for (const auto& pair : statistics) {
        const DetailedStatistics& stats = pair.second;
        auto metrics = stats.getMetricsMap();

        file << static_cast<int>(stats.treeType) << ","
            << stats.treeName << ","
            << metrics.at("insert_time") << ","
            << metrics.at("search_time") << ","
            << metrics.at("delete_time") << ","
            << stats.timeMetrics.traversalTime << ","
            << metrics.at("max_height") << ","
            << metrics.at("min_height") << ","
            << metrics.at("avg_height") << ","
            << metrics.at("node_count") << ","
            << metrics.at("leaf_count") << ","
            << metrics.at("balance_factor") << ","
            << stats.avlRotations << ","
            << stats.aaSkewOperations << ","
            << stats.aaSplitOperations << ","
            << stats.treapSplitOperations << ","
            << stats.treapMergeOperations << ","
            << stats.twoThreeSplits << ","
            << stats.twoThreeMerges << "\n";
    }

    file.close();
}

void StatisticsCollector::exportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    file << "{\n";
    file << "  \"statistics\": [\n";

    bool first = true;
    for (const auto& pair : statistics) {
        const DetailedStatistics& stats = pair.second;

        if (!first) file << ",\n";
        first = false;

        file << "    {\n";
        file << "      \"treeType\": \"" << stats.treeName << "\",\n";
        file << "      \"timeMetrics\": {\n";
        file << "        \"insertTime\": " << stats.timeMetrics.insertTime << ",\n";
        file << "        \"searchTime\": " << stats.timeMetrics.searchTime << ",\n";
        file << "        \"deleteTime\": " << stats.timeMetrics.deleteTime << ",\n";
        file << "        \"traversalTime\": " << stats.timeMetrics.traversalTime << "\n";
        file << "      },\n";
        file << "      \"structureMetrics\": {\n";
        file << "        \"maxHeight\": " << stats.structureMetrics.maxHeight << ",\n";
        file << "        \"minHeight\": " << stats.structureMetrics.minHeight << ",\n";
        file << "        \"avgHeight\": " << stats.structureMetrics.avgHeight << ",\n";
        file << "        \"nodeCount\": " << stats.structureMetrics.nodeCount << ",\n";
        file << "        \"leafCount\": " << stats.structureMetrics.leafCount << ",\n";
        file << "        \"balanceFactor\": " << stats.structureMetrics.balanceFactor << "\n";
        file << "      },\n";

        // Специфичные метрики
        file << "      \"specificMetrics\": {\n";
        file << "        \"avlRotations\": " << stats.avlRotations << ",\n";
        file << "        \"avlBalanceOperations\": " << stats.avlBalanceOperations << ",\n";
        file << "        \"aaSkewOperations\": " << stats.aaSkewOperations << ",\n";
        file << "        \"aaSplitOperations\": " << stats.aaSplitOperations << ",\n";
        file << "        \"treapSplitOperations\": " << stats.treapSplitOperations << ",\n";
        file << "        \"treapMergeOperations\": " << stats.treapMergeOperations << ",\n";
        file << "        \"treapPriorityChanges\": " << stats.treapPriorityChanges << ",\n";
        file << "        \"twoThreeSplits\": " << stats.twoThreeSplits << ",\n";
        file << "        \"twoThreeMerges\": " << stats.twoThreeMerges << ",\n";
        file << "        \"twoThreeBorrows\": " << stats.twoThreeBorrows << "\n";
        file << "      }\n";
        file << "    }";
    }

    file << "\n  ]\n";
    file << "}\n";
    file.close();
}

void StatisticsCollector::clear() {
    for (auto& pair : statistics) {
        pair.second.reset();
    }
}