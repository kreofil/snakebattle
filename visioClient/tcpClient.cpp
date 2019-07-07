#include <QtWidgets>
#include <QtNetwork>
#include <QVector>

#include "tcpClient.h"
#include "ui_tcpClient.h"

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient),
    m_socket(new QTcpSocket(this)),
    m_user("Visualizer"),
    m_lines(0), m_columns(0)
{
    ui->setupUi(this);
    ui->address->setText(QHostAddress(QHostAddress::LocalHost).toString());
    ui->port->setValue(5260);

    // Установка типа клиента в строке пользователя
    ui->user->setText(m_user);
    ui->user->setReadOnly(true);
    ui->user->setDisabled(true);

    ///ui->point1->setFocus();

    //

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnectByServer()));
}

TcpClient::~TcpClient()
{
    delete ui;
}

/**
void TcpClient::on_text_returnPressed()
{
    const QString text = ui->point1->text().simplified();
    if (text.isEmpty()
            || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QString message = m_user + ": " + ui->point1->text() + QChar(23);
    m_socket->write(message.toLocal8Bit());
    ui->point1->clear();
}
*/

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
    const QString user = ui->user->text().simplified();
    if (user.isEmpty()) {
        ui->chat->insertPlainText("== Unable to connect to server. You must define an user name.\n");
        return;
    }

    m_user = user;

    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        ui->chat->insertPlainText("== Connecting...\n");
        m_socket->connectToHost(ui->address->text(), quint16(ui->port->value()));
        updateGui(QAbstractSocket::ConnectingState);
    }

    // Добавка, обеспечивающая передачу данных вслед за установлением соединения
    ///QString message = m_user + ": " + "Visual" + QChar(23);
    QString message = m_user;
    message.append(QChar(23));
    m_socket->write(message.toLocal8Bit());
    ///ui->point1->clear();

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
    ///ui->point1->setEnabled(connected);
}

// Формирование строки с полем по данным инициализации
void TcpClient::drawInitField(QString &commonStr)
{
    // Для вывода верхней и нижней границ поля формируется строка
    QString firsLastLine = "+";
    for(int i = 0; i < m_columns; i++) {
        firsLastLine.append('-');
    }
    firsLastLine += "+\n";
    // Формируется строка для вывода боковых границ
    QString borders = "|";
    for(int i = 0; i < m_columns; i++) {
        borders.append(' ');
    }
    borders += "|\n";
    // Окончательная строка формируется путем конкатенации
    commonStr = firsLastLine;
    for(int i = 0; i < m_lines; i++) {
        commonStr.append(borders);
    }
    commonStr.append(firsLastLine);
}

// Вставка символа в строку, определяющую поле, как в двумерный массив
void TcpClient::insertSymbolInField(QString &commonStr, int line, int column, QChar ch)
{
    // Пересчет координат с учетом увеличения матрицы поля за счет границ
    int index = (m_columns + 3) * (line + 1) + (column + 1);
    commonStr[index] = ch;
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
    if(strVec[0] == "init") {
        // Сообщение, инициализирующее клиента
        // Очистка лога
        ui->chat->clear();
        ui->chat->insertPlainText("=> init package\n");
        // Очистка поля
        ui->field->clear();
        // Далее выделяются параметры поля
        m_lines = strVec[1].toInt();
        m_columns = strVec[2].toInt();
        drawInitField(fieldStr);
        // Прорисовка еды
        int iEat = 3;
        for(int t = 0; t < 5; t++) {
            int i = strVec[iEat++].toInt();
            int j = strVec[iEat++].toInt();
            insertSymbolInField(fieldStr, i, j, '*');
        }
        // Добавление питонов

        // Первый
        int iHead1 = strVec[13].toInt();
        int jHead1 = strVec[14].toInt();
        insertSymbolInField(fieldStr, iHead1, jHead1, '@');
        int iTail1 = strVec[15].toInt();
        int jTail1 = strVec[16].toInt();
        insertSymbolInField(fieldStr, iTail1, jTail1, '#');
        // Установка первоначальных значений для запоминания предыдущего шага
        iOldHead1 = iHead1;
        jOldHead1 = jHead1;
        iOldTail1 = iTail1;
        jOldTail1 = jTail1;

        // Второй
        int iHead2 = strVec[17].toInt();
        int jHead2 = strVec[18].toInt();
        insertSymbolInField(fieldStr, iHead2, jHead2, '@');
        int iTail2 = strVec[19].toInt();
        int jTail2 = strVec[20].toInt();
        insertSymbolInField(fieldStr, iTail2, jTail2, 'O');
        // Установка первоначальных значений для запоминания предыдущего шага
        iOldHead2 = iHead2;
        jOldHead2 = jHead2;
        iOldTail2 = iTail2;
        jOldTail2 = jTail2;

        // вывод начального поля на экран
        ui->field->setPlainText(fieldStr);
    } else if(strVec[0] == "next") {
        // сообщение о следующем шаге
        ui->chat->insertPlainText("=> next package:"+ allMessage + "\n");
        // Изменения питонов

        // Первый
        // Обработка головы
        int iHead1 = strVec[1].toInt();
        int jHead1 = strVec[2].toInt();
        // Старая голова первого питона забивается символом '#'
        insertSymbolInField(fieldStr, iOldHead1, jOldHead1, '#');
        // Рисуется новое местоположение головы
        insertSymbolInField(fieldStr, iHead1, jHead1, '@');
        // Запоминается вместо старой новая голова
        iOldHead1 = iHead1;
        jOldHead1 = jHead1;
        // Обработка хвоста
        int iTail1 = strVec[3].toInt();
        int jTail1 = strVec[4].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail1 != iOldTail1) || (jTail1 != jOldTail1)) {
            // Хвост передвинулся. Очистка ячейки.
            insertSymbolInField(fieldStr, iOldTail1, jOldTail1, ' ');
            // Запоминается новый хвост
            iOldTail1 = iTail1;
            jOldTail1 = jTail1;
        } // Иначе (при удлинении) хвост остается на месте

        // Второй
        // Обработка головы
        int iHead2 = strVec[5].toInt();
        int jHead2 = strVec[6].toInt();
        // Старая голова первого питона забивается символом 'O'
        insertSymbolInField(fieldStr, iOldHead2, jOldHead2, 'O');
        // Рисуется новое местоположение головы
        insertSymbolInField(fieldStr, iHead2, jHead2, '@');
        // Запоминается вместо старой новая голова
        iOldHead2 = iHead2;
        jOldHead2 = jHead2;
        // Обработка хвоста
        int iTail2 = strVec[7].toInt();
        int jTail2 = strVec[8].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail2 != iOldTail2) || (jTail2 != jOldTail2)) {
            // Хвост передвинулся. Очистка ячейки.
            insertSymbolInField(fieldStr, iOldTail2, jOldTail2, ' ');
            // Запоминается новый хвост
            iOldTail2 = iTail2;
            jOldTail2 = jTail2;
        } // Иначе (при удлинении) хвост остается на месте
        // Обработка координат еды (или ее отсутствия)
        int iEat =  strVec[9].toInt();
        int jEat =  strVec[10].toInt();
        if((iEat >= 0) && (jEat >= 0)) {
            // Новая еда
            insertSymbolInField(fieldStr, iEat, jEat, '*');
        }
        // Отображение информации о текущем счете игры
        ui->point1->setText(strVec[11]);
        ui->point2->setText(strVec[12]);

        // Отображение измененного поля
        ui->field->setPlainText(fieldStr);
    } else if(strVec[0] == "end") {
        // сообщение о завершающем шаге
        ui->chat->insertPlainText("=> end package:"+ allMessage + "\n");
        // Изменения питонов
        // Первый
        int iHead1 = strVec[1].toInt();
        int jHead1 = strVec[2].toInt();
        insertSymbolInField(fieldStr, iHead1, jHead1, '@');
        // Старая голова первого питона забивается символом '#'
        insertSymbolInField(fieldStr, iOldHead1, jOldHead1, '#');
        // Запоминается вместо старой новая голова
        int iTail1 = strVec[3].toInt();
        int jTail1 = strVec[4].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail1 != iOldTail1) || (jTail1 != jOldTail1)) {
            // Хвост передвинулся. Очистка ячейки.
            insertSymbolInField(fieldStr, iOldTail1, jOldTail1, ' ');
            // Запоминается новый хвост
            iOldTail1 = iTail1;
            jOldTail1 = jTail1;
        }
        // Второй
        int iHead2 = strVec[5].toInt();
        int jHead2 = strVec[6].toInt();
        insertSymbolInField(fieldStr, iHead2, jHead2, '@');
        // Старая голова первого питона забивается символом 'O'
        insertSymbolInField(fieldStr, iOldHead2, jOldHead2, 'O');
        // Запоминается вместо старой новая голова
        iOldHead2 = iHead2;
        jOldHead2 = jHead2;
        int iTail2 = strVec[7].toInt();
        int jTail2 = strVec[8].toInt();
        // Проверяется, удлинился ли питон или хвост сдвинулся на новое место
        if((iTail2 != iOldTail2) || (jTail2 != jOldTail2)) {
            // Хвост передвинулся. Очистка ячейки.
            insertSymbolInField(fieldStr, iOldTail2, jOldTail2, ' ');
            // Запоминается новый хвост
            iOldTail2 = iTail2;
            jOldTail2 = jTail2;
        }
        // Отображение заключительного поля
        ui->field->setPlainText(fieldStr);
        // Определяется и отображается победитель (пока в логе)
        int m_winner = strVec[9].toInt();
        if(m_winner == 0) {
            ui->chat->insertPlainText("Drawn game!!!\n");
        } else if (m_winner == 1) {
            ui->chat->insertPlainText("The 1st Player is Winner!!!\n");
        } else {
            ui->chat->insertPlainText("The 2nd Player is Winner!!!\n");
        }
        // Отображаются окончательные очки
        //int m_point1 = strVec[10].toInt();
        //int m_point2 = strVec[11].toInt();
        ui->point1->setText(strVec[10]);
        ui->point2->setText(strVec[11]);
    } else {
        ui->chat->insertPlainText("=> Unnown message\n" + allMessage);
    }

}

