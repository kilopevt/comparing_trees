#pragma once
// include/Visualizer.h
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>
#include <utility>

#include "TreeBase.h"

class Visualizer {
public:
    // Общие функции визуализации для всех деревьев
    static std::string visualizeTree(TreeBase* tree);

    // Специализированные визуализации для каждого типа дерева
    static std::string visualizeAVLTree(TreeBase* tree);
    static std::string visualizeAATree(TreeBase* tree);
    static std::string visualizeTreap(TreeBase* tree);
    static std::string visualizeTwoThreeTree(TreeBase* tree);

    // Визуализация сравнения производительности
    static std::string visualizeBenchmarkResults(
        const std::vector<std::pair<std::string, std::vector<long long>>>& results);

    // Создание ASCII-графика
    static std::string createAsciiChart(
        const std::vector<std::string>& labels,
        const std::vector<long long>& values,
        int width = 50);

private:
    // Вспомогательные методы для отрисовки
    static std::string createHorizontalBar(long long value, long long maxValue, int width);
    static std::string formatTime(long long nanoseconds);
    static std::string formatMemory(size_t bytes);
};

#endif // VISUALIZER_H