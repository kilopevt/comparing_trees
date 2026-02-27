// src/ConsoleUI.cpp
#include <ctime>
#include <cstdlib> 
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <limits> 
#include <sstream>

#include "AVLTree.h"
#include "AATree.h"
#include "ConsoleUI.h"
#include "FileExporter.h"
#include "Treap.h"
#include "TwoThreeTree.h"
#include "Visualizer.h"

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

ConsoleUI::ConsoleUI() : currentTree(nullptr), currentTreeType(TreeType::UNKNOWN) {
    // Инициализация
}

void ConsoleUI::run() {
    while (true) {
        mainMenu();
    }
}

void ConsoleUI::mainMenu() {
    printHeader("ГЛАВНОЕ МЕНЮ - Исследование бинарных деревьев");
    std::cout << "1. Выбрать тип дерева\n";
    std::cout << "2. Операции с деревом\n";
    std::cout << "3. Тестирование производительности\n";
    std::cout << "4. Экспорт результатов\n";
    std::cout << "5. Выход\n\n";
    std::cout << "Выберите опцию: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1:
        selectTreeType();
        break;
    case 2:
        if (currentTree) {
            treeOperationsMenu();
        }
        else {
            std::cout << "\nСначала выберите тип дерева!\n";
            waitForEnter();
        }
        break;
    case 3:
        testingMenu();
        break;
    case 4:
        exportMenu();
        break;
    case 5:
        std::exit(0);
    default:
        std::cout << "\nНеверный выбор!\n";
        waitForEnter();
    }
}

void ConsoleUI::selectTreeType() {
    printHeader("ВЫБОР ТИПА ДЕРЕВА");
    std::cout << "1. AVL дерево (самобалансирующееся)\n";
    std::cout << "2. AA дерево (Арне Андерсона)\n";
    std::cout << "3. Декартово дерево (Treap)\n";
    std::cout << "4. 2-3 дерево (B-дерево порядка 3)\n";
    std::cout << "5. Назад\n\n";
    std::cout << "Выберите тип дерева: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1:
        currentTree = std::make_unique<AVLTree>();
        currentTreeType = TreeType::AVL;
        std::cout << "\n✓ AVL дерево выбрано\n";
        break;
    case 2:
        currentTree = std::make_unique<AATree>();
        currentTreeType = TreeType::AA;
        std::cout << "\n✓ AA дерево выбрано\n";
        break;
    case 3:
        currentTree = std::make_unique<Treap>();
        currentTreeType = TreeType::TREAP;
        std::cout << "\n✓ Декартово дерево выбрано\n";
        break;
    case 4:
        currentTree = std::make_unique<TwoThreeTree>();
        currentTreeType = TreeType::TWO_THREE;
        std::cout << "\n✓ 2-3 дерево выбрано\n";
        break;
    case 5:
        return;
    default:
        std::cout << "\nНеверный выбор!\n";
    }
    waitForEnter();
}

void ConsoleUI::treeOperationsMenu() {
    while (true) {
        printHeader("ОПЕРАЦИИ С ДЕРЕВОМ");
        std::cout << "1. Вставить элемент\n";
        std::cout << "2. Удалить элемент\n";
        std::cout << "3. Найти элемент\n";
        std::cout << "4. Обход дерева\n";
        std::cout << "5. Очистить дерево\n";
        std::cout << "6. Показать информацию о дереве\n";
        std::cout << "7. Показать структуру дерева\n";
        std::cout << "8. Заполнить дерево случайными данными\n";
        std::cout << "9. Назад\n\n";
        std::cout << "Выберите опцию: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            insertElement();
            break;
        case 2:
            removeElement();
            break;
        case 3:
            searchElement();
            break;
        case 4:
            traverseTree();
            break;
        case 5:
            clearTree();
            break;
        case 6:
            printTreeInfo();
            break;
        case 7:
            printTreeStructure();
            break;
        case 8:
            generateAndInsertData();
            break;
        case 9:
            return;
        default:
            std::cout << "\nНеверный выбор!\n";
            waitForEnter();
        }
    }
}

void ConsoleUI::insertElement() {
    printHeader("ВСТАВКА ЭЛЕМЕНТА");
    int key = getIntegerInput("Введите ключ для вставки: ");

    OperationResult result = currentTree->insert(key);

    std::cout << "\n " << result.message << "\n";
    std::cout << "  Время выполнения: " << result.time_ns << " нс\n";
    std::cout << " Высота дерева после операции: " << result.height << "\n";
    std::cout << " Количество узлов: " << result.nodes_count << "\n";

    waitForEnter();
}

void ConsoleUI::removeElement() {
    printHeader("УДАЛЕНИЕ ЭЛЕМЕНТА");
    int key = getIntegerInput("Введите ключ для удаления: ");

    OperationResult result = currentTree->remove(key);

    std::cout << "\n " << result.message << "\n";
    std::cout << "  Время выполнения: " << result.time_ns << " нс\n";
    std::cout << " Высота дерева после операции: " << result.height << "\n";
    std::cout << " Количество узлов: " << result.nodes_count << "\n";

    waitForEnter();
}

void ConsoleUI::searchElement() {
    printHeader("ПОИСК ЭЛЕМЕНТА");
    int key = getIntegerInput("Введите ключ для поиска: ");

    OperationResult result = currentTree->search(key);

    std::cout << "\n " << result.message << "\n";
    std::cout << "  Время выполнения: " << result.time_ns << " нс\n";

    waitForEnter();
}

void ConsoleUI::traverseTree() {
    printHeader("ОБХОД ДЕРЕВА");
    std::cout << "Выберите тип обхода:\n";
    std::cout << "1. Прямой обход (Pre-order)\n";
    std::cout << "2. Центрированный обход (In-order)\n";
    std::cout << "3. Обратный обход (Post-order)\n";
    std::cout << "4. Обход в ширину (Level-order)\n";
    std::cout << "5. Назад\n\n";
    std::cout << "Выберите опцию: ";

    int choice;
    std::cin >> choice;

    if (choice == 5) return;

    TraversalType type;
    std::string type_name;

    switch (choice) {
    case 1:
        type = TraversalType::PREORDER;
        type_name = "Прямой обход";
        break;
    case 2:
        type = TraversalType::INORDER;
        type_name = "Центрированный обход";
        break;
    case 3:
        type = TraversalType::POSTORDER;
        type_name = "Обратный обход";
        break;
    case 4:
        type = TraversalType::LEVELORDER;
        type_name = "Обход в ширину";
        break;
    default:
        std::cout << "\nНеверный выбор!\n";
        waitForEnter();
        return;
    }

    std::cout << "\n" << type_name << ":\n";
    std::cout << "=====================================\n";

    std::vector<int> traversal = currentTree->traverse(type);

    if (traversal.empty()) {
        std::cout << "Дерево пустое\n";
    }
    else {
        for (size_t i = 0; i < traversal.size(); i++) {
            std::cout << traversal[i];
            if (i < traversal.size() - 1) {
                std::cout << ", ";
                if ((i + 1) % 10 == 0) std::cout << "\n";
            }
        }
        std::cout << "\n\nВсего элементов: " << traversal.size() << "\n";
    }

    waitForEnter();
}

void ConsoleUI::clearTree() {
    printHeader("ОЧИСТКА ДЕРЕВА");
    std::cout << "Вы уверены, что хотите очистить дерево? (y/n): ";

    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        OperationResult result = currentTree->clear();
        std::cout << "\n✎ " << result.message << "\n";
    }
    else {
        std::cout << "\nОперация отменена\n";
    }

    waitForEnter();
}

void ConsoleUI::printTreeInfo() {
    printHeader("ИНФОРМАЦИЯ О ДЕРЕВЕ");

    std::string tree_type;
    switch (currentTreeType) {
    case TreeType::AVL:
        tree_type = "AVL дерево";
        break;
    case TreeType::AA:
        tree_type = "AA дерево";
        break;
    case TreeType::TREAP:
        tree_type = "Декартово дерево";
        break;
    case TreeType::TWO_THREE:
        tree_type = "2-3 дерево";
        break;
    default:
        tree_type = "Неизвестный тип";
    }

    std::cout << " Тип дерева: " << tree_type << "\n";
    std::cout << " Высота дерева: " << currentTree->getHeight() << "\n";
    std::cout << " Количество узлов: " << currentTree->getNodeCount() << "\n";
    std::cout << " Дерево " << (currentTree->isEmpty() ? "пустое" : "не пустое") << "\n";

    TreeStatistics stats = currentTree->getStatistics();
    std::cout << "\n СТАТИСТИКА:\n";
    std::cout << "  Вставок: " << stats.insert_count << "\n";
    std::cout << "  Удалений: " << stats.remove_count << "\n";
    std::cout << "  Поисков: " << stats.search_count << "\n";
    std::cout << "  Обходов: " << stats.traversal_count << "\n";

    if (stats.rotations_count > 0) {
        std::cout << "  Поворотов: " << stats.rotations_count << "\n";
    }
    if (stats.skew_count > 0) {
        std::cout << "  Skew операций: " << stats.skew_count << "\n";
    }
    if (stats.split_count > 0) {
        std::cout << "  Split операций: " << stats.split_count << "\n";
    }
    if (stats.splits_count > 0) {
        std::cout << "  Расщеплений: " << stats.splits_count << "\n";
    }
    if (stats.merges_count > 0) {
        std::cout << "  Слияний: " << stats.merges_count << "\n";
    }

    waitForEnter();
}

void ConsoleUI::printTreeStructure() {
    printHeader("СТРУКТУРА ДЕРЕВА");

    std::string structure = Visualizer::visualizeTree(currentTree.get());
    std::cout << structure << "\n";

    waitForEnter();
}

void ConsoleUI::generateAndInsertData() {
    printHeader("ГЕНЕРАЦИЯ И ВСТАВКА ДАННЫХ");

    if (!currentTree) {
        std::cout << "Сначала выберите тип дерева!\n";
        waitForEnter();
        return;
    }

    // Запрос количества элементов
    int count = getIntegerInput("Введите количество элементов для генерации: ");
    if (count <= 0) {
        std::cout << "Количество должно быть положительным числом!\n";
        waitForEnter();
        return;
    }

    // Выбор типа данных
    std::cout << "\nВыберите тип данных:\n";
    std::cout << "1. Случайные значения\n";
    std::cout << "2. По возрастанию\n";
    std::cout << "3. По убыванию\n";
    std::cout << "4. Почти отсортированные (90% отсортировано)\n";
    std::cout << "5. Сортированные с дубликатами\n";
    std::cout << "6. Почти отсортированные в обратном порядке\n";
    std::cout << "7. Отмена\n\n";
    std::cout << "Выберите опцию: ";

    int choice;
    std::cin >> choice;

    if (choice == 7) return;

    DataOrder order;
    std::string order_name;

    switch (choice) {
    case 1: order = DataOrder::RANDOM; order_name = "случайные"; break;
    case 2: order = DataOrder::ASCENDING; order_name = "по возрастанию"; break;
    case 3: order = DataOrder::DESCENDING; order_name = "по убыванию"; break;
    case 4: order = DataOrder::ALMOST_SORTED; order_name = "почти отсортированные"; break;
    case 5: order = DataOrder::SORTED_WITH_DUPLICATES; order_name = "сортированные с дубликатами"; break;
    case 6: order = DataOrder::REVERSE_ALMOST_SORTED; order_name = "почти отсортированные в обратном порядке"; break;
    default:
        std::cout << "\nНеверный выбор!\n";
        waitForEnter();
        return;
    }

    // Генерация данных
    std::cout << "\n Генерация " << count << " элементов (" << order_name << ")...\n";
    std::vector<int> data = dataGen.generate(count, order);

    // Вставка данных в дерево
    std::cout << " Вставка элементов в дерево...\n";
    auto start = std::chrono::high_resolution_clock::now();

    int successful_inserts = 0;
    int duplicates = 0;

    for (int key : data) {
        OperationResult result = currentTree->insert(key);
        if (result.success) {
            successful_inserts++;
        }
        else {
            duplicates++;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Вывод результатов
    std::cout << "\n Генерация и вставка завершены!\n";
    std::cout << "════════════════════════════════════════\n";
    std::cout << " РЕЗУЛЬТАТЫ:\n";
    std::cout << "   Сгенерировано элементов: " << count << "\n";
    std::cout << "   Успешно вставлено: " << successful_inserts << "\n";
    std::cout << "   Дубликатов (пропущено): " << duplicates << "\n";
    std::cout << "   Общее время: " << total_time << " нс\n";
    std::cout << "   Среднее время на вставку: " << (count > 0 ? total_time / count : 0) << " нс\n";
    std::cout << "   Текущая высота дерева: " << currentTree->getHeight() << "\n";
    std::cout << "   Всего узлов в дереве: " << currentTree->getNodeCount() << "\n";

    waitForEnter();
}

void ConsoleUI::visualizationMenu() {
    // Заглушка - этот метод не использован
    printHeader("ВИЗУАЛИЗАЦИЯ");
    std::cout << "Функция визуализации пока не реализована.\n";
    waitForEnter();
}

void ConsoleUI::testingMenu() {
    while (true) {
        printHeader("ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ");
        std::cout << "1. Тестирование одного дерева\n";
        std::cout << "2. Сравнительное тестирование всех деревьев\n";
        std::cout << "3. Назад\n\n";
        std::cout << "Выберите опцию: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            runSingleTest();
            break;
        case 2:
            runComparativeTest();
            break;
        case 3:
            return;
        default:
            std::cout << "\nНеверный выбор!\n";
            waitForEnter();
        }
    }
}

void ConsoleUI::runSingleTest() {
    printHeader("ТЕСТИРОВАНИЕ ОДНОГО ДЕРЕВА");

    if (!currentTree) {
        std::cout << "Сначала выберите тип дерева!\n";
        waitForEnter();
        return;
    }

    std::cout << "Введите количество элементов для теста: ";
    int count = getIntegerInput("");

    std::cout << "\nВыберите порядок данных:\n";
    std::cout << "1. Случайные\n";
    std::cout << "2. По возрастанию\n";
    std::cout << "3. По убыванию\n";
    std::cout << "4. Почти отсортированные\n";
    std::cout << "Выберите опцию: ";

    int order_choice;
    std::cin >> order_choice;

    DataOrder order = DataOrder::RANDOM;
    switch (order_choice) {
    case 1: order = DataOrder::RANDOM; break;
    case 2: order = DataOrder::ASCENDING; break;
    case 3: order = DataOrder::DESCENDING; break;
    case 4: order = DataOrder::ALMOST_SORTED; break;
    default:
        std::cout << "\nНеверный выбор, используется случайный порядок\n";
    }

    DataGenerator generator;
    std::vector<int> data = generator.generate(count, order);

    std::cout << "\n Запуск теста...\n";

    // Очищаем текущее дерево
    currentTree->clear();

    // Тестируем вставку
    auto insert_start = std::chrono::high_resolution_clock::now();
    for (int key : data) {
        currentTree->insert(key);
    }
    auto insert_end = std::chrono::high_resolution_clock::now();
    long long insert_time = std::chrono::duration_cast<std::chrono::nanoseconds>(insert_end - insert_start).count();

    // Тестируем поиск
    auto search_start = std::chrono::high_resolution_clock::now();
    for (int key : data) {
        currentTree->search(key);
    }
    auto search_end = std::chrono::high_resolution_clock::now();
    long long search_time = std::chrono::duration_cast<std::chrono::nanoseconds>(search_end - search_start).count();

    // Тестируем удаление
    auto delete_start = std::chrono::high_resolution_clock::now();
    for (int key : data) {
        currentTree->remove(key);
    }
    auto delete_end = std::chrono::high_resolution_clock::now();
    long long delete_time = std::chrono::duration_cast<std::chrono::nanoseconds>(delete_end - delete_start).count();

    // Выводим результаты
    std::cout << "\n=== РЕЗУЛЬТАТЫ ===\n\n";
    std::cout << "Вставка " << count << " элементов: " << insert_time << " нс ("
        << (count > 0 ? insert_time / count : 0) << " нс/элемент)\n";
    std::cout << "Поиск " << count << " элементов: " << search_time << " нс ("
        << (count > 0 ? search_time / count : 0) << " нс/элемент)\n";
    std::cout << "Удаление " << count << " элементов: " << delete_time << " нс ("
        << (count > 0 ? delete_time / count : 0) << " нс/элемент)\n";
    std::cout << "Финальная высота: " << currentTree->getHeight() << "\n";
    std::cout << "Финальное количество узлов: " << currentTree->getNodeCount() << "\n";

    TreeStatistics stats = currentTree->getStatistics();
    if (stats.rotations_count > 0) {
        std::cout << "Количество поворотов: " << stats.rotations_count << "\n";
    }
    if (stats.skew_count > 0) {
        std::cout << "Количество skew операций: " << stats.skew_count << "\n";
    }
    if (stats.split_count > 0) {
        std::cout << "Количество split операций: " << stats.split_count << "\n";
    }

    std::cout << "\nСохранить результаты в файл? (y/n): ";
    char save_choice;
    std::cin >> save_choice;

    if (save_choice == 'y' || save_choice == 'Y') {
        std::string filename = "test_results_" + std::to_string(std::time(nullptr)) + ".csv";
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Операция;Количество;Время_нс;Время_на_элемент_нс\n";
            file << "Вставка;" << count << ";" << insert_time << ";" << (count > 0 ? insert_time / count : 0) << "\n";
            file << "Поиск;" << count << ";" << search_time << ";" << (count > 0 ? search_time / count : 0) << "\n";
            file << "Удаление;" << count << ";" << delete_time << ";" << (count > 0 ? delete_time / count : 0) << "\n";
            file.close();
            std::cout << "✓ Результаты сохранены в " << filename << "\n";
        }
        else {
            std::cout << "✗ Ошибка при сохранении файла\n";
        }
    }

    waitForEnter();
}

void ConsoleUI::runComparativeTest() {
    printHeader("СРАВНИТЕЛЬНОЕ ТЕСТИРОВАНИЕ");

    std::cout << "Введите количество элементов для теста: ";
    int count = getIntegerInput("");

    std::cout << "\nВыберите порядок данных:\n";
    std::cout << "1. Случайные\n";
    std::cout << "2. По возрастанию\n";
    std::cout << "3. По убыванию\n";
    std::cout << "4. Почти отсортированные\n";
    std::cout << "Выберите опцию: ";

    int order_choice;
    std::cin >> order_choice;

    DataOrder order = DataOrder::RANDOM;
    switch (order_choice) {
    case 1: order = DataOrder::RANDOM; break;
    case 2: order = DataOrder::ASCENDING; break;
    case 3: order = DataOrder::DESCENDING; break;
    case 4: order = DataOrder::ALMOST_SORTED; break;
    default:
        std::cout << "\nНеверный выбор, используется случайный порядок\n";
    }

    std::cout << "\nВведите количество итераций: ";
    int iterations = getIntegerInput("");

    std::cout << "\n Запуск сравнительного теста...\n";
    std::cout << "Это может занять некоторое время...\n";

    std::vector<BenchmarkResult> all_results = benchmark.runFullTest(count, order, iterations);

    benchmark.printResults(all_results);

    // Сохраняем результаты
    std::string filename = "comparative_test_" + std::to_string(std::time(nullptr)) + ".csv";
    benchmark.saveResultsToCSV(all_results, filename);
    std::cout << "\n✓ Результаты сохранены в " << filename << "\n";

    waitForEnter();
}

void ConsoleUI::exportMenu() {
    printHeader("ЭКСПОРТ РЕЗУЛЬТАТОВ");

    if (!currentTree) {
        std::cout << "Сначала выберите тип дерева!\n";
        waitForEnter();
        return;
    }

    std::cout << "1. Экспортировать структуру дерева\n";
    std::cout << "2. Экспортировать результаты обхода\n";
    std::cout << "3. Экспортировать статистику в CSV\n";
    std::cout << "4. Назад\n\n";
    std::cout << "Выберите опцию: ";

    int choice;
    std::cin >> choice;

    std::string timestamp = std::to_string(std::time(nullptr));

    switch (choice) {
    case 1: {
        std::string filename = "tree_structure_" + timestamp + ".txt";
        if (FileExporter::exportTreeStructure(currentTree.get(), filename)) {
            std::cout << "\n✓ Структура дерева экспортирована в " << filename << "\n";
        }
        else {
            std::cout << "\n✗ Ошибка при экспорте\n";
        }
        break;
    }
    case 2: {
        std::cout << "\nВыберите тип обхода для экспорта:\n";
        std::cout << "1. Прямой обход\n";
        std::cout << "2. Центрированный обход\n";
        std::cout << "3. Обратный обход\n";
        std::cout << "4. Обход в ширину\n";
        std::cout << "Выберите опцию: ";

        int traversal_choice;
        std::cin >> traversal_choice;

        TraversalType type;
        switch (traversal_choice) {
        case 1: type = TraversalType::PREORDER; break;
        case 2: type = TraversalType::INORDER; break;
        case 3: type = TraversalType::POSTORDER; break;
        case 4: type = TraversalType::LEVELORDER; break;
        default:
            std::cout << "\nНеверный выбор!\n";
            waitForEnter();
            return;
        }

        std::string filename = "traversal_" + timestamp + ".txt";
        if (FileExporter::exportTraversalResults(currentTree.get(), type, filename)) {
            std::cout << "\n Результаты обхода экспортированы в " << filename << "\n";
        }
        else {
            std::cout << "\n Ошибка при экспорте\n";
        }
        break;
    }
    case 3: {
        std::string filename = "statistics_" + timestamp + ".csv";
        std::ofstream file(filename);
        if (file.is_open()) {
            TreeStatistics stats = currentTree->getStatistics();
            file << "Операция;Количество;Среднее_время_нс\n";
            file << "Вставка;" << stats.insert_count << ";" << stats.getAverageInsertTime() << "\n";
            file << "Поиск;" << stats.search_count << ";" << stats.getAverageSearchTime() << "\n";
            file << "Удаление;" << stats.remove_count << ";" << stats.getAverageDeleteTime() << "\n";
            file << "Обходы;" << stats.traversal_count << ";0\n";
            file.close();
            std::cout << "\n✓ Статистика экспортирована в " << filename << "\n";
        }
        else {
            std::cout << "\n Ошибка при экспорте\n";
        }
        break;
    }
    case 4:
        return;
    default:
        std::cout << "\nНеверный выбор!\n";
    }

    waitForEnter();
}

void ConsoleUI::generateDataset() {
    // Заглушка - не используется
}

void ConsoleUI::printHeader(const std::string& title) {
    system(CLEAR_SCREEN);

    std::cout << "==================================================================================\n";
    std::cout << "                     ИССЛЕДОВАНИЕ БИНАРНЫХ ДЕРЕВЬЕВ\n";
    std::cout << "==================================================================================\n";
    std::cout << " " << title << "\n";
    std::cout << "==================================================================================\n\n";
}

void ConsoleUI::waitForEnter() {
    std::cout << "\n==================================================================================\n";
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    std::cin.get();
}

int ConsoleUI::getIntegerInput(const std::string& prompt) {
    int value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << "Неверный ввод. Пожалуйста, введите целое число: ";
    }
    return value;
}