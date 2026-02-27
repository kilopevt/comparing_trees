// src/Visualizer.cpp
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "Visualizer.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

std::string Visualizer::visualizeTree(TreeBase* tree) {
    if (!tree) return "Дерево не инициализировано";
    return tree->toString();
}

std::string Visualizer::visualizeAVLTree(TreeBase* tree) {
    return visualizeTree(tree);
}

std::string Visualizer::visualizeAATree(TreeBase* tree) {
    return visualizeTree(tree);
}

std::string Visualizer::visualizeTreap(TreeBase* tree) {
    return visualizeTree(tree);
}

std::string Visualizer::visualizeTwoThreeTree(TreeBase* tree) {
    return visualizeTree(tree);
}

std::string Visualizer::visualizeBenchmarkResults(
    const std::vector<std::pair<std::string, std::vector<long long>>>& results) {

    if (results.empty()) return "Нет данных для визуализации";

    std::stringstream ss;
    ss << "=== СРАВНИТЕЛЬНАЯ ТАБЛИЦА ПРОИЗВОДИТЕЛЬНОСТИ ===\n\n";

    // Находим максимальное значение для масштабирования
    long long max_value = 0;
    for (const auto& result_pair : results) {
        const std::vector<long long>& values = result_pair.second;
        if (!values.empty()) {
            // Используем ручной поиск максимума вместо std::max_element
            long long max_in_group = 0;
            for (long long val : values) {
                if (val > max_in_group) max_in_group = val;
            }
            if (max_in_group > max_value) max_value = max_in_group;
        }
    }

    if (max_value == 0) max_value = 1; // Избегаем деления на ноль

    const int BAR_WIDTH = 50;

    for (const auto& result_pair : results) {
        const std::string& name = result_pair.first;
        const std::vector<long long>& values = result_pair.second;

        if (values.empty()) continue;

        // Вычисляем среднее значение
        long long sum = 0;
        for (long long val : values) {
            sum += val;
        }
        long long avg_value = sum / values.size();

        ss << std::setw(15) << std::left << name << ": ";
        ss << std::setw(12) << std::right << formatTime(avg_value) << "  ";
        ss << createHorizontalBar(avg_value, max_value, BAR_WIDTH) << "\n";
    }

    return ss.str();
}

std::string Visualizer::createAsciiChart(
    const std::vector<std::string>& labels,
    const std::vector<long long>& values,
    int width) {

    if (labels.size() != values.size() || values.empty()) {
        return "Некорректные данные для графика";
    }

    std::stringstream ss;
    long long max_value = 0;
    // Ручной поиск максимума
    for (long long val : values) {
        if (val > max_value) max_value = val;
    }
    if (max_value == 0) max_value = 1;

    const int CHART_HEIGHT = 10;

    // Создаем сетку для графика
    std::vector<std::string> grid(CHART_HEIGHT, std::string(width, ' '));

    for (size_t i = 0; i < values.size(); i++) {
        int bar_height = static_cast<int>((values[i] * CHART_HEIGHT) / max_value);
        if (bar_height == 0 && values[i] > 0) bar_height = 1;

        int x_pos = static_cast<int>((i * width) / labels.size());
        if (x_pos >= width) x_pos = width - 1;

        for (int h = 0; h < bar_height; h++) {
            int y_pos = CHART_HEIGHT - 1 - h;
            if (y_pos >= 0 && y_pos < CHART_HEIGHT) {
                grid[y_pos][x_pos] = '#';
            }
        }
    }

    // Выводим график
    for (int h = 0; h < CHART_HEIGHT; h++) {
        ss << "|" << grid[h] << "\n";
    }

    // Линия оси X
    ss << "+";
    for (int i = 0; i < width; i++) ss << "-";
    ss << "\n";

    // Подписи
    for (size_t i = 0; i < labels.size(); i++) {
        if (i % 2 == 0) { // Выводим только каждую вторую подпись для читаемости
            int x_pos = static_cast<int>((i * width) / labels.size());
            ss << std::setw(5) << labels[i];
        }
    }

    return ss.str();
}

std::string Visualizer::createHorizontalBar(long long value, long long maxValue, int width) {
    if (maxValue == 0) return "";

    int bar_length = static_cast<int>((value * width) / maxValue);
    if (bar_length == 0 && value > 0) bar_length = 1;
    if (bar_length > width) bar_length = width;

    std::string bar(bar_length, '█');
    std::string empty(width - bar_length, '░');

    return "[" + bar + empty + "]";
}

std::string Visualizer::formatTime(long long nanoseconds) {
    if (nanoseconds < 1000) {
        return std::to_string(nanoseconds) + " нс";
    }
    else if (nanoseconds < 1000000) {
        // Используем stringstream для форматирования с плавающей точкой
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << (nanoseconds / 1000.0) << " мкс";
        return ss.str();
    }
    else if (nanoseconds < 1000000000) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << (nanoseconds / 1000000.0) << " мс";
        return ss.str();
    }
    else {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << (nanoseconds / 1000000000.0) << " с";
        return ss.str();
    }
}

std::string Visualizer::formatMemory(size_t bytes) {
    const char* units[] = { "байт", "КБ", "МБ", "ГБ" };
    int unit_index = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024 && unit_index < 3) {
        size /= 1024;
        unit_index++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return ss.str();
}