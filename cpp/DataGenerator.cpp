// src/DataGenerator.cpp
#include <algorithm>
#include <random>

#include "DataGenerator.h"

DataGenerator::DataGenerator(unsigned int seed) : rng(seed) {}
std::vector<int> DataGenerator::generate(int count, DataOrder order,
    int min_value, int max_value) {

    std::vector<int> data;
    data.reserve(count);

    std::uniform_int_distribution<int> dist(min_value, max_value);

    switch (order) {
    case DataOrder::RANDOM:
    {
        for (int i = 0; i < count; i++) {
            data.push_back(dist(rng));
        }
    }
    break;

    case DataOrder::ASCENDING:
    {
        for (int i = 0; i < count; i++) {
            data.push_back(min_value + (max_value - min_value) * i / (count - 1));
        }
    }
    break;

    case DataOrder::DESCENDING:
    {
        for (int i = 0; i < count; i++) {
            data.push_back(max_value - (max_value - min_value) * i / (count - 1));
        }
    }
    break;

    case DataOrder::ALMOST_SORTED:
    {
        // 90% отсортировано, 10% случайны
        for (int i = 0; i < count; i++) {
            if (i % 10 == 0) {
                data.push_back(dist(rng));
            }
            else {
                data.push_back(min_value + (max_value - min_value) * i / count);
            }
        }
        // Перемешиваем 10% элементов
        std::uniform_int_distribution<int> pos_dist(0, count - 1);
        for (int i = 0; i < count / 10; i++) {
            std::swap(data[pos_dist(rng)], data[pos_dist(rng)]);
        }
    }
    break;

    case DataOrder::SORTED_WITH_DUPLICATES:
    {
        // Сортированные с дубликатами
        std::uniform_int_distribution<int> small_dist(min_value, min_value + 10);
        for (int i = 0; i < count; i++) {
            data.push_back(small_dist(rng));
        }
        std::sort(data.begin(), data.end());
    }
    break;

    case DataOrder::REVERSE_ALMOST_SORTED:
    {
        // Почти отсортированные в обратном порядке
        for (int i = 0; i < count; i++) {
            if (i % 10 == 0) {
                data.push_back(dist(rng));
            }
            else {
                data.push_back(max_value - (max_value - min_value) * i / count);
            }
        }
        // Перемешиваем 10% элементов
        std::uniform_int_distribution<int> pos_dist2(0, count - 1);
        for (int i = 0; i < count / 10; i++) {
            std::swap(data[pos_dist2(rng)], data[pos_dist2(rng)]);
        }
    }
    break;
    }

    return data;
}

std::vector<int> DataGenerator::generateSequential(int start, int count) {
    std::vector<int> data(count);
    for (int i = 0; i < count; i++) {
        data[i] = start + i;
    }
    return data;
}

std::vector<int> DataGenerator::generateReverseSequential(int start, int count) {
    std::vector<int> data(count);
    for (int i = 0; i < count; i++) {
        data[i] = start - i;
    }
    return data;
}

std::vector<int> DataGenerator::generateWithDuplicates(int count, int unique_count) {
    std::vector<int> data(count);
    std::uniform_int_distribution<int> dist(0, unique_count - 1);
    for (int i = 0; i < count; i++) {
        data[i] = dist(rng);
    }
    return data;
}