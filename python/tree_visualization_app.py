import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import seaborn as sns
import numpy as np
import os


class TreeVisualizationApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Анализатор производительности деревьев")
        self.root.geometry("1200x800")

        self.data = None
        self.filtered_data = None

        # Настройка стиля
        sns.set_style("whitegrid")
        plt.rcParams['figure.figsize'] = (10, 6)
        plt.rcParams['font.size'] = 10

        self.create_widgets()

    def create_widgets(self):
        # Верхняя панель с кнопками
        control_frame = tk.Frame(self.root)
        control_frame.pack(fill=tk.X, padx=10, pady=5)

        tk.Button(control_frame, text="Загрузить CSV", command=self.load_csv,
                  bg="#4CAF50", fg="white", font=("Arial", 10, "bold")).pack(side=tk.LEFT, padx=5)

        tk.Button(control_frame, text="Статистика", command=self.show_stats,
                  bg="#2196F3", fg="white", font=("Arial", 10)).pack(side=tk.LEFT, padx=5)

        tk.Button(control_frame, text="Экспорт графиков", command=self.export_graphs,
                  bg="#FF9800", fg="white", font=("Arial", 10)).pack(side=tk.LEFT, padx=5)

        # Фрейм с фильтрами
        filter_frame = tk.LabelFrame(self.root, text="Фильтры", font=("Arial", 10, "bold"))
        filter_frame.pack(fill=tk.X, padx=10, pady=5)

        # Выбор типа дерева
        tk.Label(filter_frame, text="Тип дерева:").grid(row=0, column=0, padx=5, pady=5, sticky=tk.W)
        self.tree_type_var = tk.StringVar(value="Все")
        tree_types = ["Все", "AVL Tree", "AA Tree", "Treap", "2-3 Tree"]
        ttk.Combobox(filter_frame, textvariable=self.tree_type_var,
                     values=tree_types, state="readonly", width=15).grid(row=0, column=1, padx=5, pady=5)

        # Выбор операции
        tk.Label(filter_frame, text="Операция:").grid(row=0, column=2, padx=5, pady=5, sticky=tk.W)
        self.operation_var = tk.StringVar(value="Все")
        operations = ["Все", "Insertion", "Search", "Deletion"]
        ttk.Combobox(filter_frame, textvariable=self.operation_var,
                     values=operations, state="readonly", width=15).grid(row=0, column=3, padx=5, pady=5)

        # Выбор метрики
        tk.Label(filter_frame, text="Метрика:").grid(row=0, column=4, padx=5, pady=5, sticky=tk.W)
        self.metric_var = tk.StringVar(value="AvgTimeNs")
        metrics = ["AvgTimeNs", "TotalTimeNs", "FinalHeight", "FinalNodes", "Rotations"]
        ttk.Combobox(filter_frame, textvariable=self.metric_var,
                     values=metrics, state="readonly", width=15).grid(row=0, column=5, padx=5, pady=5)

        # Кнопка применения фильтров
        tk.Button(filter_frame, text="Применить фильтры", command=self.apply_filters,
                  bg="#9C27B0", fg="white").grid(row=0, column=6, padx=10, pady=5)

        # Notebook для вкладок с графиками
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Создание вкладок
        self.tab1 = tk.Frame(self.notebook)
        self.tab2 = tk.Frame(self.notebook)
        self.tab3 = tk.Frame(self.notebook)
        self.tab4 = tk.Frame(self.notebook)

        self.notebook.add(self.tab1, text="Время операций")
        self.notebook.add(self.tab2, text="Структура деревьев")
        self.notebook.add(self.tab3, text="Сравнение производительности")
        self.notebook.add(self.tab4, text="Данные")

        # Статус бар
        self.status_bar = tk.Label(self.root, text="Готов к загрузке данных",
                                   bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def load_csv(self):
        file_path = filedialog.askopenfilename(
            title="Выберите CSV файл",
            filetypes=[("CSV files", "*.csv"), ("All files", "*.*")]
        )

        if not file_path:
            return

        try:
            # Загрузка данных с учетом разделителя ;
            self.data = pd.read_csv(file_path, delimiter=';')

            # Очистка данных
            self.clean_data()

            # Обновление интерфейса
            self.update_data_display()
            self.apply_filters()

            self.status_bar.config(text=f"Загружено: {os.path.basename(file_path)} | "
                                        f"Записей: {len(self.data)}")

        except Exception as e:
            messagebox.showerror("Ошибка", f"Не удалось загрузить файл:\n{str(e)}")

    def clean_data(self):
        """Очистка и предобработка данных"""
        if self.data is not None:
            # Замена специальных значений
            self.data = self.data.replace(-858993460, np.nan)

            # Удаление дубликатов
            self.data = self.data.drop_duplicates()

            # Преобразование типов данных
            numeric_cols = ['DataSize', 'TotalTimeNs', 'AvgTimeNs',
                            'FinalHeight', 'FinalNodes', 'Rotations']
            for col in numeric_cols:
                if col in self.data.columns:
                    self.data[col] = pd.to_numeric(self.data[col], errors='coerce')

    def apply_filters(self):
        """Применение выбранных фильтров"""
        if self.data is None:
            return

        self.filtered_data = self.data.copy()

        # Применение фильтров
        tree_type = self.tree_type_var.get()
        operation = self.operation_var.get()

        if tree_type != "Все":
            self.filtered_data = self.filtered_data[self.filtered_data['TreeType'] == tree_type]

        if operation != "Все":
            self.filtered_data = self.filtered_data[self.filtered_data['Operation'] == operation]

        # Обновление всех графиков
        self.update_all_charts()

    def update_all_charts(self):
        """Обновление всех графиков"""
        self.create_time_chart()
        self.create_structure_chart()
        self.create_comparison_chart()
        self.update_data_display()

    def create_time_chart(self):
        """График времени операций"""
        for widget in self.tab1.winfo_children():
            widget.destroy()

        if self.filtered_data is None or len(self.filtered_data) == 0:
            tk.Label(self.tab1, text="Нет данных для отображения").pack(expand=True)
            return

        fig, axes = plt.subplots(2, 2, figsize=(12, 8))
        fig.suptitle('Анализ времени выполнения операций', fontsize=14, fontweight='bold')

        try:
            # График 1: Среднее время по типам деревьев и операциям
            ax1 = axes[0, 0]
            pivot_avg = self.filtered_data.pivot_table(
                values='AvgTimeNs',
                index='TreeType',
                columns='Operation',
                aggfunc='mean'
            )
            pivot_avg.plot(kind='bar', ax=ax1, colormap='viridis')
            ax1.set_title('Среднее время выполнения (нс)')
            ax1.set_ylabel('Время (нс)')
            ax1.tick_params(axis='x', rotation=45)
            ax1.legend(title='Операция')

            # График 2: Общее время
            ax2 = axes[0, 1]
            total_time = self.filtered_data.groupby(['TreeType', 'Operation'])['TotalTimeNs'].sum().unstack()
            total_time.plot(kind='bar', ax=ax2, colormap='plasma')
            ax2.set_title('Общее время выполнения (нс)')
            ax2.set_ylabel('Время (нс)')
            ax2.tick_params(axis='x', rotation=45)

            # График 3: Время по операциям (бокс-плот)
            ax3 = axes[1, 0]
            operations_data = []
            operations_labels = []
            for op in self.filtered_data['Operation'].unique():
                op_data = self.filtered_data[self.filtered_data['Operation'] == op]['AvgTimeNs']
                if len(op_data) > 0:
                    operations_data.append(op_data)
                    operations_labels.append(op)
            ax3.boxplot(operations_data, tick_labels=operations_labels)
            ax3.set_title('Распределение времени по операциям')
            ax3.set_ylabel('Время (нс)')

            # График 4: Сравнение производительности
            ax4 = axes[1, 1]
            tree_metrics = self.filtered_data.groupby('TreeType')['AvgTimeNs'].agg(['mean', 'std', 'count'])
            x = np.arange(len(tree_metrics))
            ax4.bar(x, tree_metrics['mean'], yerr=tree_metrics['std'],
                    capsize=5, color=sns.color_palette("husl", len(tree_metrics)))
            ax4.set_xticks(x)
            ax4.set_xticklabels(tree_metrics.index)
            ax4.set_title('Сравнение среднего времени по типам деревьев')
            ax4.set_ylabel('Среднее время (нс)')
            ax4.tick_params(axis='x', rotation=45)

            # Добавление значений на столбцы
            for i, v in enumerate(tree_metrics['mean']):
                ax4.text(i, v + (tree_metrics['std'].iloc[i] if not pd.isna(tree_metrics['std'].iloc[i]) else 0),
                         f'{v:.0f}', ha='center', va='bottom', fontweight='bold')

            plt.tight_layout()

            # Встраивание в tkinter
            canvas = FigureCanvasTkAgg(fig, self.tab1)
            canvas.draw()
            canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

            # Добавление панели инструментов
            toolbar_frame = tk.Frame(self.tab1)
            toolbar_frame.pack(side=tk.BOTTOM, fill=tk.X)

        except Exception as e:
            tk.Label(self.tab1, text=f"Ошибка при построении графиков: {str(e)}").pack()

    def create_structure_chart(self):
        """Графики структуры деревьев"""
        for widget in self.tab2.winfo_children():
            widget.destroy()

        if self.filtered_data is None or len(self.filtered_data) == 0:
            tk.Label(self.tab2, text="Нет данных для отображения").pack(expand=True)
            return

        fig, axes = plt.subplots(2, 2, figsize=(12, 8))
        fig.suptitle('Анализ структуры деревьев', fontsize=14, fontweight='bold')

        try:
            # График 1: Высота деревьев
            ax1 = axes[0, 0]
            height_data = self.filtered_data[self.filtered_data['Operation'] == 'Insertion']
            if len(height_data) > 0:
                height_pivot = height_data.pivot_table(
                    values='FinalHeight',
                    index='TreeType',
                    aggfunc=['mean', 'std', 'count']
                )
                height_pivot.columns = ['mean', 'std', 'count']
                height_pivot['mean'].plot(kind='bar', ax=ax1, color='skyblue', yerr=height_pivot['std'], capsize=5)
                ax1.set_title('Средняя высота дерева после вставки')
                ax1.set_ylabel('Высота')
                ax1.tick_params(axis='x', rotation=45)

            # График 2: Количество узлов
            ax2 = axes[0, 1]
            nodes_data = self.filtered_data[self.filtered_data['Operation'] == 'Insertion']
            if len(nodes_data) > 0:
                nodes_by_tree = nodes_data.groupby('TreeType')['FinalNodes'].mean()
                colors = plt.cm.Set3(np.linspace(0, 1, len(nodes_by_tree)))
                ax2.pie(nodes_by_tree, labels=nodes_by_tree.index, autopct='%1.1f%%',
                        colors=colors, startangle=90)
                ax2.set_title('Распределение узлов по типам деревьев')

            # График 3: Количество поворотов
            ax3 = axes[1, 0]
            rotations_data = self.filtered_data.dropna(subset=['Rotations'])
            if len(rotations_data) > 0:
                rotations_by_tree = rotations_data.groupby('TreeType')['Rotations'].sum()
                rotations_by_tree.plot(kind='bar', ax=ax3, color=['red', 'blue', 'green', 'orange'])
                ax3.set_title('Общее количество поворотов')
                ax3.set_ylabel('Количество поворотов')
                ax3.tick_params(axis='x', rotation=45)

            # График 4: Соотношение высоты и узлов
            ax4 = axes[1, 1]
            scatter_data = self.filtered_data[self.filtered_data['Operation'] == 'Insertion']
            if len(scatter_data) > 0:
                for tree_type in scatter_data['TreeType'].unique():
                    tree_data = scatter_data[scatter_data['TreeType'] == tree_type]
                    ax4.scatter(tree_data['FinalNodes'], tree_data['FinalHeight'],
                                label=tree_type, alpha=0.6, s=100)
                ax4.set_xlabel('Количество узлов')
                ax4.set_ylabel('Высота дерева')
                ax4.set_title('Зависимость высоты от количества узлов')
                ax4.legend()
                ax4.grid(True, alpha=0.3)

            plt.tight_layout()

            # Встраивание в tkinter
            canvas = FigureCanvasTkAgg(fig, self.tab2)
            canvas.draw()
            canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        except Exception as e:
            tk.Label(self.tab2, text=f"Ошибка при построении графиков: {str(e)}").pack()

    def create_comparison_chart(self):
        """Сравнительные графики производительности"""
        for widget in self.tab3.winfo_children():
            widget.destroy()

        if self.filtered_data is None or len(self.filtered_data) == 0:
            tk.Label(self.tab3, text="Нет данных для отображения").pack(expand=True)
            return

        # Создаем фрейм для управления
        control_frame = tk.Frame(self.tab3)
        control_frame.pack(fill=tk.X, padx=10, pady=5)

        tk.Label(control_frame, text="Метрика для сравнения:").pack(side=tk.LEFT, padx=5)
        self.comparison_metric = tk.StringVar(value="AvgTimeNs")
        metrics_combo = ttk.Combobox(control_frame, textvariable=self.comparison_metric,
                                     values=["AvgTimeNs", "TotalTimeNs", "FinalHeight", "FinalNodes", "Rotations"],
                                     state="readonly", width=15)
        metrics_combo.pack(side=tk.LEFT, padx=5)
        metrics_combo.bind('<<ComboboxSelected>>', lambda e: self.update_comparison_chart())

        # Фрейм для графика
        chart_frame = tk.Frame(self.tab3)
        chart_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        self.chart_canvas = None
        self.update_comparison_chart()

    def update_comparison_chart(self):
        """Обновление сравнительного графика"""
        if self.filtered_data is None:
            return

        metric = self.comparison_metric.get()

        # Очистка предыдущего графика
        if self.chart_canvas:
            self.chart_canvas.get_tk_widget().destroy()

        fig, ax = plt.subplots(figsize=(10, 6))

        try:
            # Подготовка данных
            comparison_data = self.filtered_data.copy()

            # Группировка по типу дерева и операции
            grouped = comparison_data.groupby(['TreeType', 'Operation'])[metric].mean().unstack()

            # Создание группированного bar chart
            x = np.arange(len(grouped.index))
            width = 0.25
            multiplier = 0

            operations = grouped.columns
            colors = plt.cm.tab10(np.linspace(0, 1, len(operations)))

            for i, operation in enumerate(operations):
                offset = width * multiplier
                rects = ax.bar(x + offset, grouped[operation], width,
                               label=operation, color=colors[i])
                ax.bar_label(rects, padding=3, fmt='%.0f')
                multiplier += 1

            ax.set_xlabel('Тип дерева')
            ax.set_ylabel(metric)
            ax.set_title(f'Сравнение {metric} по типам деревьев и операциям')
            ax.set_xticks(x + width * (len(operations) - 1) / 2)
            ax.set_xticklabels(grouped.index)
            ax.legend(loc='upper left', ncols=len(operations))
            ax.tick_params(axis='x', rotation=45)

            plt.tight_layout()

            # Встраивание в tkinter
            self.chart_canvas = FigureCanvasTkAgg(fig, self.tab3)
            self.chart_canvas.draw()
            self.chart_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        except Exception as e:
            tk.Label(self.tab3, text=f"Ошибка при построении графика: {str(e)}").pack()

    def update_data_display(self):
        """Обновление отображения данных в таблице"""
        for widget in self.tab4.winfo_children():
            widget.destroy()

        if self.filtered_data is None:
            tk.Label(self.tab4, text="Нет данных для отображения").pack(expand=True)
            return

        # Создание Treeview для отображения данных
        columns = list(self.filtered_data.columns)
        tree = ttk.Treeview(self.tab4, columns=columns, show='headings', height=20)

        # Определение заголовков
        for col in columns:
            tree.heading(col, text=col)
            tree.column(col, width=100)

        # Добавление данных
        for _, row in self.filtered_data.iterrows():
            tree.insert('', tk.END, values=list(row))

        # Добавление скроллбаров
        vsb = ttk.Scrollbar(self.tab4, orient="vertical", command=tree.yview)
        hsb = ttk.Scrollbar(self.tab4, orient="horizontal", command=tree.xview)
        tree.configure(yscrollcommand=vsb.set, xscrollcommand=hsb.set)

        # Размещение элементов
        tree.grid(row=0, column=0, sticky='nsew')
        vsb.grid(row=0, column=1, sticky='ns')
        hsb.grid(row=1, column=0, sticky='ew')

        # Настройка весов для расширения
        self.tab4.grid_rowconfigure(0, weight=1)
        self.tab4.grid_columnconfigure(0, weight=1)

        # Статистика по данным
        stats_frame = tk.Frame(self.tab4)
        stats_frame.grid(row=2, column=0, columnspan=2, sticky='ew', padx=5, pady=5)

        stats_text = f"Показано записей: {len(self.filtered_data)} | "
        stats_text += f"Уникальных деревьев: {self.filtered_data['TreeType'].nunique()} | "
        stats_text += f"Операций: {self.filtered_data['Operation'].nunique()}"

        tk.Label(stats_frame, text=stats_text, font=("Arial", 9)).pack()

    def show_stats(self):
        """Показать статистику по данным"""
        if self.data is None:
            messagebox.showinfo("Статистика", "Нет данных для анализа")
            return

        stats_window = tk.Toplevel(self.root)
        stats_window.title("Статистика данных")
        stats_window.geometry("600x400")

        # Основные статистики
        text = "ОБЩАЯ СТАТИСТИКА ДАННЫХ\n"
        text += "=" * 50 + "\n\n"
        text += f"Всего записей: {len(self.data)}\n"
        text += f"Типы деревьев: {', '.join(self.data['TreeType'].unique())}\n"
        text += f"Операции: {', '.join(self.data['Operation'].unique())}\n\n"

        # Статистика по времени
        text += "СТАТИСТИКА ПО ВРЕМЕНИ (нс)\n"
        text += "-" * 30 + "\n"
        text += f"Среднее время (AvgTimeNs): {self.data['AvgTimeNs'].mean():.0f}\n"
        text += f"Мин. время: {self.data['AvgTimeNs'].min():.0f}\n"
        text += f"Макс. время: {self.data['AvgTimeNs'].max():.0f}\n"
        text += f"Стандартное отклонение: {self.data['AvgTimeNs'].std():.0f}\n\n"

        # Статистика по типам деревьев
        text += "ПРОИЗВОДИТЕЛЬНОСТЬ ПО ТИПАМ ДЕРЕВЬЕВ\n"
        text += "-" * 30 + "\n"

        for tree_type in self.data['TreeType'].unique():
            tree_data = self.data[self.data['TreeType'] == tree_type]
            avg_time = tree_data['AvgTimeNs'].mean()
            text += f"{tree_type}: {avg_time:.0f} нс (записей: {len(tree_data)})\n"

        # Отображение статистики
        text_widget = tk.Text(stats_window, wrap=tk.WORD, font=("Courier", 10))
        text_widget.insert(tk.END, text)
        text_widget.config(state=tk.DISABLED)

        scrollbar = tk.Scrollbar(stats_window, command=text_widget.yview)
        text_widget.config(yscrollcommand=scrollbar.set)

        text_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

    def export_graphs(self):
        """Экспорт графиков в файлы"""
        if self.data is None:
            messagebox.showwarning("Экспорт", "Нет данных для экспорта")
            return

        export_dir = filedialog.askdirectory(title="Выберите папку для сохранения графиков")
        if not export_dir:
            return

        try:
            # Создание всех графиков для экспорта
            self.export_all_charts(export_dir)
            messagebox.showinfo("Успех", f"Графики успешно экспортированы в:\n{export_dir}")

        except Exception as e:
            messagebox.showerror("Ошибка", f"Не удалось экспортировать графики:\n{str(e)}")

    def export_all_charts(self, directory):
        """Создание и сохранение всех графиков"""
        import matplotlib.pyplot as plt

        # 1. График сравнения среднего времени
        plt.figure(figsize=(10, 6))
        avg_time_data = self.data.groupby(['TreeType', 'Operation'])['AvgTimeNs'].mean().unstack()
        avg_time_data.plot(kind='bar', colormap='viridis')
        plt.title('Сравнение среднего времени выполнения операций')
        plt.ylabel('Среднее время (нс)')
        plt.xlabel('Тип дерева')
        plt.xticks(rotation=45)
        plt.tight_layout()
        plt.savefig(f"{directory}/avg_time_comparison.png", dpi=300, bbox_inches='tight')
        plt.close()

        # 2. График общего времени
        plt.figure(figsize=(10, 6))
        total_time_data = self.data.groupby(['TreeType', 'Operation'])['TotalTimeNs'].sum().unstack()
        total_time_data.plot(kind='bar', colormap='plasma')
        plt.title('Общее время выполнения операций')
        plt.ylabel('Общее время (нс)')
        plt.xlabel('Тип дерева')
        plt.xticks(rotation=45)
        plt.tight_layout()
        plt.savefig(f"{directory}/total_time_comparison.png", dpi=300, bbox_inches='tight')
        plt.close()

        # 3. Heatmap корреляций
        plt.figure(figsize=(8, 6))
        numeric_data = self.data.select_dtypes(include=[np.number])
        correlation = numeric_data.corr()
        sns.heatmap(correlation, annot=True, cmap='coolwarm', center=0,
                    square=True, linewidths=.5, cbar_kws={"shrink": .8})
        plt.title('Матрица корреляций числовых параметров')
        plt.tight_layout()
        plt.savefig(f"{directory}/correlation_heatmap.png", dpi=300, bbox_inches='tight')
        plt.close()

        # 4. Распределение высот деревьев
        plt.figure(figsize=(10, 6))
        height_data = self.data[self.data['Operation'] == 'Insertion']
        if len(height_data) > 0:
            for tree_type in height_data['TreeType'].unique():
                tree_height = height_data[height_data['TreeType'] == tree_type]['FinalHeight']
                plt.hist(tree_height, alpha=0.5, label=tree_type, bins=15)
            plt.title('Распределение высот деревьев после вставки')
            plt.xlabel('Высота')
            plt.ylabel('Частота')
            plt.legend()
            plt.tight_layout()
            plt.savefig(f"{directory}/height_distribution.png", dpi=300, bbox_inches='tight')
            plt.close()

        self.status_bar.config(text=f"Графики экспортированы в: {directory}")


def main():
    root = tk.Tk()
    app = TreeVisualizationApp(root)

    # Загрузка тестового файла при запуске (опционально)
    # if os.path.exists("случайно.csv"):
    #     app.data = pd.read_csv("случайно.csv", delimiter=';')
    #     app.clean_data()
    #     app.filtered_data = app.data
    #     app.update_all_charts()

    root.mainloop()


if __name__ == "__main__":
    main()
