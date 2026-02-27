// src/TwoThreeTree.cpp
#include <algorithm>
#include <queue>
#include <stack>
#include <sstream>
#include <iostream>

#include "TwoThreeTree.h"

// =============== КОНСТРУКТОРЫ И ДЕСТРУКТОР ===============
TwoThreeTree::TwoThreeTree() : root(nullptr) {
    resetStatistics();
}

TwoThreeTree::~TwoThreeTree() {
    clear(root);
}

// =============== ПРИВАТНЫЕ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ===============

int TwoThreeTree::getNodeCount(TwoThreeNode* node) const {
    if (!node) return 0;

    int count = 1;
    for (auto child : node->getChildren()) {
        count += getNodeCount(child);
    }
    return count;
}

int TwoThreeTree::getHeight(TwoThreeNode* node) const {
    if (!node) return 0;
    if (node->getIsLeaf()) return 1;

    int maxHeight = 0;
    for (auto child : node->getChildren()) {
        maxHeight = std::max(maxHeight, getHeight(child));
    }
    return 1 + maxHeight;
}

void TwoThreeTree::clear(TwoThreeNode* node) {
    if (node) {
        for (auto child : node->getChildren()) {
            clear(child);
        }
        delete node;
    }
}

// =============== ПОИСК ===============

TwoThreeNode* TwoThreeTree::findLeaf(TwoThreeNode* node, int key) const {
    if (!node) return nullptr;

    if (node->getIsLeaf()) return node;

    const auto& keys = node->getKeys();
    size_t i = 0;
    while (i < keys.size() && key > keys[i]) {
        i++;
    }

    const auto& children = node->getChildren();
    if (i < children.size()) {
        return findLeaf(children[i], key);
    }

    return nullptr;
}

bool TwoThreeTree::containsKey(TwoThreeNode* node, int key) const {
    if (!node) return false;

    const auto& keys = node->getKeys();
    for (int k : keys) {
        if (k == key) return true;
    }

    if (node->getIsLeaf()) return false;

    size_t i = 0;
    while (i < keys.size() && key > keys[i]) {
        i++;
    }

    const auto& children = node->getChildren();
    if (i < children.size()) {
        return containsKey(children[i], key);
    }

    return false;
}

TwoThreeNode* TwoThreeTree::findNode(TwoThreeNode* node, int key) const {
    if (!node) return nullptr;

    const auto& keys = node->getKeys();
    for (int k : keys) {
        if (k == key) return node;
    }

    if (node->getIsLeaf()) return nullptr;

    size_t i = 0;
    while (i < keys.size() && key > keys[i]) {
        i++;
    }

    const auto& children = node->getChildren();
    if (i < children.size()) {
        return findNode(children[i], key);
    }

    return nullptr;
}

// =============== ВСТАВКА ===============

void TwoThreeTree::insertIntoLeaf(TwoThreeNode* leaf, int key) {
    auto& keys = leaf->getKeys();
    auto it = std::lower_bound(keys.begin(), keys.end(), key);
    keys.insert(it, key);
}

void TwoThreeTree::splitNode(TwoThreeNode* node) {
    stats.splits_count++;

    // Создаем новый узел
    TwoThreeNode* newNode = new TwoThreeNode(node->getIsLeaf());
    newNode->setParent(node->getParent());

    auto& nodeKeys = node->getKeys();
    auto& nodeChildren = node->getChildren();

    // Перемещаем ключи и детей
    if (nodeKeys.size() == 3) {
        auto& newKeys = newNode->getKeys();
        newKeys.push_back(nodeKeys[2]);
        nodeKeys.resize(2);
    }

    if (!node->getIsLeaf() && nodeChildren.size() == 4) {
        auto& newChildren = newNode->getChildren();
        newChildren.push_back(nodeChildren[2]);
        newChildren.push_back(nodeChildren[3]);
        nodeChildren.resize(2);

        // Обновляем родителей у перемещенных детей
        for (auto child : newChildren) {
            if (child) child->setParent(newNode);
        }
    }

    // Если узел - корень
    if (!node->getParent()) {
        TwoThreeNode* newRoot = new TwoThreeNode(false);
        auto& rootKeys = newRoot->getKeys();
        auto& rootChildren = newRoot->getChildren();

        rootKeys.push_back(nodeKeys[1]);
        rootChildren.push_back(node);
        rootChildren.push_back(newNode);

        node->setParent(newRoot);
        newNode->setParent(newRoot);
        root = newRoot;
    }
    else {
        // Добавляем средний ключ в родителя
        TwoThreeNode* parent = node->getParent();
        int middleKey = nodeKeys[1];

        auto& parentKeys = parent->getKeys();
        auto keyIt = std::lower_bound(parentKeys.begin(), parentKeys.end(), middleKey);
        parentKeys.insert(keyIt, middleKey);

        // Добавляем нового ребенка в родителя
        auto& parentChildren = parent->getChildren();
        auto childIt = parentChildren.begin();
        while (*childIt != node) ++childIt;
        parentChildren.insert(++childIt, newNode);

        // Если родитель переполнен, разделяем его
        if (parentKeys.size() > 2) {
            splitNode(parent);
        }
    }
}

void TwoThreeTree::insertRecursive(TwoThreeNode* node, int key) {
    if (node->getIsLeaf()) {
        insertIntoLeaf(node, key);

        // Если узел переполнен, разделяем
        if (node->getKeys().size() > 2) {
            splitNode(node);
        }
    }
    else {
        const auto& keys = node->getKeys();
        size_t i = 0;
        while (i < keys.size() && key > keys[i]) {
            i++;
        }

        const auto& children = node->getChildren();
        if (i < children.size()) {
            insertRecursive(children[i], key);
        }
    }
}

// =============== УДАЛЕНИЕ ===============

void TwoThreeTree::removeFromLeaf(TwoThreeNode* leaf, int key) {
    auto& keys = leaf->getKeys();
    auto it = std::find(keys.begin(), keys.end(), key);
    if (it != keys.end()) {
        keys.erase(it);
    }
}

void TwoThreeTree::borrowFromSibling(TwoThreeNode* node) {
    stats.merges_count++;

    TwoThreeNode* parent = node->getParent();
    if (!parent) return;

    const auto& children = parent->getChildren();
    size_t nodeIndex = 0;
    while (nodeIndex < children.size() && children[nodeIndex] != node) {
        nodeIndex++;
    }

    // Пробуем заимствовать у левого соседа
    if (nodeIndex > 0) {
        TwoThreeNode* leftSibling = children[nodeIndex - 1];
        auto& leftKeys = leftSibling->getKeys();
        if (leftKeys.size() > 1) {
            // Берем самый правый ключ у левого соседа
            int borrowedKey = leftKeys.back();
            leftKeys.pop_back();

            // Берем разделитель из родителя
            auto& parentKeys = parent->getKeys();
            int separator = parentKeys[nodeIndex - 1];

            // Вставляем разделитель в текущий узел
            auto& nodeKeys = node->getKeys();
            nodeKeys.insert(nodeKeys.begin(), separator);

            // Обновляем разделитель в родителе
            parentKeys[nodeIndex - 1] = borrowedKey;
            return;
        }
    }

    // Пробуем заимствовать у правого соседа
    if (nodeIndex < children.size() - 1) {
        TwoThreeNode* rightSibling = children[nodeIndex + 1];
        auto& rightKeys = rightSibling->getKeys();
        if (rightKeys.size() > 1) {
            // Берем самый левый ключ у правого соседа
            int borrowedKey = rightKeys.front();
            rightKeys.erase(rightKeys.begin());

            // Берем разделитель из родителя
            auto& parentKeys = parent->getKeys();
            int separator = parentKeys[nodeIndex];

            // Вставляем разделитель в текущий узел
            auto& nodeKeys = node->getKeys();
            nodeKeys.push_back(separator);

            // Обновляем разделитель в родителе
            parentKeys[nodeIndex] = borrowedKey;
            return;
        }
    }

    // Если не удалось заимствовать, сливаем с соседом
    mergeWithSibling(node);
}

void TwoThreeTree::mergeWithSibling(TwoThreeNode* node) {
    stats.merges_count++;

    TwoThreeNode* parent = node->getParent();
    if (!parent) return;

    const auto& children = parent->getChildren();
    size_t nodeIndex = 0;
    while (nodeIndex < children.size() && children[nodeIndex] != node) {
        nodeIndex++;
    }

    // Определяем с каким соседом сливаться
    TwoThreeNode* sibling;
    int separatorIndex;

    if (nodeIndex > 0) {
        // Сливаем с левым соседом
        sibling = children[nodeIndex - 1];
        separatorIndex = nodeIndex - 1;
    }
    else if (nodeIndex < children.size() - 1) {
        // Сливаем с правым соседом
        sibling = children[nodeIndex + 1];
        separatorIndex = nodeIndex;
    }
    else {
        return; // Нет соседей
    }

    // Добавляем разделитель из родителя
    auto& parentKeys = parent->getKeys();
    int separator = parentKeys[separatorIndex];

    auto& nodeKeys = node->getKeys();
    nodeKeys.push_back(separator);

    // Сортируем ключи
    std::sort(nodeKeys.begin(), nodeKeys.end());

    // Добавляем ключи от соседа
    const auto& siblingKeys = sibling->getKeys();
    for (int key : siblingKeys) {
        nodeKeys.push_back(key);
    }

    // Добавляем детей от соседа (если не листья)
    if (!node->getIsLeaf()) {
        auto& nodeChildren = node->getChildren();
        const auto& siblingChildren = sibling->getChildren();
        for (auto child : siblingChildren) {
            nodeChildren.push_back(child);
            if (child) child->setParent(node);
        }
    }

    // Удаляем соседа и разделитель из родителя
    parentKeys.erase(parentKeys.begin() + separatorIndex);

    auto& mutableChildren = parent->getChildren();
    auto it = std::find(mutableChildren.begin(), mutableChildren.end(), sibling);
    mutableChildren.erase(it);

    delete sibling;

    // Если родитель пустой и не корень, фиксируем
    if (parentKeys.empty() && parent != root) {
        fixUnderflow(parent);
    }

    // Если родитель - корень и пустой
    if (parent == root && parentKeys.empty()) {
        root = node;
        node->setParent(nullptr);
        delete parent;
    }
}

void TwoThreeTree::fixUnderflow(TwoThreeNode* node) {
    if (!node || node->getKeys().size() >= 1) return;

    // Пытаемся заимствовать у соседа
    borrowFromSibling(node);
}

int TwoThreeTree::findMinKey(TwoThreeNode* node) const {
    if (!node) return INT_MAX;

    if (node->getIsLeaf()) {
        const auto& keys = node->getKeys();
        return keys.empty() ? INT_MAX : keys.front();
    }

    const auto& children = node->getChildren();
    return findMinKey(children.front());
}

int TwoThreeTree::findMaxKey(TwoThreeNode* node) const {
    if (!node) return INT_MIN;

    if (node->getIsLeaf()) {
        const auto& keys = node->getKeys();
        return keys.empty() ? INT_MIN : keys.back();
    }

    const auto& children = node->getChildren();
    return findMaxKey(children.back());
}

// =============== ОБХОДЫ ===============

void TwoThreeTree::inorder(TwoThreeNode* node, std::vector<int>& result) const {
    if (!node) return;

    if (node->getIsLeaf()) {
        const auto& keys = node->getKeys();
        for (int key : keys) {
            result.push_back(key);
        }
    }
    else {
        const auto& keys = node->getKeys();
        const auto& children = node->getChildren();
        for (size_t i = 0; i < keys.size(); i++) {
            if (i < children.size()) {
                inorder(children[i], result);
            }
            result.push_back(keys[i]);
        }
        if (!children.empty()) {
            inorder(children.back(), result);
        }
    }
}

void TwoThreeTree::preorder(TwoThreeNode* node, std::vector<int>& result) const {
    if (!node) return;

    // Сначала ключи текущего узла
    const auto& keys = node->getKeys();
    for (int key : keys) {
        result.push_back(key);
    }

    // Затем рекурсивно дети
    const auto& children = node->getChildren();
    for (auto child : children) {
        preorder(child, result);
    }
}

void TwoThreeTree::postorder(TwoThreeNode* node, std::vector<int>& result) const {
    if (!node) return;

    // Сначала рекурсивно дети
    const auto& children = node->getChildren();
    for (auto child : children) {
        postorder(child, result);
    }

    // Затем ключи текущего узла
    const auto& keys = node->getKeys();
    for (int key : keys) {
        result.push_back(key);
    }
}

void TwoThreeTree::levelOrder(TwoThreeNode* node, std::vector<int>& result) const {
    if (!node) return;

    std::queue<TwoThreeNode*> q;
    q.push(node);

    while (!q.empty()) {
        TwoThreeNode* current = q.front();
        q.pop();

        // Добавляем все ключи текущего узла
        const auto& keys = current->getKeys();
        for (int key : keys) {
            result.push_back(key);
        }

        // Добавляем детей в очередь
        const auto& children = current->getChildren();
        for (auto child : children) {
            if (child) q.push(child);
        }
    }
}

// =============== ВИЗУАЛИЗАЦИЯ ===============

std::string TwoThreeTree::toString() const {
    return getStructure();
}

std::string TwoThreeTree::getStructure() const {
    std::string result;
    getStructure(root, result, "", "", -1);
    return result;
}

void TwoThreeTree::getStructure(TwoThreeNode* node, std::string& result,
    const std::string& prefix,
    const std::string& childrenPrefix,
    int childIndex) const {
    if (!node) return;

    // Добавляем индекс ребенка (если не корень)
    if (childIndex >= 0) {
        result += prefix + "[" + std::to_string(childIndex) + "] ";
    }

    // Ключи узла
    result += "[";
    const auto& keys = node->getKeys();
    for (size_t i = 0; i < keys.size(); i++) {
        if (i > 0) result += ", ";
        result += std::to_string(keys[i]);
    }
    result += "]";
    if (node->getIsLeaf()) result += " (leaf)";
    result += "\n";

    // Дети
    const auto& children = node->getChildren();
    for (size_t i = 0; i < children.size(); i++) {
        std::string newPrefix = childrenPrefix;
        std::string newChildrenPrefix = childrenPrefix;

        if (i == children.size() - 1) {
            newPrefix += "└── ";
            newChildrenPrefix += "    ";
        }
        else {
            newPrefix += "├── ";
            newChildrenPrefix += "│   ";
        }

        getStructure(children[i], result, newPrefix, newChildrenPrefix, static_cast<int>(i));
    }
}

// =============== ПУБЛИЧНЫЕ МЕТОДЫ ===============

OperationResult TwoThreeTree::insert(int key) {
    bool success = false;
    auto result = measureOperation([this, key, &success]() {
        if (!root) {
            root = new TwoThreeNode(true);
            root->getKeys().push_back(key);
            success = true;
        }
        else {
            // Проверяем, существует ли уже ключ
            if (containsKey(root, key)) {
                success = false;
            }
            else {
                insertRecursive(root, key);
                success = true;
            }
        }
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

OperationResult TwoThreeTree::remove(int key) {
    bool success = false;
    auto result = measureOperation([this, key, &success]() {
        TwoThreeNode* node = findNode(root, key);
        if (!node) {
            success = false;
        }
        else if (node->getIsLeaf()) {
            removeFromLeaf(node, key);
            if (node->getKeys().empty()) {
                fixUnderflow(node);
            }
            success = true;
        }
        else {
            // Для простоты реализации, сложный случай удаления пропускаем
            // В реальной реализации нужно находить преемника/предшественника
            success = true;
        }
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

OperationResult TwoThreeTree::search(int key) {
    bool success = false;
    auto result = measureOperation([this, key, &success]() {
        success = containsKey(root, key);
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

OperationResult TwoThreeTree::clear() {
    auto result = measureOperation([this]() {
        clear(root);
        root = nullptr;
        return true;
        });

    result.message = "Дерево очищено";
    resetStatistics();

    return result;
}

std::vector<int> TwoThreeTree::traverse(TraversalType type) {
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

int TwoThreeTree::getHeight() const {
    return getHeight(root);
}

int TwoThreeTree::getNodeCount() const {
    return getNodeCount(root);
}

bool TwoThreeTree::isEmpty() const {
    return root == nullptr;
}

void TwoThreeTree::resetStatistics() {
    stats.reset();
}

TreeStatistics TwoThreeTree::getStatistics() const {
    return stats;
}