#include "game.h"
#include <QRandomGenerator>

// Инициализация параметров питона
void Snake::init(char color, int oldDirection, int iHead, int jHead, int iTail, int jTail)
{
    // Размер и позиции в кольцевом списке фиксированы
    m_head = 1;
    m_tail = 0;
    m_size = 2;
    // Позиция головы
    m_body[1].i = iHead;
    m_body[1].j = jHead;
    // Позиция хвоста
    m_body[0].i = iTail;
    m_body[0].j = jTail;
    // Цвет питона
    m_color = color;
    // Начальное направление движения
    m_oldDirection = oldDirection;
    // Начальное число очков
    m_point = 0;
}

// Перемещение питона в новую ячейку поля по заданному направлению
void Snake::step(int direction) {
    int i = 0;
    int j = 0;   // новые координаты головы
    // При попытке движения в противоположном направлении оставляется старое направление
    if((direction == stepleft  && m_oldDirection == stepright) ||
       (direction == stepright && m_oldDirection == stepleft)  ||
       (direction == stepup    && m_oldDirection == stepdown)  ||
       (direction == stepdown  && m_oldDirection == stepup)
    ) {
            direction = m_oldDirection;
    }
    // Изменение координат перемещения
    switch(direction) {
        case stepleft:
            i = m_body[m_head].i;
            j = m_body[m_head].j - 1;
            break;
        case stepright:
            i = m_body[m_head].i;
            j = m_body[m_head].j + 1;
            break;
        case stepup:
            i = m_body[m_head].i - 1;
            j = m_body[m_head].j;
            break;
        case stepdown:
            i = m_body[m_head].i + 1;
            j = m_body[m_head].j;
            break;
    }
    // Запоминаем текущее направление в качестве предыдущего
    m_oldDirection = direction;
    // Изменение данных змейки после получения новых координат головы...
    newHeadPosition(i, j);
}

// Формирование новых координат головы питона после перемещения в указанную точку
void Snake::newHeadPosition(int i, int j)
{
    int maxLength = maxLines * maxColumns;
    m_head = (m_head + 1) % maxLength;
    m_body[m_head].i = i;
    m_body[m_head].j = j;
}

// Формирование новых координат хвоста питона после перемещения в указанную точку
void Snake::newTailPosition()
{
    int maxLength = maxLines * maxColumns;
    m_tail = (m_tail + 1) % maxLength;
}

//=================================================

Game::Game():
    m_lines(0), m_columns(0),
    m_eatNum(0)
{}

Game::~Game() {}

// Установка размера поля игры
void Game::setFieldSize(int lines, int columns)
{
    m_lines = lines;
    m_columns = columns;
}

// Начальная инициализация игровой ситуации
void Game::init(int lines, int columns)
{
    m_lines = lines;
    m_columns = columns;
    // Обнуление поля
    for(int i = 0; i < m_lines; i++) {
        for(int j = 0; j < m_columns; j++) {
            m_field[i][j] = ' ';
        }
    }
    // Формирование начального положения змей и их размеров
    // сверху поля
    snake1.init(snakeColor1, stepright, 0, 1, 0, 0);
    // Занимаются клетки на поле
    m_field[0][0] = m_field[0][1] = snakeColor1;
    // снизу поля
    snake2.init(snakeColor1, stepleft, m_lines-1, m_columns-2, m_lines-1, m_columns-1);
    // Занимаются клетки на поле
    m_field[m_lines-1][m_columns-2] = m_field[m_lines-1][m_columns-1] = snakeColor2;

    // Начальная генерация еды на поле
    m_eatNum = 0;
    for(int i = 0; i < maxEat; i++) {
        AddNewEat();
    }
}

// Проверка того, что анализируемая ячейка находится в границах поля
bool Game::isCeilInField(int i, int j)
{
    if (i >= 0 && i < m_lines && j >= 0 && j < m_columns) return true;
    else return false;
}

// Проверка, что клетка поля свободна
bool Game::isCeilEmpty(int i, int j)
{
    return m_field[i][j] == ' ' ? true : false;
}

// Проверка, что клетка поля с eдой
bool Game::isCeilEat(int i, int j)
{
    return m_field[i][j] == '*' ? true : false;
}

// Проверка клетки на все возможные столкновения
bool Game::isAnyCollision(int i, int j)
{
    if((!isCeilInField(i, j))
            || (m_field[i][j] == snakeColor1)
            || (m_field[i][j] == snakeColor2)
       ) {
        return true;
    }
    return false;
}

// Питон, делающий корректный шаг
void Game::correctStep(Snake &snake, int i, int j)
{
    // Голова пошла.
    if(isCeilEmpty(i, j)) {
        // Пустая ячейка. Хвост сдвинулся
        m_field[snake.m_body[snake.m_tail].i][snake.m_body[snake.m_tail].j] = emptyColor;
        snake.newTailPosition();
    } else if(isCeilEat(i, j)) {
        // Клетка с едой. Размер растет. Хвост стоит на месте
        snake.m_size++;
        // Счет тоже растет
        snake.m_point++;
        // Убираем пищу из массива
        // (добавление новой после всех шагов при формировании пакета)
        // В начале поиск пищи с координатами головы
        for (int t = 0; t < m_eatNum; t++) {
            if((m_eat[t].i == i)&&(m_eat[t].j == j)) {
                // Пища найдена. Заносим на ее место последний элемент
                m_eat[t].i = m_eat[m_eatNum-1].i;
                m_eat[t].j = m_eat[m_eatNum-1].j;
                // и уменьшаем количество еды на 1
                m_eatNum--;
                // Дальше перебирать смысла нет
                break;
            }
        } // Обязательно должна найтись. Иначе где-то ошибка
    }
}

// Генерация случайного целого положительного числа в диапазоне start <= i < start+N
int Game::IntRndValue(int start, int end)
{
    int number = end - start;
    int randomValue = int(QRandomGenerator::global()->generate()) % number;
    //int randomValue = qrand() % number;
    return randomValue + start;
}

// Добавление новой ячеки с пищей в массив ячеек и на поле
void Game::AddNewEat()
{
    int i, j;
    // Цикл до тех пор, пока не определятся координаты пустой ячейки
    do {
        i = IntRndValue(0, m_lines);
        j = IntRndValue(0, m_columns);
    } while (!isCeilEmpty(i, j));
    // Добавление пищи на поле в первую свободную ячейку
    m_field[i][j] = eatColor;
    // Добавление пищи в массив
    m_eat[m_eatNum].i = i;
    m_eat[m_eatNum].j = j;
    m_eatNum++;
}

// Формирование начального пакета для клиентов
void Game::makeInitPackage (QString &package)
{
    package = "init ";
    // Формирование размеров поля
    QString lines   = QString::number(m_lines);
    QString columns = QString::number(m_columns);
    package += lines + " " + columns;
    // Формирование пяти ячеек с едой
    for(int i = 0; i < m_eatNum; i++) {
        package += " " + QString::number(m_eat[i].i)
                + " " + QString::number(m_eat[i].j);
    }
    // Перенос в пакет начального положения питонов
    // Первый
    package += " " + QString::number(snake1.m_body[snake1.m_head].i);
    package += " " + QString::number(snake1.m_body[snake1.m_head].j);
    package += " " + QString::number(snake1.m_body[snake1.m_tail].i);
    package += " " + QString::number(snake1.m_body[snake1.m_tail].j);
    // Второй
    package += " " + QString::number(snake2.m_body[snake2.m_head].i);
    package += " " + QString::number(snake2.m_body[snake2.m_head].j);
    package += " " + QString::number(snake2.m_body[snake2.m_tail].i);
    package += " " + QString::number(snake2.m_body[snake2.m_tail].j);
}

// Формирование пакета для следующего шага
void Game::makeNextPackage(QString &package)
{
    package = "next";
    // Изменение положения питонов
    // Первый
    package += " " + QString::number(snake1.m_body[snake1.m_head].i);
    package += " " + QString::number(snake1.m_body[snake1.m_head].j);
    package += " " + QString::number(snake1.m_body[snake1.m_tail].i);
    package += " " + QString::number(snake1.m_body[snake1.m_tail].j);
    // Второй
    package += " " + QString::number(snake2.m_body[snake2.m_head].i);
    package += " " + QString::number(snake2.m_body[snake2.m_head].j);
    package += " " + QString::number(snake2.m_body[snake2.m_tail].i);
    package += " " + QString::number(snake2.m_body[snake2.m_tail].j);

    // Добавление еды и ее передача, если имеется нехватка
    if(m_eatNum < maxEat) {
        AddNewEat();
        package += " " + QString::number(m_eat[m_eatNum-1].i)
                + " " + QString::number(m_eat[m_eatNum-1].j);
    } else {
        package += " -1 -1";
    }
    // Передача числа набранных игроками очков
    package += " " + QString::number(snake1.m_point);
    package += " " + QString::number(snake2.m_point);
}

// Формирование пакета завершения игры
void Game::makeEndPackage(QString &package)
{
    package = "end";
    // Изменение положения питонов
    // Первый
    package += " " + QString::number(snake1.m_body[snake1.m_head].i);
    package += " " + QString::number(snake1.m_body[snake1.m_head].j);
    package += " " + QString::number(snake1.m_body[snake1.m_tail].i);
    package += " " + QString::number(snake1.m_body[snake1.m_tail].j);
    // Второй
    package += " " + QString::number(snake2.m_body[snake2.m_head].i);
    package += " " + QString::number(snake2.m_body[snake2.m_head].j);
    package += " " + QString::number(snake2.m_body[snake2.m_tail].i);
    package += " " + QString::number(snake2.m_body[snake2.m_tail].j);
    // Победитель или ничья
    package += " " + QString::number(m_winner);
    // Передача числа набранных игроками очков (окончательный счет)
    package += " " + QString::number(snake1.m_point);
    package += " " + QString::number(snake2.m_point);
}

// Выполнение шага игры и формирование пакета
// Возвращает true, если шаг корректный и false при завершении игры
bool Game::makeStep(int m_direction1, int m_direction2)
{
    bool fall1 = false, fall2 = false; // индикаторы падения
    // Шаг питона 1
    snake1.step(m_direction1);
    snake2.step(m_direction2);

    // Выделение координат голов на новом месте
    int h1i = snake1.m_body[snake1.m_head].i;
    int h1j = snake1.m_body[snake1.m_head].j;
    int h2i = snake2.m_body[snake2.m_head].i;
    int h2j = snake2.m_body[snake2.m_head].j;

    // Анализ ситуациq после проделанного шага
    if((h1i==h2i)&&(h1j==h2j)) {
        // питоны столкнулись головами
        fall1 = fall2 = true;
        snake1.newTailPosition();
        snake2.newTailPosition();
        goto twoFall;
    }
    // Столкновение первого питона с чем-то нехорошим
    if((fall1 = isAnyCollision(h1i, h1j)) == true) {
        snake1.newTailPosition();
    }
    // Столкновение второго питона с чем-то нехорошим
    if((fall2 = isAnyCollision(h2i, h2j)) == true) {
        snake2.newTailPosition();
    }
// Проверка комбинаций состояний питонов
twoFall:
    if(fall1 && fall2) {
        // Оба питона сдохли
        // Показать слияние голов в экстазе или оба лоханулись
        ///snake1.newHeadPosition(h1i, h1j);
        ///snake2.newHeadPosition(h2i, h2j);
        // Проверка длины питонов
        if(snake1.m_size==snake2.m_size) {
            // Равны -> ничья
            m_winner = 0;
        } else if(snake1.m_size > snake2.m_size) {
            // Первая длиннее - ее победа
            m_winner = 1;
        } else {
            // Вторая длиннее - ее победа
            m_winner = 2;
        }
        return false;
    } else if(!fall1 && fall2) {
        // выигрыш первого
        m_winner = 1;
        correctStep(snake1, h1i, h1j);
        ///snake2.newHeadPosition(h2i, h2j);
        return false;
    } else if(fall1 && !fall2) {
        // выигрыш второго
        m_winner = 2;
        ///snake1.newHeadPosition(h1i, h1j);
        correctStep(snake2, h2i, h2j);
        return false;
    }
    // Иначе продолжение банкета
    // Первый питон.
    correctStep(snake1, h1i, h1j);
    m_field[h1i][h1j] = snakeColor1;
    // Второй питон.
    correctStep(snake2, h2i, h2j);
    m_field[h2i][h2j] = snakeColor2;
    return true;
}
