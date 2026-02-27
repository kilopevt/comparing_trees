// src/AATree.cpp
#include <algorithm>
#include <queue>
#include <stack>
#include <sstream>
#include <iostream>

#include "AATree.h"

// =============== КОНСТРУКТОРЫ И ДЕСТРУКТОР ===============
AATree::AATree() : root(nullptr) {
    resetStatistics();
}

AATree::~AATree() {
    clear(root);
}

// =============== ОСНОВНЫЕ ОПЕРАЦИИ АА-ДЕРЕВА ===============

AANode* AATree::skew(AANode* node) {
    if (!node || !node->getLeft()) return node;

    if (node->getLeft()->getLevel() == node->getLevel()) {
        stats.skew_count++;

        AANode* L = node->getLeft();
        node->setLeft(L->getRight());
        L->setRight(node);
        return L;
    }
    return node;
}

AANode* AATree::split(AANode* node) {
    if (!node || !node->getRight() || !node->getRight()->getRight()) return node;

    if (node->getLevel() == node->getRight()->getRight()->getLevel()) {
        stats.split_count++;

        AANode* R = node->getRight();
        node->setRight(R->getLeft());
        R->setLeft(node);
        R->setLevel(R->getLevel() + 1);
        return R;
    }
    return node;
}

AANode* AATree::decreaseLevel(AANode* node) {
    if (!node) return nullptr;

    int leftLevel = node->getLeft() ? node->getLeft()->getLevel() : 0;
    int rightLevel = node->getRight() ? node->getRight()->getLevel() : 0;
    int expectedLevel = 1 + std::min(leftLevel, rightLevel);

    if (expectedLevel < node->getLevel()) {
        node->setLevel(expectedLevel);
        if (node->getRight() && node->getRight()->getLevel() > expectedLevel) {
            node->getRight()->setLevel(expectedLevel);
        }
    }
    return node;
}

// =============== РЕКУРСИВНЫЕ ОПЕРАЦИИ ===============

AANode* AATree::insert(AANode* node, int key, bool& success) {
    if (!node) {
        success = true;
        return new AANode(key);
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

    node = skew(node);
    node = split(node);
    return node;
}

AANode* AATree::remove(AANode* node, int key, bool& success) {
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

        // Лист
        if (!node->getLeft() && !node->getRight()) {
            delete node;
            return nullptr;
        }

        // Узел с одним ребенком
        if (!node->getLeft()) {
            AANode* temp = node->getRight();
            delete node;
            return temp;
        }

        if (!node->getRight()) {
            AANode* temp = node->getLeft();
            delete node;
            return temp;
        }

        // Узел с двумя детьми: находим преемника
        AANode* successor = node->getRight();
        while (successor->getLeft()) {
            successor = successor->getLeft();
        }

        // Заменяем ключ
        node->setKey(successor->getKey());

        // Удаляем преемника
        bool tempSuccess;
        node->setRight(remove(node->getRight(), successor->getKey(), tempSuccess));
    }

    node = decreaseLevel(node);
    node = skew(node);
    if (node->getRight()) {
        node->setRight(skew(node->getRight()));
        if (node->getRight()->getRight()) {
            node->getRight()->setRight(skew(node->getRight()->getRight()));
        }
    }

    node = split(node);
    if (node->getRight()) {
        node->setRight(split(node->getRight()));
    }

    return node;
}

bool AATree::search(AANode* node, int key) const {
    if (!node) return false;

    if (key == node->getKey()) return true;
    if (key < node->getKey()) return search(node->getLeft(), key);
    return search(node->getRight(), key);
}

void AATree::clear(AANode* node) {
    if (node) {
        clear(node->getLeft());
        clear(node->getRight());
        delete node;
    }
}

int AATree::getNodeCount(AANode* node) const {
    if (!node) return 0;
    return 1 + getNodeCount(node->getLeft()) + getNodeCount(node->getRight());
}

int AATree::getHeight(AANode* node) const {
    if (!node) return 0;
    return 1 + std::max(getHeight(node->getLeft()), getHeight(node->getRight()));
}

AANode* AATree::findMin(AANode* node) const {
    if (!node) return nullptr;
    while (node->getLeft()) node = node->getLeft();
    return node;
}

AANode* AATree::findMax(AANode* node) const {
    if (!node) return nullptr;
    while (node->getRight()) node = node->getRight();
    return node;
}

// =============== ОБХОДЫ ===============

void AATree::inorder(AANode* node, std::vector<int>& result) const {
    if (node) {
        inorder(node->getLeft(), result);
        result.push_back(node->getKey());
        inorder(node->getRight(), result);
    }
}

void AATree::preorder(AANode* node, std::vector<int>& result) const {
    if (node) {
        result.push_back(node->getKey());
        preorder(node->getLeft(), result);
        preorder(node->getRight(), result);
    }
}

void AATree::postorder(AANode* node, std::vector<int>& result) const {
    if (node) {
        postorder(node->getLeft(), result);
        postorder(node->getRight(), result);
        result.push_back(node->getKey());
    }
}

void AATree::levelOrder(AANode* node, std::vector<int>& result) const {
    if (!node) return;

    std::queue<AANode*> q;
    q.push(node);

    while (!q.empty()) {
        AANode* current = q.front();
        q.pop();

        result.push_back(current->getKey());

        if (current->getLeft()) q.push(current->getLeft());
        if (current->getRight()) q.push(current->getRight());
    }
}

// =============== ВИЗУАЛИЗАЦИЯ ===============

std::string AATree::toString() const {
    return getStructure();
}

std::string AATree::getStructure() const {
    std::string result;
    getStructure(root, result, "", "");
    return result;
}

void AATree::getStructure(AANode* node, std::string& result,
    const std::string& prefix,
    const std::string& childrenPrefix) const {
    if (!node) return;

    result += prefix + std::to_string(node->getKey()) +
        " (lvl=" + std::to_string(node->getLevel()) + ")\n";

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

OperationResult AATree::insert(int key) {
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

OperationResult AATree::remove(int key) {
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

OperationResult AATree::search(int key) {
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

OperationResult AATree::clear() {
    auto result = measureOperation([this]() {
        clear(root);
        root = nullptr;
        return true;
        });

    result.message = "Дерево очищено";
    resetStatistics();

    return result;
}

std::vector<int> AATree::traverse(TraversalType type) {
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

int AATree::getHeight() const {
    return getHeight(root);
}

int AATree::getNodeCount() const {
    return getNodeCount(root);
}

bool AATree::isEmpty() const {
    return root == nullptr;
}

void AATree::resetStatistics() {
    stats.reset();
}

TreeStatistics AATree::getStatistics() const {
    return stats;
}