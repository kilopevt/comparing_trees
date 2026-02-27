#pragma once
// include/CommonTypes.h
#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <string>

// Типы деревьев
enum class TreeType {
    AVL,
    AA,
    TREAP,
    TWO_THREE,
    UNKNOWN
};

// Типы обхода
enum class TraversalType {
    INORDER,     // Симметричный (левый, корень, правый)
    PREORDER,    // Прямой (корень, левый, правый)
    POSTORDER,   // Обратный (левый, правый, корень)
    LEVELORDER   // В ширину
};

// Типы данных для тестов
enum class DataOrder {
    RANDOM,          // Случайные значения
    ASCENDING,       // По возрастанию
    DESCENDING,      // По убыванию
    ALMOST_SORTED,   // 90% отсортировано
    SORTED_WITH_DUPLICATES, // Сортированные с дубликатами
    REVERSE_ALMOST_SORTED   // Почти отсортированные в обратном порядке
};

// Типы операций для статистики
enum class OperationType {
    INSERT,
    SEARCH,
    DELETE,
    TRAVERSAL,
    BALANCE  // Балансировка (повороты и т.д.)
};

// Структура для результатов операции
struct OperationResult {
    bool success = false;
    long long time_ns = 0;          // Время выполнения в наносекундах
    int height = 0;                 // Высота дерева после операции
    int nodes_count = 0;            // Количество узлов после операции
    int extra_operations = 0;       // Доп. операции (повороты, расщепления и т.д.)
    std::string message;            // Сообщение об ошибке или успехе

    OperationResult() = default;
    OperationResult(bool s, long long t, int h, int n)
        : success(s), time_ns(t), height(h), nodes_count(n) {}
};

// Структура для статистики дерева
struct TreeStatistics {
    // Основные счетчики операций
    int insert_count = 0;
    int remove_count = 0;
    int search_count = 0;
    int traversal_count = 0;

    // Специфичные операции для разных деревьев
    int rotations_count = 0;        // Для АВЛ и АА деревьев
    int splits_count = 0;           // Для 2-3 деревьев и Treap
    int merges_count = 0;           // Для 2-3 деревьев
    int skew_count = 0;             // Для АА деревьев
    int split_count = 0;            // Для АА деревьев

    // Временная статистика
    long long total_insert_time = 0;
    long long total_search_time = 0;
    long long total_delete_time = 0;

    // Методы
    void reset() {
        insert_count = remove_count = search_count = traversal_count = 0;
        rotations_count = splits_count = merges_count = skew_count = split_count = 0;
        total_insert_time = total_search_time = total_delete_time = 0;
    }

    double getAverageInsertTime() const {
        return insert_count > 0 ? static_cast<double>(total_insert_time) / insert_count : 0.0;
    }

    double getAverageSearchTime() const {
        return search_count > 0 ? static_cast<double>(total_search_time) / search_count : 0.0;
    }

    double getAverageDeleteTime() const {
        return remove_count > 0 ? static_cast<double>(total_delete_time) / remove_count : 0.0;
    }
};

#endif // COMMONTYPES_H