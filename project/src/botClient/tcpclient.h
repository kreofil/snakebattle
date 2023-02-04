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
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();

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

    void updateGui(QAbstractSocket::SocketState state);
};

#endif // TCPCLIENT_H
