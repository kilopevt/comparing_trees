#pragma once
// include/ConsoleUI.h
#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include <memory>
#include <vector>

#include "Benchmark.h"
#include "DataGenerator.h"
#include "TreeBase.h"

class ConsoleUI {
private:
    std::unique_ptr<TreeBase> currentTree;
    TreeType currentTreeType;
    DataGenerator dataGen;
    Benchmark benchmark;
    std::vector<int> currentDataSet;

    // Основные меню
    void mainMenu();
    void selectTreeType();
    void treeOperationsMenu();
    void testingMenu();
    void visualizationMenu();
    void exportMenu();

    // Операции с деревьями
    void insertElement();
    void removeElement();
    void searchElement();
    void traverseTree();
    void clearTree();
    void printTreeInfo();
    void printTreeStructure();
    void generateAndInsertData();

    // Тестирование
    void runSingleTest();
    void runComparativeTest();
    void generateDataset();

    // Вспомогательные методы
    void printHeader(const std::string& title);
    void waitForEnter(); 
    int getIntegerInput(const std::string& prompt);

public:
    ConsoleUI();
    void run();
};

#endif // CONSOLEUI_H