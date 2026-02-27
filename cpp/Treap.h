#pragma once
// include/Treap.h
#ifndef TREAP_H
#define TREAP_H

#include <random>

#include "TreeBase.h"

// Узел декартова дерева
class TreapNode {
private:
    int key;
    int priority;
    TreapNode* left;
    TreapNode* right;

public:
    TreapNode(int k, int prio) : key(k), priority(prio), left(nullptr), right(nullptr) {}

    // Геттеры
    int getKey() const { return key; }
    int getPriority() const { return priority; }
    TreapNode* getLeft() const { return left; }
    TreapNode* getRight() const { return right; }

    // Сеттеры
    void setLeft(TreapNode* node) { left = node; }
    void setRight(TreapNode* node) { right = node; }
    void setPriority(int p) { priority = p; }
    void setKey(int k) { key = k; }
};

class Treap : public TreeBase {
private:
    TreapNode* root;
    mutable TreeStatistics stats;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    // Вспомогательные методы
    int getNodeCount(TreapNode* node) const;
    int getHeight(TreapNode* node) const;
    void clear(TreapNode* node);

    // Операции декартова дерева
    std::pair<TreapNode*, TreapNode*> split(TreapNode* node, int key);
    TreapNode* merge(TreapNode* left, TreapNode* right);

    // Рекурсивные операции
    TreapNode* insert(TreapNode* node, int key, int priority, bool& success);
    TreapNode* remove(TreapNode* node, int key, bool& success);
    bool search(TreapNode* node, int key) const;

    // Обходы
    void inorder(TreapNode* node, std::vector<int>& result) const;
    void preorder(TreapNode* node, std::vector<int>& result) const;
    void postorder(TreapNode* node, std::vector<int>& result) const;
    void levelOrder(TreapNode* node, std::vector<int>& result) const;

    // Визуализация
    void getStructure(TreapNode* node, std::string& result,
        const std::string& prefix,
        const std::string& childrenPrefix) const;

public:
    Treap();
    ~Treap();

    // Реализация интерфейса TreeBase
    OperationResult insert(int key) override;
    OperationResult remove(int key) override;
    OperationResult search(int key) override;
    OperationResult clear() override;

    std::vector<int> traverse(TraversalType type) override;

    int getHeight() const override;
    int getNodeCount() const override;
    bool isEmpty() const override;

    std::string toString() const override;
    std::string getStructure() const;

    void resetStatistics() override;
    TreeStatistics getStatistics() const override;

    TreeType getType() const override { return TreeType::TREAP; }
};

#endif // TREAP_H