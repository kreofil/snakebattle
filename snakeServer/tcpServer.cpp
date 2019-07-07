#include <QtNetwork>
#include <QPlainTextEdit>

#include "tcpServer.h"
#include "ui_tcpServer.h"

#include <QTimer>
void Qsleep(int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

TcpServer::TcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpServer),
    m_server(new QTcpServer(this)),
    m_player1(nullptr), m_player2(nullptr), m_playerCount(0)
{
    ui->setupUi(this);

    if (!m_server->listen(QHostAddress::LocalHost, 5260)) {
    ///if (!m_server->listen(QHostAddress::Any, 5260)) {
        ui->log->setPlainText("Failure while starting server: "+ m_server->errorString());
        return;
    }

    // Установка связи между сигналами и слотами
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(ui->startGameButton, SIGNAL(clicked()), this, SLOT(StartGame()));

    ui->address->setText(m_server->serverAddress().toString());
    ui->port->setText(QString::number(m_server->serverPort()));

    // Установка начальных значений игрового поля
    m_lines = 10;
    m_columns = 20;
    ui->lines->setText(QString::number(m_lines));
    ui->columns->setText(QString::number(m_columns));

    m_flag1 = m_flag2 = false;
    //ui->startGameButton->setEnabled(false);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::newConnection()
{
    QTcpSocket *con = nullptr;
    while (m_server->hasPendingConnections()) {
        //QTcpSocket *con = m_server->nextPendingConnection();
        con = m_server->nextPendingConnection();

        // Регистрация всех клиентов в общем списке.
        // Способствует их массовому разъединению
        m_clients << con;

        ui->disconnectClients->setEnabled(true);
        connect(con, SIGNAL(disconnected()), this, SLOT(removeConnection()));
        connect(con, SIGNAL(readyRead()), this, SLOT(newMessage()));

        // Запись в лог сведений о создании соединения
        ui->log->insertPlainText(QString("* New connection: %1, port %2\n")
                                 .arg(con->peerAddress().toString())
                                 .arg(QString::number(con->peerPort())));

    }

    // Идентификация соединяемого клиента путем запроса его типа
    // и получения в ответ информации о типе клиента
    ///auto clientType = identify(con);
    // Запись в лог возвращенного признака
    ///ui->log->insertPlainText(QString("* Client number is %1\n")
    ///                         .arg(QString::number(clientType)));

    // Получение информации от клиента об его типе и занесение клиента
    // в список визуализаторов или в качестве одного из игроков.
    ///registration(con);
}

void TcpServer::removeConnection()
{
    if (QTcpSocket *con = qobject_cast<QTcpSocket*>(sender())) {
        ui->log->insertPlainText(QString("* Connection removed: %1, port %2\n")
                                 .arg(con->peerAddress().toString())
                                 .arg(QString::number(con->peerPort())));
        m_clients.removeOne(con);
        con->deleteLater();
        ui->disconnectClients->setEnabled(!m_clients.isEmpty());
    }
}

void TcpServer::newMessage()
{
    // Идентификация передатчика сообщений по сокету.
    if (QTcpSocket *con = qobject_cast<QTcpSocket*>(sender())) {
        // Сохранение сообщения, если последнее содержит признак конца блока,
        // в заданном сокете (Хеше)
        m_receivedData[con].append(con->readAll());
        if (!m_receivedData[con].contains(QChar(23)))
            return;

        // Расщепление принятого блочного сообщения на несколько отдельных
        // с удалением признака блока.
        QStringList messages = m_receivedData[con].split(QChar(23));
        QString fullMessage = "";   // полученное сообщение в виде одной строки
        m_receivedData[con] = messages.takeLast();
        foreach (QString message, messages) {
            // Формирование единого сообщения из отдельных блоков конкатенацией.
            fullMessage += message;
            // Вывод в лог полученных сообщений с разделением блоков признаком конца строки
            ///ui->log->insertPlainText("Sending message: " + message + "\n");
        }
        // Распознавание принятого сообщения и выполнение соответствующих действий
        translateMessage(fullMessage, con);
    }
}

void TcpServer::on_disconnectClients_clicked()
{
    foreach (QTcpSocket *socket, m_clients) {
        socket->close();
    }
    ui->disconnectClients->setEnabled(false);
}

// Идентификация соединяемого клиента путем запроса его типа
// и получения в ответ информации о типе клиента
int TcpServer::identify(QTcpSocket *con)
{
    // В начале клиенту передается пакет с запросом на получение типа
    QString message("?");
    message.append(QChar(23));
    con->write(message.toLocal8Bit());

    return 0;
}

// Передача пакета с начальными данными подключившемуся клиенту
void TcpServer::initPackageSend(QTcpSocket *con)
{
    // Формат начального пакета, общего для всех клиентов:
    // <init> - тип пакета
    // <чиcло_строк_в_поле>
    // <чиcло_стобцов_в_поле>
    // Координаты еды (пять точек на поле)
    // Начальные координаты змей.
    // Первая змея располагается в точках (0,1) - голова, (0,0) - хвост
    // Вторая змея располагается в точках (N-1,N-2), (N-1,N-1),
    //      где N - число строк поля, M - число столбцов
    // В принципе координаты змей в дальнейшем могут генерироваться случайно.
    // Все данные разделяются пробелами и передаются в символьном виде.
/**
    QString package = "init ";
    // Формирование размеров поля
    QString lines   = QString::number(m_lines);
    QString columns = QString::number(m_columns);
    package += lines + " " + columns + " ";
    // Формирование пяти ячеек с едой
    for(int i = 0; i < 5; i++) {

    }
*/
    // Инициализация игрового поля
    m_game.init(m_lines, m_columns);

    QString package;
    m_game.makeInitPackage(package);
    package.append(QChar(23));
    if (con->state() == QAbstractSocket::ConnectedState) {
        con->write(package.toLocal8Bit());
    }
}

// Слот, обеспечивающий запуск игры по нажатию соответствующей кнопки
void TcpServer::StartGame()
{
    // Сброс лога для новых данных
    ui->log->clear();
    // Сообщение в лог о нажатии кнопки Start Game
    ui->log->insertPlainText("Server: Start Game botton clicked\n");

    // Инициализация игрового поля
    m_game.init(m_lines, m_columns);

    // Пересылка инициализационного пакета всем подключенным клиентам
    QString package;
    m_game.makeInitPackage(package);
    package.append(QChar(23));
    foreach (QTcpSocket *socket, m_clients) {
        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->write(package.toLocal8Bit());
    }
}

// Анализ принятого сообщения для инициации различных действий
// в зависимости от принятого пакета
void TcpServer::translateMessage(QString &message, QTcpSocket *con)
{
    // Расщепление сообщения по пробелам
    QStringList parts = message.split(QChar(' '));
    // Занесение сообщения в вектор для анализа элементов сообщения по индексам
    QVector<QString> strVec;
    foreach (const QString &p, parts) {
        strVec.append(p);
    }
    ///int vecSize = strVec.size();
    //ui->log->insertPlainText("=> strVec[0] = " + strVec[0] + "\n");

    if(strVec[0] == "Visualizer") {
        // На связи визуализатор. Нужно зафиксировать указатель в списке визуализаторов
        m_visio << con;
        // Сообщение в лог о подключении визуализатора
        ui->log->insertPlainText("Recived: " + message + " is connected\n");
    } else if (strVec[0] == "Player") {
        // Сообщение в лог о подключении визуализатора
        ui->log->insertPlainText("Recived: " + message + " is connected\n");
        // На связи   игрок.
        // Регистрируем в зависимости от порядка поступления
        if(m_playerCount == 0) {
            // Это первый игрок
            m_player1 = con;
            m_playerCount++;
            // Отправка номера игрока
            QString message("ident 1");
            message.append(QChar(23));
            con->write(message.toLocal8Bit());
        } else if (m_playerCount == 1) {
            // Это второй игрок
            m_player1 = con;
            m_playerCount++;
            // Отправка номера игрока
            QString message("ident 2");
            message.append(QChar(23));
            con->write(message.toLocal8Bit());
        } else {
            // Больше игроков не нужно
            con->close();
        }
    } else if (strVec[0] == "step") {
        // Сообщение в лог о приходе очередного хода
        ui->log->insertPlainText("Next step: " + message + "\n");
        // Анализ пришедшего хода
        int direction = 0;
        if(strVec[2] == "left") {
            direction = stepleft;
        } else if (strVec[2] == "right") {
            direction = stepright;
        } else if (strVec[2] == "up") {
            direction = stepup;
        } else if (strVec[2] == "down") {
            direction = stepdown;
        }
        // Начинается разбор пакета, пришедшего от одного из игроков
        if(strVec[1] == "1") {
            // Ход игрока 1
            m_flag1 = true;
            m_direction1 = direction;
        } else if(strVec[1] == "2") {
            // ход игрока 2
            m_flag2 = true;
            m_direction2 = direction;
        }
        if(m_flag1 && m_flag2) {
            m_flag1 = m_flag2 = false;
            QString package;
            if(m_game.makeStep(m_direction1, m_direction2)) {
                // Продолжение банкета. Следующий пакет
                m_game.makeNextPackage(package);
            } else {
                // Конец игры. Завершающий пакет
                m_game.makeEndPackage(package);
            }
            // Сообщение в лог об отправке пакета
            ui->log->insertPlainText("Sended: " + package + "\n");
            // Отправка пакета всем клиентам
            package.append(QChar(23));
            ///con->write(package.toLocal8Bit());
            foreach (QTcpSocket *socket, m_clients) {
                if (socket->state() == QAbstractSocket::ConnectedState)
                    socket->write(package.toLocal8Bit());
            }
        }
    } else {
        // Принятый пакет не идентифицирован
        // Сообщение в лог о нераспознанном пакете
        ui->log->insertPlainText("Server: The message \"" + message + "\" is not identyfied\n");
    }
}

