// src/Treap.cpp
#include <algorithm>
#include <iostream>
#include <queue>
#include <random>
#include <stack>
#include <sstream>

#include "Treap.h"

// =============== КОНСТРУКТОРЫ И ДЕСТРУКТОР ===============
Treap::Treap() : root(nullptr), rng(std::random_device{}()), dist(1, 1000000) {
    resetStatistics();
}

Treap::~Treap() {
    clear(root);
}

// =============== ОСНОВНЫЕ ОПЕРАЦИИ TREAP ===============

std::pair<TreapNode*, TreapNode*> Treap::split(TreapNode* node, int key) {
    stats.splits_count++;

    if (!node) return std::make_pair(nullptr, nullptr);

    if (node->getKey() < key) {
        std::pair<TreapNode*, TreapNode*> splitResult = split(node->getRight(), key);
        node->setRight(splitResult.first);
        return std::make_pair(node, splitResult.second);
    }
    else {
        std::pair<TreapNode*, TreapNode*> splitResult = split(node->getLeft(), key);
        node->setLeft(splitResult.second);
        return std::make_pair(splitResult.first, node);
    }
}

TreapNode* Treap::merge(TreapNode* left, TreapNode* right) {
    if (!left || !right) return left ? left : right;

    if (left->getPriority() > right->getPriority()) {
        left->setRight(merge(left->getRight(), right));
        return left;
    }
    else {
        right->setLeft(merge(left, right->getLeft()));
        return right;
    }
}

// =============== РЕКУРСИВНЫЕ ОПЕРАЦИИ ===============

TreapNode* Treap::insert(TreapNode* node, int key, int priority, bool& success) {
    if (!node) {
        success = true;
        return new TreapNode(key, priority);
    }

    if (key == node->getKey()) {
        success = false; // Дубликат
        return node;
    }

    if (key < node->getKey()) {
        node->setLeft(insert(node->getLeft(), key, priority, success));
        if (node->getLeft()->getPriority() > node->getPriority()) {
            // Поворот вправо
            TreapNode* temp = node->getLeft();
            node->setLeft(temp->getRight());
            temp->setRight(node);
            stats.rotations_count++;
            return temp;
        }
    }
    else {
        node->setRight(insert(node->getRight(), key, priority, success));
        if (node->getRight()->getPriority() > node->getPriority()) {
            // Поворот влево
            TreapNode* temp = node->getRight();
            node->setRight(temp->getLeft());
            temp->setLeft(node);
            stats.rotations_count++;
            return temp;
        }
    }

    return node;
}

TreapNode* Treap::remove(TreapNode* node, int key, bool& success) {
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
        TreapNode* temp = merge(node->getLeft(), node->getRight());
        delete node;
        return temp;
    }

    return node;
}

bool Treap::search(TreapNode* node, int key) const {
    if (!node) return false;

    if (key == node->getKey()) return true;
    if (key < node->getKey()) return search(node->getLeft(), key);
    return search(node->getRight(), key);
}

void Treap::clear(TreapNode* node) {
    if (node) {
        clear(node->getLeft());
        clear(node->getRight());
        delete node;
    }
}

int Treap::getNodeCount(TreapNode* node) const {
    if (!node) return 0;
    return 1 + getNodeCount(node->getLeft()) + getNodeCount(node->getRight());
}

int Treap::getHeight(TreapNode* node) const {
    if (!node) return 0;
    return 1 + std::max(getHeight(node->getLeft()), getHeight(node->getRight()));
}

// =============== ОБХОДЫ ===============

void Treap::inorder(TreapNode* node, std::vector<int>& result) const {
    if (node) {
        inorder(node->getLeft(), result);
        result.push_back(node->getKey());
        inorder(node->getRight(), result);
    }
}

void Treap::preorder(TreapNode* node, std::vector<int>& result) const {
    if (node) {
        result.push_back(node->getKey());
        preorder(node->getLeft(), result);
        preorder(node->getRight(), result);
    }
}

void Treap::postorder(TreapNode* node, std::vector<int>& result) const {
    if (node) {
        postorder(node->getLeft(), result);
        postorder(node->getRight(), result);
        result.push_back(node->getKey());
    }
}

void Treap::levelOrder(TreapNode* node, std::vector<int>& result) const {
    if (!node) return;

    std::queue<TreapNode*> q;
    q.push(node);

    while (!q.empty()) {
        TreapNode* current = q.front();
        q.pop();

        result.push_back(current->getKey());

        if (current->getLeft()) q.push(current->getLeft());
        if (current->getRight()) q.push(current->getRight());
    }
}

// =============== ВИЗУАЛИЗАЦИЯ ===============

std::string Treap::toString() const {
    return getStructure();
}

std::string Treap::getStructure() const {
    std::string result;
    getStructure(root, result, "", "");
    return result;
}

void Treap::getStructure(TreapNode* node, std::string& result,
    const std::string& prefix,
    const std::string& childrenPrefix) const {
    if (!node) return;

    result += prefix + std::to_string(node->getKey()) +
        " (prio=" + std::to_string(node->getPriority()) + ")\n";

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

// =============== ПУБЛИЧНЫЕ МЕТОДЫ ===============

OperationResult Treap::insert(int key) {
    bool success = false;
    int priority = dist(rng);
    auto result = measureOperation([this, key, priority, &success]() {
        root = insert(root, key, priority, success);
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

OperationResult Treap::remove(int key) {
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

OperationResult Treap::search(int key) {
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

OperationResult Treap::clear() {
    auto result = measureOperation([this]() {
        clear(root);
        root = nullptr;
        return true;
        });

    result.message = "Дерево очищено";
    resetStatistics();

    return result;
}

std::vector<int> Treap::traverse(TraversalType type) {
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

int Treap::getHeight() const {
    return getHeight(root);
}

int Treap::getNodeCount() const {
    return getNodeCount(root);
}

bool Treap::isEmpty() const {
    return root == nullptr;
}

void Treap::resetStatistics() {
    stats.reset();
}

TreeStatistics Treap::getStatistics() const {
    return stats;
}