#pragma once
// include/DataGenerator.h
#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <algorithm>
#include <vector>
#include <random>

#include "CommonTypes.h"

class DataGenerator {
private:
    std::mt19937 rng;

public:
    DataGenerator(unsigned int seed = std::random_device{}());

    std::vector<int> generate(int count, DataOrder order,
        int min_value = 1, int max_value = 1000000);

    // Генерация специфических паттернов
    std::vector<int> generateSequential(int start, int count);
    std::vector<int> generateReverseSequential(int start, int count);
    std::vector<int> generateWithDuplicates(int count, int unique_count);
};

#endif // DATAGENERATOR_H