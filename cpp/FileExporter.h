// include/FileExporter.h
#ifndef FILEEXPORTER_H
#define FILEEXPORTER_H

#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include "CommonTypes.h"
#include "Statistics.h"

class FileExporter {
public:
    // Экспорт структуры дерева
    static bool exportTreeStructure(TreeBase* tree, const std::string& filename);

    // Экспорт результатов обхода
    static bool exportTraversalResults(TreeBase* tree, TraversalType type,
        const std::string& filename);

    // Экспорт статистики в CSV
    static bool exportStatisticsToCSV(const StatisticsCollector& stats,
        const std::string& filename);

    // Экспорт сравнительных тестов
    static bool exportComparativeTest(
        const std::vector<TreeType>& treeTypes,
        const std::vector<std::string>& treeNames,
        const std::vector<std::vector<long long>>& results,
        const std::string& filename);

    // Экспорт в формат дл€ построени€ графиков (gnuplot, matplotlib)
    static bool exportForPlotting(
        const std::vector<std::pair<std::string, std::vector<long long>>>& data,
        const std::string& filename,
        const std::string& xlabel = "Количество элементов",
        const std::string& ylabel = "Время (нс)");

    // Генераци€ отчета в формате Markdown
    static std::string generateMarkdownReport(
        const std::vector<DetailedStatistics>& stats,
        const std::string& title = "ќтчет по исследованию деревьев");

private:
    static std::string getCurrentDateTime();
    static std::string escapeCSV(const std::string& str);
};

#endif // FILEEXPORTER_H