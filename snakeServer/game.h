#include <QObject>
#include <QThread>

#ifndef GAME_H
#define GAME_H

// Константы, ограничивающие размерность
int const maxEat = 5;       // Максимальное число одновременно доступных ячеек с едой.
int const maxLines = 20;    // Максимальное число строк в игровом поле
int const maxColumns = 40;  // Максимальное число столбцов в игровом поле

// Идентификаторы (цвета) клеток игрового поля
char const emptyColor  = ' ';    // идентификатор (цвет) свободных клеток
char const snakeColor1 = '#';    // идентификатор (цвет) клеток змеи №1
char const snakeColor2 = 'O';    // идентификатор (цвет) клеток змеи №2
char const eatColor    = '*';    // идентификатор (цвет) клеток с пищей

// Константы, определяющие перемещение змей
int const stepleft  = 1;
int const stepright = 2;
int const stepup    = 3;
int const stepdown  = 4;

// Класс для формирования задержки (временно используется для тестирования)
class Sleeper: public QThread
{
public:
    static void msleep(unsigned long ms)
    {
        QThread::msleep(ms);
    }
};

// Координаты ячейки в виде точки
struct Point {int i, j;};

// Структура, описывающая питона (кольцевым списком на основе одномерного массива)
// Максимальный размер массива равен площади поля.
struct Snake {
    int m_head;       // Индекс головы в списке
    int m_tail;       // Индеск хвоста в списке
    int m_size;       // Размер змеи
    char m_color;     // Цвет (номер) змеи
    int m_point;      // Число набранных баллов
    int m_oldDirection; // Предыдущее направление движения
    Point m_body[maxLines*maxColumns];

    // Инициализация параметров питона
    void init(char color, int oldDirection, int iHead, int jHead, int iTail, int jTail);
    // Перемещение питона в новую ячейку поля по заданному направлению
    void step(int direction);
    // Формирование новых координат головы питона после перемещения в указанную точку
    void newHeadPosition(int i, int j);
    // Формирование новых координат хвоста питона после перемещения в указанную точку
    void newTailPosition();
};

class Game: QObject
{
    Q_OBJECT

public:
    explicit Game();
    ~Game();

    // Начальная инициализация игровой ситуации
    void init(int lines, int columns);
    // Установка размера поля игры
    void setFieldSize(int lines, int columns);
    // Проверка того, что анализируемая ячейка находится в границах поля
    bool isCeilInField(int i, int j);
    // Проверка, что клетка поля пуста
    bool isCeilEmpty(int i, int j);
    // Проверка, что клетка поля с eдой
    bool isCeilEat(int i, int j);
    // Проверка клетки на все возможные столкновения
    bool isAnyCollision(int i, int j);
    // Питон, делающий корректный шаг
    void correctStep(Snake &snake, int i, int j);

    // Добавление новой ячеки с пищей в массив ячеек.
    void AddNewEat();

    // Генерация случайного целого положительного числа в диапазоне start <= i < start+N
    int IntRndValue(int start, int end);

    // Формирование начального пакета для клиентов
    void makeInitPackage (QString &package);
    // Формирование пакета для следующего шага
    void makeNextPackage(QString &package);
    // Формирование пакета завершения игры
    void makeEndPackage(QString &package);

    // Выполнение шага игры
    // Возвращает true, если шаг корректный и false при завершении игры
    bool makeStep(int m_direction1, int m_direction2);

    // Поле игры максимального размера
    char m_field[maxLines][maxColumns];
    int m_lines;    // установленное число строк
    int m_columns;  // установленное число столбцов



    // Массив для хранения пищи. Задается двумя координатами (точкой ее размещения)
    Point m_eat[maxEat];
    int m_eatNum = 0;     // Текущее количество ячеек с пищей

    // Структуры для хранения питонов
    Snake snake1;
    Snake snake2;

    // Победитель
    int m_winner;

};

#endif // GAME_H
