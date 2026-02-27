#pragma once
// include/Config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#include "DataGenerator.h"

struct BenchmarkConfig {
    // Конфигурация тестов
    struct TestConfig {
        int min_data_size = 100;
        int max_data_size = 10000;
        int step_size = 100;
        int iterations_per_step = 10;
        DataOrder data_order = DataOrder::RANDOM;
        bool enable_progress_bar = true;
        bool save_detailed_logs = false;
    } testConfig;

    // Конфигурация вывода
    struct OutputConfig {
        bool show_console_output = true;
        bool save_to_file = true;
        std::string output_directory = "results/";
        std::string csv_separator = ";";
        bool generate_plots = true;
        bool export_raw_data = false;
    } outputConfig;

    // Конфигурация деревьев
    struct TreeConfig {
        bool test_avl = true;
        bool test_aa = true;
        bool test_treap = true;
        bool test_two_three = true;

        // Специфичные настройки для каждого дерева
        int treap_max_priority = 1000000;
        int two_three_node_capacity = 3; // Для 2-3 дерева = 3, для 2-3-4 = 4
    } treeConfig;

    // Загрузка/сохранение конфигурации
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);

    // Получение конфигурации по умолчанию
    static BenchmarkConfig getDefaultConfig();
};

#endif // CONFIG_H