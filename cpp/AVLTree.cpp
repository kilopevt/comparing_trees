// src/AVLTree.cpp
#include <algorithm>
#include <queue>
#include <stack>
#include <sstream>
#include <iostream>

#include "AVLTree.h"

// =============== КОНСТРУКТОРЫ И ДЕСТРУКТОР ===============
AVLTree::AVLTree() : root(nullptr) {
    resetStatistics();
}

AVLTree::~AVLTree() {
    clear(root);
}

// =============== ПРИВАТНЫЕ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ===============

int AVLTree::getHeight(AVLNode* node) const {
    return node ? node->getHeight() : 0;
}

void AVLTree::updateHeight(AVLNode* node) {
    if (node) {
        node->setHeight(1 + std::max(getHeight(node->getLeft()), getHeight(node->getRight())));
    }
}

int AVLTree::getBalanceFactor(AVLNode* node) const {
    return node ? getHeight(node->getLeft()) - getHeight(node->getRight()) : 0;
}

// =============== ОПЕРАЦИИ БАЛАНСИРОВКИ ===============

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    stats.rotations_count++;

    AVLNode* x = y->getLeft();
    AVLNode* T2 = x->getRight();

    x->setRight(y);
    y->setLeft(T2);

    updateHeight(y);
    updateHeight(x);

    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    stats.rotations_count++;

    AVLNode* y = x->getRight();
    AVLNode* T2 = y->getLeft();

    y->setLeft(x);
    x->setRight(T2);

    updateHeight(x);
    updateHeight(y);

    return y;
}

AVLNode* AVLTree::balance(AVLNode* node) {
    if (!node) return nullptr;

    updateHeight(node);
    int balanceFactor = getBalanceFactor(node);

    // Left Left Case
    if (balanceFactor > 1 && getBalanceFactor(node->getLeft()) >= 0) {
        return rotateRight(node);
    }

    // Right Right Case
    if (balanceFactor < -1 && getBalanceFactor(node->getRight()) <= 0) {
        return rotateLeft(node);
    }

    // Left Right Case
    if (balanceFactor > 1 && getBalanceFactor(node->getLeft()) < 0) {
        node->setLeft(rotateLeft(node->getLeft()));
        return rotateRight(node);
    }

    // Right Left Case
    if (balanceFactor < -1 && getBalanceFactor(node->getRight()) > 0) {
        node->setRight(rotateRight(node->getRight()));
        return rotateLeft(node);
    }

    return node;
}

// =============== РЕКУРСИВНЫЕ ОПЕРАЦИИ ===============

AVLNode* AVLTree::insert(AVLNode* node, int key, bool& success) {
    if (!node) {
        success = true;
        return new AVLNode(key);
    }

    if (key < node->getKey()) {
        node->setLeft(insert(node->getLeft(), key, success));
    }
    else if (key > node->getKey()) {
        node->setRight(insert(node->getRight(), key, success));
    }
    else {
        success = false; // Дубликат
        return node;
    }

    return balance(node);
}

AVLNode* AVLTree::remove(AVLNode* node, int key, bool& success) {
    if (!node) {
        success = false;
        return nullptr;
    }

    if (key < node->getKey()) {
        node->setLeft(remove(node->getLeft(), key, success));
    }
    else if (key > node->getKey()) {
        node->setRight(remove(node->getRight(), key, success));
    }
    else {
        // Узел найден
        success = true;

        // Узел с одним или без детей
        if (!node->getLeft() || !node->getRight()) {
            AVLNode* temp = node->getLeft() ? node->getLeft() : node->getRight();

            // Нет детей
            if (!temp) {
                temp = node;
                node = nullptr;
            }
            else {
                // Один ребенок: копируем содержимое
                node->setKey(temp->getKey());
                node->setHeight(temp->getHeight());
                node->setLeft(temp->getLeft());
                node->setRight(temp->getRight());
            }
            delete temp;
        }
        else {
            // Два ребенка: находим преемника (минимальный в правом поддереве)
            AVLNode* temp = node->getRight();
            while (temp->getLeft()) {
                temp = temp->getLeft();
            }

            // Копируем ключ преемника
            node->setKey(temp->getKey());

            // Удаляем преемника
            bool tempSuccess;
            node->setRight(remove(node->getRight(), temp->getKey(), tempSuccess));
        }
    }

    // Если дерево стало пустым
    if (!node) {
        return nullptr;
    }

    return balance(node);
}

bool AVLTree::search(AVLNode* node, int key) const {
    if (!node) return false;

    if (key == node->getKey()) return true;
    if (key < node->getKey()) return search(node->getLeft(), key);
    return search(node->getRight(), key);
}

void AVLTree::clear(AVLNode* node) {
    if (node) {
        clear(node->getLeft());
        clear(node->getRight());
        delete node;
    }
}

int AVLTree::getNodeCount(AVLNode* node) const {
    if (!node) return 0;
    return 1 + getNodeCount(node->getLeft()) + getNodeCount(node->getRight());
}

// =============== ОБХОДЫ ДЕРЕВА ===============

void AVLTree::inorder(AVLNode* node, std::vector<int>& result) const {
    if (node) {
        inorder(node->getLeft(), result);
        result.push_back(node->getKey());
        inorder(node->getRight(), result);
    }
}

void AVLTree::preorder(AVLNode* node, std::vector<int>& result) const {
    if (node) {
        result.push_back(node->getKey());
        preorder(node->getLeft(), result);
        preorder(node->getRight(), result);
    }
}

void AVLTree::postorder(AVLNode* node, std::vector<int>& result) const {
    if (node) {
        postorder(node->getLeft(), result);
        postorder(node->getRight(), result);
        result.push_back(node->getKey());
    }
}

void AVLTree::levelOrder(AVLNode* node, std::vector<int>& result) const {
    if (!node) return;

    std::queue<AVLNode*> q;
    q.push(node);

    while (!q.empty()) {
        AVLNode* current = q.front();
        q.pop();

        result.push_back(current->getKey());

        if (current->getLeft()) q.push(current->getLeft());
        if (current->getRight()) q.push(current->getRight());
    }
}

// =============== ВИЗУАЛИЗАЦИЯ ===============

std::string AVLTree::toString() const {
    return getStructure();
}

std::string AVLTree::getStructure() const {
    std::string result;
    getStructure(root, result, "", "");
    return result;
}

void AVLTree::getStructure(AVLNode* node, std::string& result,
    const std::string& prefix,
    const std::string& childrenPrefix) const {
    if (!node) return;

    result += prefix + std::to_string(node->getKey()) +
        " (h=" + std::to_string(node->getHeight()) +
        ", bf=" + std::to_string(getBalanceFactor(node)) + ")\n";

    if (node->getLeft() || node->getRight()) {
        if (node->getLeft()) {
            getStructure(node->getLeft(), result,
                childrenPrefix + "├── ",
                childrenPrefix + "│   ");
        }
        if (node->getRight()) {
            getStructure(node->getRight(), result,
                childrenPrefix + "└── ",
                childrenPrefix + "    ");
        }
    }
}

// =============== ПУБЛИЧНЫЕ МЕТОДЫ (ИНТЕРФЕЙС TreeBase) ===============

OperationResult AVLTree::insert(int key) {
    bool success = false;
    auto result = measureOperation([this, key, &success]() {
        root = insert(root, key, success);
        return success;
        });

    if (success) {
        stats.insert_count++;
        result.message = "Вставка выполнена успешно";
    }
    else {
        result.message = "Ключ уже существует";
    }

    return result;
}

OperationResult AVLTree::remove(int key) {
    bool success = false;
    auto result = measureOperation([this, key, &success]() {
        root = remove(root, key, success);
        return success;
        });

    if (success) {
        stats.remove_count++;
        result.message = "Удаление выполнено успешно";
    }
    else {
        result.message = "Ключ не найден";
    }

    return result;
}

OperationResult AVLTree::search(int key) {
    bool success = false;
    auto result = measureOperation([this, key, &success]() {
        success = search(root, key);
        return success;
        });

    if (success) {
        stats.search_count++;
        result.message = "Ключ найден";
    }
    else {
        result.message = "Ключ не найден";
    }

    return result;
}

OperationResult AVLTree::clear() {
    auto result = measureOperation([this]() {
        clear(root);
        root = nullptr;
        return true;
        });

    result.message = "Дерево очищено";
    resetStatistics();

    return result;
}

std::vector<int> AVLTree::traverse(TraversalType type) {
    std::vector<int> result;

    switch (type) {
    case TraversalType::INORDER:
        inorder(root, result);
        break;
    case TraversalType::PREORDER:
        preorder(root, result);
        break;
    case TraversalType::POSTORDER:
        postorder(root, result);
        break;
    case TraversalType::LEVELORDER:
        levelOrder(root, result);
        break;
    }

    stats.traversal_count++;
    return result;
}

int AVLTree::getHeight() const {
    return getHeight(root);
}

int AVLTree::getNodeCount() const {
    return getNodeCount(root);
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

void AVLTree::resetStatistics() {
    stats.reset();
}

TreeStatistics AVLTree::getStatistics() const {
    return stats;
}