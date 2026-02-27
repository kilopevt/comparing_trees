#pragma once
// include/TwoThreeTree.h
#ifndef TWOTHREETREE_H
#define TWOTHREETREE_H

#include <vector>

#include "TreeBase.h"

// Узел 2-3 дерева
class TwoThreeNode {
private:
    bool isLeaf;
    std::vector<int> keys;
    std::vector<TwoThreeNode*> children;
    TwoThreeNode* parent;

public:
    TwoThreeNode(bool leaf = true) : isLeaf(leaf), parent(nullptr) {}

    // Геттеры
    bool getIsLeaf() const { return isLeaf; }
    const std::vector<int>& getKeys() const { return keys; }
    std::vector<int>& getKeys() { return keys; } // Для модификации
    const std::vector<TwoThreeNode*>& getChildren() const { return children; }
    std::vector<TwoThreeNode*>& getChildren() { return children; } // Для модификации
    TwoThreeNode* getParent() const { return parent; }

    // Сеттеры
    void setIsLeaf(bool leaf) { isLeaf = leaf; }
    void setParent(TwoThreeNode* p) { parent = p; }

    // Вспомогательные методы
    bool isFull() const { return keys.size() == 2; }
    bool isUnderflow() const { return keys.empty(); }
    int getKeyCount() const { return keys.size(); }
    int getChildCount() const { return children.size(); }
};

class TwoThreeTree : public TreeBase {
private:
    TwoThreeNode* root;
    mutable TreeStatistics stats;

    // Вспомогательные методы
    int getNodeCount(TwoThreeNode* node) const;
    int getHeight(TwoThreeNode* node) const;
    void clear(TwoThreeNode* node);

    // Поиск
    TwoThreeNode* findLeaf(TwoThreeNode* node, int key) const;
    bool containsKey(TwoThreeNode* node, int key) const;

    // Вставка
    void insertIntoLeaf(TwoThreeNode* leaf, int key);
    void splitNode(TwoThreeNode* node);
    void insertRecursive(TwoThreeNode* node, int key);

    // Удаление
    TwoThreeNode* findNode(TwoThreeNode* node, int key) const;
    void removeFromLeaf(TwoThreeNode* leaf, int key);
    void borrowFromSibling(TwoThreeNode* node);
    void mergeWithSibling(TwoThreeNode* node);
    void fixUnderflow(TwoThreeNode* node);

    // Поиск минимума/максимума
    int findMinKey(TwoThreeNode* node) const;
    int findMaxKey(TwoThreeNode* node) const;

    // Обходы
    void inorder(TwoThreeNode* node, std::vector<int>& result) const;
    void preorder(TwoThreeNode* node, std::vector<int>& result) const;
    void postorder(TwoThreeNode* node, std::vector<int>& result) const;
    void levelOrder(TwoThreeNode* node, std::vector<int>& result) const;

    // Визуализация
    void getStructure(TwoThreeNode* node, std::string& result,
        const std::string& prefix,
        const std::string& childrenPrefix,
        int childIndex = -1) const;

public:
    TwoThreeTree();
    ~TwoThreeTree();

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

    TreeType getType() const override { return TreeType::TWO_THREE; }
};

#endif // TWOTHREETREE_H