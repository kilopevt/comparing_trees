#pragma once
// include/Statistics.h
#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <map>
#include <string>

#include "TreeBase.h"

struct DetailedStatistics {
    // Общие метрики
    TreeType treeType;
    std::string treeName;

    // Метрики производительности
    struct TimeMetrics {
        long long insertTime = 0;
        long long searchTime = 0;
        long long deleteTime = 0;
        long long traversalTime = 0;
        int operationCount = 0;
    } timeMetrics;

    // Метрики структуры
    struct StructureMetrics {
        int maxHeight = 0;
        int minHeight = 0;
        double avgHeight = 0.0;
        int nodeCount = 0;
        int leafCount = 0;
        double balanceFactor = 0.0; // Коэффициент балансировки
    } structureMetrics;

    // AVL метрики
    int avlRotations = 0;
    int avlBalanceOperations = 0;

    // AA метрики
    int aaSkewOperations = 0;
    int aaSplitOperations = 0;

    // Treap метрики
    int treapSplitOperations = 0;
    int treapMergeOperations = 0;
    int treapPriorityChanges = 0;

    // 2-3 дерево метрики
    int twoThreeSplits = 0;
    int twoThreeMerges = 0;
    int twoThreeBorrows = 0;

    // Временная статистика по операциям
    std::vector<long long> insertTimes;
    std::vector<long long> searchTimes;
    std::vector<long long> deleteTimes;

    // Методы
    void reset();
    void addInsertTime(long long time);
    void addSearchTime(long long time);
    void addDeleteTime(long long time);

    std::string generateReport() const;
    std::map<std::string, double> getMetricsMap() const;
};

class StatisticsCollector {
private:
    std::map<TreeType, DetailedStatistics> statistics;

public:
    StatisticsCollector();

    // Регистрация операций
    void registerInsert(TreeBase* tree, long long time);
    void registerSearch(TreeBase* tree, long long time);
    void registerDelete(TreeBase* tree, long long time);
    void registerTraversal(TreeBase* tree, long long time);

    // Обновление структурных метрик
    void updateStructureMetrics(TreeBase* tree);

    // Получение статистики
    const DetailedStatistics& getStatistics(TreeType type) const;
    std::vector<DetailedStatistics> getAllStatistics() const;

    // Экспорт
    void exportToCSV(const std::string& filename) const;
    void exportToJSON(const std::string& filename) const;

    // Очистка
    void clear();
};

#endif // STATISTICS_H