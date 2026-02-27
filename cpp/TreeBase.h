#pragma once
// include/TreeBase.h
#ifndef TREEBASE_H
#define TREEBASE_H

#include <chrono>
#include <vector>
#include <memory>
#include <string>

#include "CommonTypes.h"

class TreeBase {
public:
    virtual ~TreeBase() = default;

    // Основные операции
    virtual OperationResult insert(int key) = 0;
    virtual OperationResult remove(int key) = 0;
    virtual OperationResult search(int key) = 0;
    virtual OperationResult clear() = 0;

    // Обходы дерева
    virtual std::vector<int> traverse(TraversalType type) = 0;

    // Информация о дереве
    virtual int getHeight() const = 0;
    virtual int getNodeCount() const = 0;
    virtual bool isEmpty() const = 0;

    // Визуализация
    virtual std::string toString() const = 0;

    // Статистика
    virtual void resetStatistics() = 0;
    virtual TreeStatistics getStatistics() const = 0;

    // Геттер типа дерева
    virtual TreeType getType() const = 0;

protected:
    // Вспомогательные методы для замера времени
    template<typename Func, typename... Args>
    OperationResult measureOperation(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        bool success = func(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();

        OperationResult result;
        result.success = success;
        result.time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        result.height = getHeight();
        result.nodes_count = getNodeCount();

        return result;
    }
};

#endif // TREEBASE_H