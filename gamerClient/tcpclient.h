#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QAbstractSocket>

class QTcpSocket;

namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    // Анализ поступившего сообщения
    void tranlateMessage(QString &allMessage);

private slots:
    void on_text_returnPressed();
    void readMessage();
    void on_connect_clicked();
    void connectedToServer();
    void on_disconnect_clicked();
    void disconnectByServer();


private:
    Ui::TcpClient *ui;
    QTcpSocket *m_socket;
    QString m_user;
    QString m_receivedData;

    int m_playerNum;    // номер игрока
    int m_lines;    // количество строк в поле
    int m_columns;  // количество столбцов в поле
    int m_winner;   // победитель

    // Счетчики очков
    int m_point1;
    int m_point2;

    // Вспомогательные данные для работы с отображаемым полем
    QString fieldStr;           // Строка для отображения поля
    int iOldHead1, jOldHead1;   // Предыдущее местоположение головы первого питона
    int iOldTail1, jOldTail1;   // Предыдущее местоположение хвоста первого питона
    int iOldHead2, jOldHead2;   // Предыдущее местоположение головы второго питона
    int iOldTail2, jOldTail2;   // Предыдущее местоположение хвоста второго питона

    void updateGui(QAbstractSocket::SocketState state);


};

#endif // TCPCLIENT_H
