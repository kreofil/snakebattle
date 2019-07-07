#include "tcpclient.h"
#include "ui_tcpclient.h"

#include <QtWidgets>
#include <QtNetwork>

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
  , m_socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    ui->address->setText(QHostAddress(QHostAddress::LocalHost).toString());
    ui->port->setValue(52693);

    ui->text->setFocus();

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnectByServer()));
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::on_text_returnPressed()
{
    const QString text = ui->text->text().simplified();
    if (text.isEmpty()
            || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QString message = m_user + ": " + ui->text->text() + QChar(23);
    m_socket->write(message.toLocal8Bit());
    ui->text->clear();
}

void TcpClient::readMessage()
{
    if (m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    m_receivedData.append(m_socket->readAll());
    if (!m_receivedData.contains(QChar(23)))
        return;

    QStringList messages = m_receivedData.split(QChar(23));
    m_receivedData = messages.takeLast();
    foreach (const QString &message, messages) {
        ui->chat->insertPlainText(message + "\n");
    }
}

void TcpClient::on_connect_clicked()
{
    const QString user = ui->user->text().simplified();
    if (user.isEmpty()) {
        ui->chat->insertPlainText("== Unable to connect to server. You must define an user name.\n");
        return;
    }

    m_user = user;

    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        ui->chat->insertPlainText("== Connecting...\n");
        m_socket->connectToHost(ui->address->text(), ui->port->value());
        updateGui(QAbstractSocket::ConnectingState);
    }
}

void TcpClient::connectedToServer()
{
    ui->chat->insertPlainText("== Connected to server.\n");
    updateGui(QAbstractSocket::ConnectedState);
}

void TcpClient::on_disconnect_clicked()
{
    if (m_socket->state() != QAbstractSocket::ConnectingState) {
        ui->chat->insertPlainText("== Abort connecting.\n");
    }
    m_socket->abort();
    updateGui(QAbstractSocket::UnconnectedState);
}

void TcpClient::disconnectByServer()
{
    ui->chat->insertPlainText("== Disconnected by server.\n");
    updateGui(QAbstractSocket::UnconnectedState);
}

void TcpClient::updateGui(QAbstractSocket::SocketState state)
{
    const bool connected = (state == QAbstractSocket::ConnectedState);
    const bool unconnected = (state == QAbstractSocket::UnconnectedState);
    ui->connect->setEnabled(unconnected);
    ui->address->setEnabled(unconnected);
    ui->port->setEnabled(unconnected);
    ui->user->setEnabled(unconnected);

    ui->disconnect->setEnabled(!unconnected);
    ui->chat->setEnabled(connected);
    ui->text->setEnabled(connected);
}
