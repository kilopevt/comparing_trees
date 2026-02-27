#pragma once
// include/AVLTree.h
#ifndef AVLTREE_H
#define AVLTREE_H

#include "TreeBase.h"

class AVLNode {
private:
    int key;
    int height;
    AVLNode* left;
    AVLNode* right;

public:
    // Конструктор
    AVLNode(int k) : key(k), height(1), left(nullptr), right(nullptr) {}

    // Геттеры
    int getKey() const { return key; }
    int getHeight() const { return height; }
    AVLNode* getLeft() const { return left; }
    AVLNode* getRight() const { return right; }

    // Сеттеры
    void setLeft(AVLNode* node) { left = node; }
    void setRight(AVLNode* node) { right = node; }
    void setHeight(int h) { height = h; }
    void setKey(int k) { key = k; }
};

class AVLTree : public TreeBase {
private:
    AVLNode* root;
    mutable TreeStatistics stats;

    // Вспомогательные методы
    int getHeight(AVLNode* node) const;
    int getBalanceFactor(AVLNode* node) const;
    void updateHeight(AVLNode* node);

    // Повороты
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* balance(AVLNode* node);

    // Рекурсивные операции
    AVLNode* insert(AVLNode* node, int key, bool& success);
    AVLNode* remove(AVLNode* node, int key, bool& success);
    bool search(AVLNode* node, int key) const;
    void clear(AVLNode* node);

    int getNodeCount(AVLNode* node) const;

    // Обходы
    void inorder(AVLNode* node, std::vector<int>& result) const;
    void preorder(AVLNode* node, std::vector<int>& result) const;
    void postorder(AVLNode* node, std::vector<int>& result) const;
    void levelOrder(AVLNode* node, std::vector<int>& result) const;

    // Визуализация
    std::string toString(AVLNode* node, const std::string& prefix, bool isLeft) const;

public:
    AVLTree();
    ~AVLTree();

    void getStructure(AVLNode* node, std::string& result, const std::string& prefix, const std::string& childrenPrefix) const;

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

    TreeType getType() const override { return TreeType::AVL; }
};

#endif // AVLTREE_H