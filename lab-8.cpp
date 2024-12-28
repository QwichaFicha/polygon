#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <limits>
#include <chrono>
#include <algorithm>
#include <unordered_set>
#include <stack>

// Структура для представления узла графа
struct Node {
    double lon, lat;  // Долгота и широта узла
    std::vector<std::pair<Node*, double>> neighbors;  // Список соседних узлов и весов ребер
};

// Структура для представления графа
struct Graph {
    std::unordered_map<std::string, Node*> nodes;  // Хранит узлы графа, используя строковый ключ

    // Создает строковый ключ на основе долготы и широты
    std::string get_key(double lon, double lat) {
        return std::to_string(lon) + "," + std::to_string(lat);
    }

    // Добавляет новый узел в граф, если он еще не существует
    void add_node(double lon, double lat) {
        std::string key = get_key(lon, lat);
        if (nodes.find(key) == nodes.end()) {
            nodes[key] = new Node{ lon, lat };  // Создаем новый узел
        }
    }

    // Создает ребро между двумя узлами с заданным весом
    void add_edge(double lon1, double lat1, double lon2, double lat2, double weight) {
        add_node(lon1, lat1);  // Убедимся, что оба узла существуют
        add_node(lon2, lat2);
        Node* node1 = nodes[get_key(lon1, lat1)];
        Node* node2 = nodes[get_key(lon2, lat2)];
        node1->neighbors.emplace_back(node2, weight);  // Добавляем соседа с весом
    }

    // Функция для поиска ближайшего узла к заданным координатам
    Node* find_closest_node(double lat, double lon) {
        double min_distance = std::numeric_limits<double>::max();
        Node* node_founded = nullptr;

        for (auto& pair : nodes) {
            Node* node = pair.second;
            double distance = std::sqrt(std::pow(node->lat - lat, 2) + std::pow(node->lon - lon, 2));  // Вычисляем расстояние
            if (distance < min_distance) {  // Если нашли более близкий узел
                node_founded = node;  // Обновляем найденный узел
                min_distance = distance;
            }
        }
        return node_founded;  // Возвращаем ближайший узел
    }
};

// Функция для чтения графа из файла
Graph from_file(const std::string& filename) {
    Graph graph;
    std::ifstream file(filename);
    std::string line;

    // Читаем файл построчно
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string parent, neighbors;
        std::getline(ss, parent, ':');
        double lon1, lat1;
        sscanf_s(parent.c_str(), "%lf,%lf", &lon1, &lat1);  // Извлекаем координаты

        // Читаем соседей узла
        while (std::getline(ss, neighbors, ';')) {
            double lon2, lat2, weight;
            sscanf_s(neighbors.c_str(), "%lf,%lf,%lf", &lon2, &lat2, &weight);  // Извлекаем координаты соседей и вес
            graph.add_edge(lon1, lat1, lon2, lat2, weight);  // Добавляем ребро
        }
    }

    return graph;
}

// Алгоритм поиска в ширину (BFS)
std::vector<Node*> BFS(Graph& graph, Node* start, Node* goal) {
    if (!start || !goal) return {};  // Проверка на существование стартового и целевого узлов

    std::queue<std::vector<Node*>> queue;  // Очередь для хранения путей
    std::unordered_map<Node*, bool> visited;  // Массив для отслеживания посещенных узлов

    queue.push({ start });

    // Пока есть узлы для обработки
    while (!queue.empty()) {
        std::vector<Node*> path = queue.front();  // Получаем текущий путь
        queue.pop();  // Убираем из очереди

        Node* current = path.back();  // Узел в конце пути
        if (current == goal) {  // Если достигли цель, возвращаем путь
            return path;
        }

        // Если узел уже посещен, продолжаем
        if (visited[current]) continue;
        visited[current] = true;  // Отмечаем узел как посещенный

        // Проходим по всем соседям текущего узла
        for (const auto& neighbor : current->neighbors) {
            std::vector<Node*> new_path = path;  // Копируем текущий путь
            new_path.push_back(neighbor.first);  // Добавляем соседний узел
            queue.push(new_path);  // Добавляем новый путь в очередь
        }
    }

    return {};  // Если путь не найден
}

// Алгоритм поиска в глубину (DFS)
std::vector<Node*> DFS(Graph& graph, Node* start, Node* goal) {
    if (!start || !goal) return {};  // Проверка на существование стартового и целевого узлов

    std::unordered_set<Node*> visited;  // Множество для хранения посещенных узлов
    std::stack<Node*> stack;  // Стек для хранения узлов
    std::vector<Node*> path;  // Вектор для хранения результата

    visited.insert(start);  // Добавляем стартовый узел в посещенные
    stack.push(start);  // Запускаем с начального узла

    // Пока стек не пуст
    while (!stack.empty()) {
        Node* current = stack.top();  // Получаем текущий узел
        stack.pop();  // Убираем его из стека
        path.push_back(current);  // Добавляем текущий узел в результат

        // Обходим всех соседей текущего узла
        for (const auto& neighbor : current->neighbors) {
            Node* neighbor_node = neighbor.first;  // Получаем соседний узел
            if (visited.find(neighbor_node) == visited.end()) {  // Если сосед еще не посещен
                visited.insert(neighbor_node);  // Отмечаем его как посещенный
                stack.push(neighbor_node);  // Добавляем соседний узел в стек
            }
        }
    }

    return path;
}

// Алгоритм Дейкстры
std::vector<Node*> Dijkstra(Graph& graph, Node* start, Node* goal) {
    if (!start || !goal) return {};  // Проверка на существование стартового и целевого узлов

    std::unordered_map<Node*, double> distances;  // Хранит расстояния до узлов
    std::unordered_map<Node*, Node*> previous;  // Хранит предыдущие узлы для восстановления пути
    auto compare = [](std::pair<Node*, double> a, std::pair<Node*, double> b) {
        return a.second > b.second;  // Сравнение по расстояниям
        };
    // Приоритетная очередь для выбора узлов с минимальным расстоянием
    std::priority_queue<std::pair<Node*, double>, std::vector<std::pair<Node*, double>>, decltype(compare)> node_queue(compare);

    // Инициализируем расстояния до всех узлов бесконечностью
    for (auto& pair : graph.nodes) {
        distances[pair.second] = std::numeric_limits<double>::infinity();
    }

    distances[start] = 0.0;  // Расстояние до стартового узла равно 0
    node_queue.push({ start, 0.0 });  // Добавляем стартовый узел в очередь

    // Пока есть узлы для обработки
    while (!node_queue.empty()) {
        Node* current = node_queue.top().first;  // Получаем узел с минимальным расстоянием
        node_queue.pop();

        if (current == goal) {  // Если достигли цель, прекращаем
            break;
        }

        // Обходим соседей текущего узла
        for (auto& neighbor : current->neighbors) {
            double new_distance = distances[current] + neighbor.second;  // Рассчитываем новое расстояние
            // Если новое расстояние меньше известного, обновляем
            if (new_distance < distances[neighbor.first]) {
                distances[neighbor.first] = new_distance;  // Обновляем расстояние
                previous[neighbor.first] = current;  // Запоминаем предшествующий узел
                node_queue.push({ neighbor.first, new_distance });  // Помещаем в очередь
            }
        }
    }

    std::vector<Node*> path;
    for (Node* at = goal; at != nullptr; at = previous[at]) {  // Восстанавливаем путь
        path.push_back(at);
    }

    std::reverse(path.begin(), path.end());  // Оборачиваем путь в правильном порядке
    return path;
}

// Эвристическая функция для алгоритма A*
double heuristic(Node* a, Node* b) {
    return std::sqrt(std::pow(a->lat - b->lat, 2) + std::pow(a->lon - b->lon, 2));  // Эвклидово расстояние
}

// Алгоритм A*
std::vector<Node*> AStar(Graph& graph, Node* start, Node* goal) {
    if (!start || !goal) return {};  // Проверка на существование стартового и целевого узлов

    std::unordered_map<Node*, Node*> came_from;  // Для восстановления пути
    std::unordered_map<Node*, double> g_score;  // Расстояния от стартового узла
    std::unordered_map<Node*, double> f_score;  // Оценка расстояний до целевого узла
    auto compare = [](const std::pair<Node*, double>& a, const std::pair<Node*, double>& b) {
        return a.second > b.second;  // Сравнение узлов по оценке
        };
    // Приоритетная очередь для узлов
    std::priority_queue<std::pair<Node*, double>, std::vector<std::pair<Node*, double>>, decltype(compare)> node_pq(compare);

    g_score[start] = 0;  // Расстояние от старта к себе равно 0
    f_score[start] = heuristic(start, goal);  // Оценка расстояния
    node_pq.push({ start, f_score[start] });  // Добавляем стартовый узел в очередь

    // Пока есть узлы для обработки
    while (!node_pq.empty()) {
        Node* current = node_pq.top().first;  // Узел с минимальной оценкой
        node_pq.pop();

        // Если мы достигли цели, восстанавливаем путь
        if (current == goal) {
            std::vector<Node*> path;  // Вектор для хранения найденного пути
            while (came_from.find(current) != came_from.end()) {  // Восстанавливаем путь
                path.push_back(current);
                current = came_from[current];  // Переход к предыдущему узлу
            }
            std::reverse(path.begin(), path.end());  // Правильный порядок
            return path;  // Возвращаем путь
        }

        // Обходим всех соседей текущего узла
        for (const auto& neighbor_pair : current->neighbors) {
            Node* neighbor = neighbor_pair.first;  // Соседний узел
            double weight = neighbor_pair.second;  // Вес ребра
            double curr_g_score = g_score[current] + weight;  // Рассчитываем текущую g-оценку

            // Если сосед еще не оценивался
            if (g_score.find(neighbor) == g_score.end()) {
                g_score[neighbor] = std::numeric_limits<double>::max();  // Устанавливаем максимально возможное значение
            }

            // Если новая g-оценка меньше старой
            if (curr_g_score < g_score[neighbor]) {
                came_from[neighbor] = current;  // Запоминаем путь
                g_score[neighbor] = curr_g_score;  // Обновляем g-оценку
                f_score[neighbor] = g_score[neighbor] + heuristic(neighbor, goal);  // Обновляем f-оценку
                node_pq.push({ neighbor, f_score[neighbor] });  // Запускаем узел в обработку
            }
        }
    }

    return {};  // Если путь не найден
}

// Функция для вывода найденного пути
void print_path(const std::vector<Node*>& path) {
    std::cout << "Path: ";
    for (const auto& node : path) {
        std::cout << "(" << node->lat << ", " << node->lon << ") -> ";
    }
    std::cout << "END" << std::endl;
}


int main() {
    Graph graph = from_file("spb_graph.txt");

    double univercity_lat = 59.957238, univercity_lon = 30.308108;
    double home_lat = 59.848294, home_lon = 30.329455;

    // Поиск ближайших узлов к университету и дому
    std::cout << "The closest node to the university: (" << univercity_lat << ", " << univercity_lon << ")" << std::endl;
    std::cout << "The closest node to the home: (" << home_lat << ", " << home_lon << ")" << std::endl;

    Node* start = graph.find_closest_node(univercity_lat, univercity_lon);
    Node* goal = graph.find_closest_node(home_lat, home_lon);

    // Выполнение и замер времени для алгоритма BFS
    std::cout << "BFS: " << std::endl << "^^^^^^^^^^^^^^^" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto BFS_path = BFS(graph, start, goal);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> bfs_duration = end_time - start_time;  // Вычисляем продолжительность
    print_path(BFS_path);  // Выводим найденный путь

    // Выполнение и замер времени для алгоритма DFS
    std::cout << "DFS: " << std::endl << "^^^^^^^^^^^^^^^" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    auto DFS_path = DFS(graph, start, goal);
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dfs_duration = end_time - start_time;
    print_path(DFS_path);

    // Выполнение и замер времени для алгоритма Дейкстры
    std::cout << "Dijkstra: " << std::endl << "^^^^^^^^^^^^^^^" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    auto Dijkstra_path = Dijkstra(graph, start, goal);
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dijkstra_duration = end_time - start_time;
    print_path(Dijkstra_path);

    // Выполнение и замер времени для алгоритма A*
    std::cout << "AStar: " << std::endl << "^^^^^^^^^^^^^^^" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    auto AStar_path = AStar(graph, start, goal);
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> AStar_duration = end_time - start_time;
    print_path(AStar_path);

    // Вывод времени выполнения каждого алгоритма
    std::cout << "BFS completed in " << bfs_duration.count() << " seconds." << std::endl;
    std::cout << "DFS completed in " << dfs_duration.count() << " seconds." << std::endl;
    std::cout << "Dijkstra completed in " << dijkstra_duration.count() << " seconds." << std::endl;
    std::cout << "AStar completed in " << AStar_duration.count() << " seconds." << std::endl;

    return 0;  // Завершаем программу
};