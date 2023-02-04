#include "tcpclient.h"
#include "ui_tcpclient.h"

#include <QtWidgets>
#include <QtNetwork>

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient),
    m_socket(new QTcpSocket(this)),
    m_user("Player"),
    m_lines(0), m_columns(0)
{
    ui->setupUi(this);
    ui->address->setText(QHostAddress(QHostAddress::LocalHost).toString());
    ui->port->setValue(5260);

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
    // Контролируется отправка только возможных команд
    QString step;
    const QString text = ui->text->text().simplified();
    if (text.isEmpty()
            || m_socket->state() != QAbstractSocket::ConnectedState) {
        return;
    } else if((text == "l") || (text == "left")) {
        step = "left";
    } else if((text == "r") || (text == "right")) {
        step = "right";
    } else if((text == "u") || (text == "up")) {
        step = "up";
    } else if((text == "d") || (text == "down")) {
        step = "down";
    } else {
        ui->chat->insertPlainText("==> incorrect command: " + step + "\n");
        ui->text->clear();
        return;
    }

    QString message = "step " + m_user;
    message += " " + step;
    ui->chat->insertPlainText("==> " + message + "\n");

    message.append(QChar(23));
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
    QString allMessage = ""; // Сбор всего поступившего сообщения
    m_receivedData = messages.takeLast();
    foreach (const QString &message, messages) {
        ui->chat->insertPlainText("Received: " + message + "\n");
        allMessage += message;
    }

    // Анализ поступившего сообщения
    tranlateMessage(allMessage);
}

void TcpClient::on_connect_clicked()
{
    /*
    const QString user = ui->user->text().simplified();
    if (user.isEmpty()) {
        ui->chat->insertPlainText("== Unable to connect to server. You must define an user name.\n");
        return;
    }

    m_user = user;
*/
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        ui->chat->insertPlainText("== Connecting...\n");
        m_socket->connectToHost(ui->address->text(), ui->port->value());
        updateGui(QAbstractSocket::ConnectingState);
    }

    // Добавка, обеспечивающая передачу данных вслед за установлением соединения
    ///QString message = m_user + ": " + "Visual" + QChar(23);
    QString message = m_user;
    message.append(QChar(23));
    m_socket->write(message.toLocal8Bit());
    ui->text->clear();
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

// Анализ поступившего сообщения
void TcpClient::tranlateMessage(QString &allMessage)
{
    // Расщепление сообщения по пробелам
    QStringList parts = allMessage.split(QChar(' '));
    // Занесение сообщения в вектор для анализа элементов сообщения по индексам
    QVector<QString> strVec;
    foreach (const QString &p, parts) {
        strVec.append(p);
    }
    ///int vecSize = strVec.size();
    ui->chat->insertPlainText("=> strVec[0] = " + strVec[0] + "\n");
    // Анализ сообщения
    if(strVec[0] == "ident") {
        ui->chat->insertPlainText("=> ident package\n");
        // передача номера игрока
        m_playerNum = strVec[1].toInt();
        m_user = strVec[1];
        ui->user->setText(m_user);
    } else if(strVec[0] == "init") {
        // Сообщение, инициализирующее клиента
        // Очистка лога
        ui->chat->clear();
        ui->chat->insertPlainText("=> init package\n");
        // Очистка поля
        ///ui->field->clear();
        // Далее выделяются параметры поля
        m_lines = strVec[1].toInt();
        m_columns = strVec[2].toInt();
        ///drawInitField(fieldStr);
        // Прорисовка еды
        int iEat = 3;
        for(int t = 0; t < 5; t++) {
            int i = strVec[iEat++].toInt();
            int j = strVec[iEat++].toInt();
            ///insertSymbolInField(fieldStr, i, j, '*');
        }
        // Добавление питонов
        // Первый
        int iHead1 = strVec[13].toInt();
        int jHead1 = strVec[14].toInt();
        ///insertSymbolInField(fieldStr, iHead1, jHead1, '@');
        int iTail1 = strVec[15].toInt();
        int jTail1 = strVec[16].toInt();
        ///insertSymbolInField(fieldStr, iTail1, jTail1, '#');
        // Второй
        int iHead2 = strVec[17].toInt();
        int jHead2 = strVec[18].toInt();
        ///insertSymbolInField(fieldStr, iHead2, jHead2, '@');
        int iTail2 = strVec[19].toInt();
        int jTail2 = strVec[20].toInt();
        ///insertSymbolInField(fieldStr, iTail2, jTail2, 'O');
        // вывод начального поля на экран
        ///ui->field->insertPlainText(fieldStr);
        // Установка первоначальных значений для запоминания предыдущего шага
        iOldHead1 = iHead1;
        jOldHead1 = jHead1;
        iOldTail1 = iTail1;
        jOldTail1 = jTail1;
    } else if(strVec[0] == "next") {
        // сообщение о следующем шаге
        ui->chat->insertPlainText("=> next package\n");
        // Изменения питонов
        // Первый
        int iHead1 = strVec[1].toInt();
        int jHead1 = strVec[2].toInt();
        ///insertSymbolInField(fieldStr, iHead1, jHead1, '@');
        // Старая голова первого питона забивается символом '#'
        ///insertSymbolInField(fieldStr, iOldHead1, jOldHead1, '#');
        // Запоминается вместо старой новая голова
        int iTail1 = strVec[3].toInt();
        int jTail1 = strVec[4].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail1 != iOldTail1) || (jTail1 != jOldTail1)) {
            // Хвост передвинулся. Очистка ячейки.
            ///insertSymbolInField(fieldStr, iOldTail1, jOldTail1, ' ');
            // Запоминается новый хвост
            iOldTail1 = iTail1;
            jOldTail1 = jTail1;
        }
        // Второй
        int iHead2 = strVec[5].toInt();
        int jHead2 = strVec[6].toInt();
        ///insertSymbolInField(fieldStr, iHead2, jHead2, '@');
        // Старая голова первого питона забивается символом 'O'
        ///insertSymbolInField(fieldStr, iOldHead2, jOldHead2, 'O');
        // Запоминается вместо старой новая голова
        iOldHead2 = iHead2;
        jOldHead2 = jHead2;
        int iTail2 = strVec[7].toInt();
        int jTail2 = strVec[8].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail2 != iOldTail2) || (jTail2 != jOldTail2)) {
            // Хвост передвинулся. Очистка ячейки.
            ///insertSymbolInField(fieldStr, iOldTail2, jOldTail2, ' ');
            // Запоминается новый хвост
            iOldTail2 = iTail2;
            jOldTail2 = jTail2;
        }
        //
    } else if(strVec[0] == "end") {
        // сообщение о завершающем шаге
        ui->chat->insertPlainText("=> end package\n");
        // Изменения питонов
        // Первый
        int iHead1 = strVec[1].toInt();
        int jHead1 = strVec[2].toInt();
        ///insertSymbolInField(fieldStr, iHead1, jHead1, '@');
        // Старая голова первого питона забивается символом '#'
        ///insertSymbolInField(fieldStr, iOldHead1, jOldHead1, '#');
        // Запоминается вместо старой новая голова
        int iTail1 = strVec[3].toInt();
        int jTail1 = strVec[4].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail1 != iOldTail1) || (jTail1 != jOldTail1)) {
            // Хвост передвинулся. Очистка ячейки.
            ///insertSymbolInField(fieldStr, iOldTail1, jOldTail1, ' ');
            // Запоминается новый хвост
            iOldTail1 = iTail1;
            jOldTail1 = jTail1;
        }
        // Второй
        int iHead2 = strVec[5].toInt();
        int jHead2 = strVec[6].toInt();
        ///insertSymbolInField(fieldStr, iHead2, jHead2, '@');
        // Старая голова первого питона забивается символом 'O'
        ///insertSymbolInField(fieldStr, iOldHead2, jOldHead2, 'O');
        // Запоминается вместо старой новая голова
        iOldHead2 = iHead2;
        jOldHead2 = jHead2;
        int iTail2 = strVec[7].toInt();
        int jTail2 = strVec[8].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail2 != iOldTail2) || (jTail2 != jOldTail2)) {
            // Хвост передвинулся. Очистка ячейки.
            ///insertSymbolInField(fieldStr, iOldTail2, jOldTail2, ' ');
            // Запоминается новый хвост
            iOldTail2 = iTail2;
            jOldTail2 = jTail2;
        }
        // Определяется и отображается победитель (пока в логе)
        int m_winner = strVec[9].toInt();
        if(m_winner == 0) {
            ui->chat->insertPlainText("Drawn game!!!\n");
        } else if (m_winner == 1) {
            ui->chat->insertPlainText("The 1st Player is Winner!!!\n");
        } else {
            ui->chat->insertPlainText("The 2nd Player is Winner!!!\n");
        }
        // Вводятся и отображаются окончательные очки
        //int m_point1 = strVec[10].toInt();
        //int m_point2 = strVec[11].toInt();
        ///ui->point1->setText(strVec[10]);
        ///ui->point2->setText(strVec[11]);
    } else {
        ui->chat->insertPlainText("=> Unnown message\n" + allMessage);
    }

}
