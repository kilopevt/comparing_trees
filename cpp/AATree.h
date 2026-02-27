#pragma once
// include/AATree.h
#ifndef AATREE_H
#define AATREE_H

#include "TreeBase.h"

// Узел АА-дерева
class AANode {
private:
    int key;
    int level;
    AANode* left;
    AANode* right;

public:
    AANode(int k, int lvl = 1) : key(k), level(lvl), left(nullptr), right(nullptr) {}

    // Геттеры
    int getKey() const { return key; }
    int getLevel() const { return level; }
    AANode* getLeft() const { return left; }
    AANode* getRight() const { return right; }

    // Сеттеры
    void setLeft(AANode* node) { left = node; }
    void setRight(AANode* node) { right = node; }
    void setLevel(int l) { level = l; }
    void setKey(int k) { key = k; }
};

class AATree : public TreeBase {
private:
    AANode* root;
    mutable TreeStatistics stats;

    // Вспомогательные методы
    int getNodeCount(AANode* node) const;
    int getHeight(AANode* node) const;
    void clear(AANode* node);

    // Базовые операции балансировки АА-дерева
    AANode* skew(AANode* node);
    AANode* split(AANode* node);
    AANode* decreaseLevel(AANode* node);

    // Рекурсивные операции
    AANode* insert(AANode* node, int key, bool& success);
    AANode* remove(AANode* node, int key, bool& success);
    bool search(AANode* node, int key) const;

    // Поиск минимального и максимального
    AANode* findMin(AANode* node) const;
    AANode* findMax(AANode* node) const;

    // Обходы
    void inorder(AANode* node, std::vector<int>& result) const;
    void preorder(AANode* node, std::vector<int>& result) const;
    void postorder(AANode* node, std::vector<int>& result) const;
    void levelOrder(AANode* node, std::vector<int>& result) const;

    // Визуализация
    void getStructure(AANode* node, std::string& result,
        const std::string& prefix,
        const std::string& childrenPrefix) const;

public:
    AATree();
    ~AATree();

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

    TreeType getType() const override { return TreeType::AA; }
};

#endif // AATREE_H