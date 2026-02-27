// src/Benchmark.cpp
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <numeric>

#include "AVLTree.h"
#include "AATree.h"
#include "Benchmark.h"
#include "DataGenerator.h"
#include "Treap.h"
#include "TwoThreeTree.h"


Benchmark::Benchmark() {
    tree_names = {
        {TreeType::AVL, "AVL Tree"},
        {TreeType::AA, "AA Tree"},
        {TreeType::TREAP, "Treap"},
        {TreeType::TWO_THREE, "2-3 Tree"}
    };
}

std::unique_ptr<TreeBase> Benchmark::createTree(TreeType type) {
    switch (type) {
    case TreeType::AVL:
        return std::make_unique<AVLTree>();
    case TreeType::AA:
        return std::make_unique<AATree>();
    case TreeType::TREAP:
        return std::make_unique<Treap>();
    case TreeType::TWO_THREE:
        return std::make_unique<TwoThreeTree>();
    default:
        return nullptr;
    }
}

BenchmarkResult Benchmark::testInsertion(TreeType type, const std::vector<int>& data) {
    auto tree = createTree(type);
    if (!tree) return BenchmarkResult();

    BenchmarkResult result;
    result.tree_type = type;
    result.operation = "Insertion";
    result.data_size = data.size();
    result.data_order = DataOrder::RANDOM; // TODO: передавать как параметр

    auto start = std::chrono::high_resolution_clock::now();

    int rotations_before = tree->getStatistics().rotations_count;
    for (int key : data) {
        tree->insert(key);
    }
    int rotations_after = tree->getStatistics().rotations_count;

    auto end = std::chrono::high_resolution_clock::now();

    result.total_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    result.avg_time_ns = data.empty() ? 0 : result.total_time_ns / data.size();
    result.final_height = tree->getHeight();
    result.final_nodes = tree->getNodeCount();
    result.rotations = rotations_after - rotations_before;

    return result;
}

BenchmarkResult Benchmark::testSearch(TreeType type, const std::vector<int>& data_to_insert,
    const std::vector<int>& data_to_search) {
    auto tree = createTree(type);
    if (!tree) return BenchmarkResult();

    // Сначала заполн€ем дерево
    for (int key : data_to_insert) {
        tree->insert(key);
    }

    BenchmarkResult result;
    result.tree_type = type;
    result.operation = "Search";
    result.data_size = data_to_search.size();

    auto start = std::chrono::high_resolution_clock::now();

    for (int key : data_to_search) {
        tree->search(key);
    }

    auto end = std::chrono::high_resolution_clock::now();

    result.total_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    result.avg_time_ns = data_to_search.empty() ? 0 : result.total_time_ns / data_to_search.size();
    result.final_height = tree->getHeight();
    result.final_nodes = tree->getNodeCount();

    return result;
}

BenchmarkResult Benchmark::testDeletion(TreeType type, const std::vector<int>& data_to_insert,
    const std::vector<int>& data_to_remove) {
    auto tree = createTree(type);
    if (!tree) return BenchmarkResult();

    // Сначала заполн€ем дерево
    for (int key : data_to_insert) {
        tree->insert(key);
    }

    BenchmarkResult result;
    result.tree_type = type;
    result.operation = "Deletion";
    result.data_size = data_to_remove.size();

    auto start = std::chrono::high_resolution_clock::now();

    for (int key : data_to_remove) {
        tree->remove(key);
    }

    auto end = std::chrono::high_resolution_clock::now();

    result.total_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    result.avg_time_ns = data_to_remove.empty() ? 0 : result.total_time_ns / data_to_remove.size();
    result.final_height = tree->getHeight();
    result.final_nodes = tree->getNodeCount();

    return result;
}

std::vector<BenchmarkResult> Benchmark::runFullTest(int data_size, DataOrder order,
    int iterations) {
    std::vector<BenchmarkResult> results;
    DataGenerator generator;

    std::vector<TreeType> tree_types = {
        TreeType::AVL,
        TreeType::AA,
        TreeType::TREAP,
        TreeType::TWO_THREE
    };

    for (int i = 0; i < iterations; i++) {
        std::vector<int> data = generator.generate(data_size, order);

        for (TreeType type : tree_types) {
            BenchmarkResult insert_result = testInsertion(type, data);
            BenchmarkResult search_result = testSearch(type, data, data);
            BenchmarkResult delete_result = testDeletion(type, data, data);

            results.push_back(insert_result);
            results.push_back(search_result);
            results.push_back(delete_result);
        }
    }

    return results;
}

void Benchmark::saveResultsToCSV(const std::vector<BenchmarkResult>& results,
    const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return;
    }

    // Заголовок CSV
    file << "TreeType;Operation;DataSize;TotalTimeNs;AvgTimeNs;"
        << "FinalHeight;FinalNodes;Rotations\n";

    for (const auto& result : results) {
        file << tree_names[result.tree_type] << ";"
            << result.operation << ";"
            << result.data_size << ";"
            << result.total_time_ns << ";"
            << result.avg_time_ns << ";"
            << result.final_height << ";"
            << result.final_nodes << ";"
            << result.rotations << "\n";
    }

    file.close();
    std::cout << "–езультаты сохранены в: " << filename << std::endl;
}

void Benchmark::printResults(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n=== –≈«”Ћ№“ј“џ “≈—“»–ќ¬јЌ»я ===\n\n";

    std::map<std::pair<TreeType, std::string>, std::vector<BenchmarkResult>> grouped_results;

    for (const auto& result : results) {
        auto key = std::make_pair(result.tree_type, result.operation);
        grouped_results[key].push_back(result);
    }

    for (const auto& entry : grouped_results) {
        TreeType tree_type = entry.first.first;
        std::string operation = entry.first.second;
        const std::vector<BenchmarkResult>& result_group = entry.second;
        std::string tree_name = tree_names[tree_type];

        std::cout << tree_name << " - " << operation << ":\n";

        if (!result_group.empty()) {
            long long total_time_sum = 0;
            long long avg_time_sum = 0;

            for (const auto& result : result_group) {
                total_time_sum += result.total_time_ns;
                avg_time_sum += result.avg_time_ns;
            }

            long long avg_total_time = total_time_sum / result_group.size();
            long long avg_avg_time = avg_time_sum / result_group.size();

            std::cout << "  Среднее общее врем€: " << avg_total_time << " нс\n";
            std::cout << "  Среднее врем€ на операцию: " << avg_avg_time << " нс\n";
            std::cout << "  Финальна€ высота: " << result_group[0].final_height << "\n";
            std::cout << "  Финальное количество узлов: " << result_group[0].final_nodes << "\n";
            if (result_group[0].rotations > 0) {
                std::cout << "  Количество поворотов: " << result_group[0].rotations << "\n";
            }
        }
        std::cout << "-------------------------\n";
    }
}