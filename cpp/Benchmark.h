#pragma once
// include/Benchmark.h
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <vector>
#include <memory>
#include <map>
#include <string>

#include "CommonTypes.h"
#include "TreeBase.h"

struct BenchmarkResult {
    TreeType tree_type;
    std::string operation;
    int data_size;
    DataOrder data_order;

    long long total_time_ns;
    long long avg_time_ns;
    long long min_time_ns;
    long long max_time_ns;

    int final_height;
    int final_nodes;
    int rotations;
    int comparisons;
};

class Benchmark {
private:
    std::map<TreeType, std::string> tree_names;
    std::unique_ptr<TreeBase> createTree(TreeType type);

public:
    Benchmark();

    // Тестирование отдельных операций
    BenchmarkResult testInsertion(TreeType type, const std::vector<int>& data);
    BenchmarkResult testSearch(TreeType type, const std::vector<int>& data_to_insert,
        const std::vector<int>& data_to_search);
    BenchmarkResult testDeletion(TreeType type, const std::vector<int>& data_to_insert,
        const std::vector<int>& data_to_remove);

    // Комплексное тестирование
    std::vector<BenchmarkResult> runFullTest(int data_size, DataOrder order,
        int iterations = 10);

    // Сохранение результатов
    void saveResultsToCSV(const std::vector<BenchmarkResult>& results,
        const std::string& filename);

    // Вывод в консоль
    void printResults(const std::vector<BenchmarkResult>& results);
};

#endif // BENCHMARK_H