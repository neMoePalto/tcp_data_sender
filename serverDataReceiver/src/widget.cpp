#include "widget.h"
#include <QHBoxLayout>
#include <QTime>
#include <QHeaderView>
#include "parsersmanager.h"
#include "restarter.h"

Widget::Widget()
{
//    int b = 4;
//    if (b == MySpace::a)
//        qDebug() << "namespace works!";

    quint16 defaultPort = 3600;
    quint32 defaultRestartValue = 0;
    _time = std::make_unique<QTime>();

    // --------------------- GUI --------------------
    _green0  = new QPalette(Qt::green);
    _red0    = new QPalette(Qt::red);
    _green1  = new QPalette(QColor(110,140,60));
    _red1    = new QPalette(QColor(200,110,70));
    _gray0   = new QPalette(Qt::gray);
    _yellow0 = new QPalette(Qt::yellow);

    QFont   arial_13B("Monospace", 13, QFont::DemiBold);
    QFont   arial_12B("Monospace", 12, QFont::Bold);
    QFont   arial_12("Arial", 12, QFont::Normal);
    QFont   arial_14("Arial", 14, QFont::Normal);
    QFont   arial_15("Arial", 15, QFont::Normal);
    QFont   arial_16("Arial", 16, QFont::Normal);
    QFont   arial_16B("Arial", 16, QFont::DemiBold);
    QFont   arial_20B("Arial", 20, QFont::DemiBold);
    this->setFont(arial_12);


    QGridLayout* gridl = new QGridLayout(this);
    QLabel* lbTitle = new QLabel("Server");
    lbTitle->setFont(arial_20B);
    _lbCurrentPort = new QLabel();
    _lbCurrentPort->setFont(arial_16B);
    gridl->addWidget(lbTitle,      0, 0,   1, 1);  // (obj, row, colmn,    rowSpan, colmnSpan)
    gridl->addWidget(_lbCurrentPort,   0, 2,   1, 1);

    QString _step;
    QLabel* lbLeft1 = new QLabel(tr("Номер порта tcp-сервера (авторестарт сервера \nбудет проходить на интервале [порт + 23]):"));
    _step.setNum(defaultPort);
    _lePort = new QLineEdit();
    _lePort->setText(_step);
    _lePort->setFixedWidth(70);
    _pbConnectionStatus = new QPushButton();
    _pbConnectionStatus->setPalette(*_red0);
    _pbConnectionStatus->setFixedWidth(40);
    _pbConnectionStatus->setCheckable(false);

    QHBoxLayout* hLay = new QHBoxLayout();
    hLay->addWidget(_lePort);
    hLay->addWidget(_pbConnectionStatus);
    gridl->addWidget(lbLeft1, 1, 0,   1, 1);
    gridl->addLayout(hLay,    1, 2,   1, 1, Qt::AlignLeft);

    QLabel *lbLeft2 = new QLabel(tr("Интервал авторестарта, от 4 сек \n(0 - авторестар отключен):"));
    _step.setNum(defaultRestartValue);
    _leRestartValue = new QLineEdit();
    _leRestartValue->setText(_step);
    _leRestartValue->setFixedWidth(70);
    gridl->addWidget(lbLeft2,         2, 0,   1, 1);
    gridl->addWidget(_leRestartValue, 2, 2,   1, 1);
    _pbClearOutput = new QPushButton(tr("Очистить вывод"));
    _pbClearOutput->setFixedSize(150, 70);
    gridl->addWidget(_pbClearOutput, 3, 0,   2, 2);

    _pbStart = new QPushButton(tr("Запустить\nсервер"));
    _pbStart->setPalette(*_green1);
    _pbStart->setFixedSize(250, 100);
    gridl->addWidget(_pbStart, 3, 2,   2, 2);

    QLabel *lb_33 = new QLabel(tr("Принятые блоки данных:"));
    _teStatistics = new QTextEdit();
//    _teStatistics->setMinimumSize(500, 150);
    _teStatistics->setFixedWidth(500);

    _teStatistics->setPalette(*_green1);
    _teStatistics->setFont(arial_15);
    QLabel *lbErrors = new QLabel(tr("Отладочная информация:"));
    _teErrors = new QTextEdit();
//    _teErrors->setMinimumSize(400, 150);
    _teErrors->setFixedWidth(500);
    _teErrors->setPalette(*_red1);
    _teErrors->setFont(arial_12B);
    gridl->addWidget(lb_33,     0, 4,   1, 2, Qt::AlignCenter);
    gridl->addWidget(_teStatistics, 1, 4,   14, 2);
    gridl->addWidget(lbErrors,  15, 4,   1, 2, Qt::AlignCenter);
    gridl->addWidget(_teErrors, 16, 4,   2, 2);

    // Описание таблицы:
    _tableStatistics = new QTableWidget(0, 4, this);
    _tableStatistics->setFont(arial_12B);
    QStringList headers{tr("Тип\nобъекта")
                , tr("Количество\nобъектов")
                , tr("Общее время\nразбора сообщения, с")
                , tr("Среднее время сериализации\nодного объекта, с")};
    _tableStatistics->setHorizontalHeaderLabels(headers);
    _tableStatistics->setColumnWidth(0, 150);
    _tableStatistics->setColumnWidth(1, 150);
    _tableStatistics->setColumnWidth(2, 200);
    _tableStatistics->setColumnWidth(3, 260);
    _tableStatistics->setFixedWidth(780);
    auto header = _tableStatistics->horizontalHeader();
    header->setFixedHeight(46);
    gridl->addWidget(_tableStatistics,  5, 0,   13, 3);
    // ---------------------------------------
    setLayout(gridl);
    setFixedWidth(1315);
    setMinimumHeight(800);
    connect(_pbStart, SIGNAL(clicked()),            this, SLOT(slotStartServer()) );
    connect(_pbClearOutput, SIGNAL(clicked()),      this, SLOT(clearOutput()) );
}

Widget::~Widget()
{
    qDebug() << "Widget::~dtor() called";
}

void Widget::showEvent(QShowEvent* /*event*/)
{
    // Т.к. метод shared_from_this() не может быть использован в конструкторе
    // класса Widget, конструировать объект класса Restarter приходится
    // в данном методе.
    if (_restarter == nullptr)
    {
        _restarter = std::make_unique<Restarter>(shared_from_this());
        // Активируем сервер:
        _pbStart->click();
    }
//    qDebug() << "Widget::showEvent() called!";
}


void Widget::printJsonObjAmount(ulong size)
{
    QString time = _time->currentTime().toString("hh:mm:ss.zzz");
    QString timeAndSize = time + tr("  Сериализовано ") + QString::number(size) + tr(" объектов.");
    _teStatistics->setTextColor(QColor(160,60,0));
    _teStatistics->append(timeAndSize);
    _teStatistics->setTextColor(Qt::black);
}

void Widget::printTimeAndSizeInfo(int msgSize)
{
    QString time = _time->currentTime().toString("hh:mm:ss.zzz");
    QString timeAndSize = time + tr("  принято ") + QString::number(msgSize) + tr(" байт");
    _teStatistics->append(timeAndSize);
}

void Widget::printParsingResults(MessageParsingResult info)
{
    DataType type;
    uint amount;
    double wholeMsgTime;
    double objAverageTime;
    std::tie(type, amount, wholeMsgTime, objAverageTime) = info;
    printJsonObjAmount(amount);

    _tableStatistics->insertRow(_tableStatistics->rowCount());
    addDataItemToRow(1, amount);
    addDataItemToRow(2, wholeMsgTime);
    addDataItemToRow(3, objAverageTime);
    QString str{};
    if (type == DataType::Json)
        str = "JSON";
    if (type == DataType::Struct)
        str = "Struct";
    addDataItemToRow(0, str);
//    auto item = new QTableWidgetItem();
//    item->setData(Qt::DisplayRole, amount);
//    item->setTextAlignment(Qt::AlignCenter);
//    _tableStatistics->setItem(_tableStatistics->rowCount() - 1, 1, item);
}

void Widget::addDataItemToRow(int column, const QVariant& data)
{
    auto item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, data);
    item->setTextAlignment(Qt::AlignCenter);
    _tableStatistics->setItem(_tableStatistics->rowCount() - 1, column, item);
}


void Widget::processMsg(std::vector<char>& data, int size, ushort portFrom)
{
    printTimeAndSizeInfo(size);
    auto parser = getParser(portFrom);
    parser->parseMsg(data.data(), size);
}

Widget::ShPtrParser Widget::getParser(TcpPort port)
{
    auto it = _parsers.find(port);
    if (it == _parsers.end())
    {
        DataHeader header{0x1002, 0xdddd, 0, 0x1003};
//        EmptyHeader header;
        auto hd = std::make_shared<HeaderDescription<DataHeader>>(header);
        auto p = ParsersManager<HeaderDescription<DataHeader>>::create(shared_from_this(), hd);
        _parsers.insert({port, p});
        return p;
    }
    else {
        return (*it).second;
    }
}

void Widget::slotStartServer()
{
    QString st;
    st = _lePort->text();
    quint16 port = st.toUShort();
    if (port == 0)
    {
        qDebug() << "Widget::slotStartServer() error! Unkorrect port value!";
        return;
    }

    st = _leRestartValue->text();
    quint32 restartValue = st.toUInt();
    if (restartValue > 0 && restartValue < 4) // Искусственно исключаем слишком маленькие интервалы
    {
        qDebug() << "Widget::slotStartServer() error! Unkorrect restartValue!";
        return;
    }

    TcpServerMode mode;
    QPalette* currPalette{nullptr};
    if (restartValue == 0)
    {
        mode = TcpServerMode::ChangeBusyPort;
        st = tr("Сервер работает в режиме\n\"Один порт из множества\"");
        currPalette = _yellow0;
    }
    else
    {
        mode = TcpServerMode::RandPortRestart;
        st = tr("Сервер работает в режиме\n\"Авторестарт со сменой порта\"");
        currPalette = _red1;
    }

    _restarter->restart(port, mode, restartValue);
    disconnect(_pbStart, SIGNAL(clicked()),     this, SLOT(slotStartServer()) );
       connect(_pbStart, SIGNAL(clicked()),     this, SLOT(slotStopServer()) );
    _pbStart->setText(st);
    _pbStart->setPalette(*currPalette);
}

void Widget::slotStopServer()
{
    _restarter->close();
    disconnect(_pbStart, SIGNAL(clicked()),     this, SLOT(slotStopServer()) );
       connect(_pbStart, SIGNAL(clicked()),     this, SLOT(slotStartServer()) );
    _pbStart->setText(tr("Активировать tcp-сервер"));
    _pbStart->setPalette(*_green1);
    _pbConnectionStatus->setPalette(*_red0);
}

//void Widget::updateGui(...)
//{
//    lb_receive->setText(QString::number(chRcv_));
//    lb_send->setText(QString::number(chSnd_));
//    lb_ntpd->setText(QString::number(tpd));
//}

void Widget::showServPort(quint16 port)
{
    QString str;
    str.setNum(port);
    _lbCurrentPort->setText(str);
    clearLabels(ClearLabelsPolicy::ExceptFirst);
}

void Widget::slotCliConnected(quint16 port)
{
    QString s("Client with port ");
    s = s + QString::number(port) + " connected";
    _pbConnectionStatus->setPalette(*_green0);
//    qDebug() << s;
    _teErrors->append(s);
}

void Widget::slotCliDisconnected(quint16 port)
{
    QString s("Client with port ");
    s = s + QString::number(port) + " disconnected";
    _pbConnectionStatus->setPalette(*_red0);
//    qDebug() << s;
    _teErrors->append(s);
    // Удаляем связанный экземпляр парсера:
    _parsers.erase(port);
}

void Widget::clearLabels(ClearLabelsPolicy fl)
{
    if (fl == ClearLabelsPolicy::All)
    {
        _lbCurrentPort->clear();
    }
//    if (fl == ClearLabelsPolicy::ExceptFirst)
//    {}
    _pbConnectionStatus->setPalette(*_red0);
    _teStatistics->clear();
}

void Widget::clearOutput()
{
    _teStatistics->clear();
    for (auto i = _tableStatistics->rowCount(); i >= 0; i--)
    {
        _tableStatistics->removeRow(i);
//        qDebug() << "Rows:" << _tableStatistics->rowCount()
//                 << "\nRemove row " << i;
    }
}

