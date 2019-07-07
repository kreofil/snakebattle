#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>

#include "game.h"

const int players = 2; // Максимально возможное количество игроков

namespace Ui {
class TcpServer;
}

class QTcpServer;
class QTcpSocket;

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

private slots:
    void newConnection();
    void removeConnection();
    void newMessage();

    void on_disconnectClients_clicked();

    // Слот, обеспечивающий запуск игры по нажатию соответствующей кнопки
    void StartGame();

private:
    // Идентификация соединяемого клиента путем запроса его типа
    // и получения в ответ информации о типе клиента
    int identify(QTcpSocket *con);

    // Анализ принятого сообщения для инициации различных действий
    // в зависимости от принятого пакета
    void translateMessage(QString &message, QTcpSocket *con);

    // Передача пакета с начальными данными подключенным клиентам
    void initPackageSend(QTcpSocket *con);

    Ui::TcpServer *ui;      // Основное окно сервера
    QTcpServer *m_server;   // Сервер
    QList<QTcpSocket*> m_clients;   // Список клиентов
    QHash<QTcpSocket*, QString> m_receivedData; // Принимаемые данные

    QList<QTcpSocket*> m_visio;   // Список визуализаторов

    QTcpSocket* m_player1;      // игрок 1
    QTcpSocket* m_player2;      // игрок 2
    int m_playerCount;          // счетчик для регистрации игроков

    bool m_flag1;       // пришел ход от первого игрока
    bool m_flag2;       // пришел ход от второго игрока

    int m_direction1;       // ход первого игрока
    int m_direction2;       // ход второго игрока

    //Параметры игрового поля
    int m_lines;    // количество строк
    int m_columns;  // количество столбцов

    // Игра
    Game m_game;
};

#endif // TCPSERVER_H
