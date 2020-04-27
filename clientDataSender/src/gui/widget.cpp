#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QString>
#include <QDebug>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <algorithm>
#include <string>
#include <netinet/in.h>
#include "widget.h"
#include "gui/netsettingsdialog.h"
#include "tcpsmartclient.h"
#include "averagetime.h"
#include "somestruct.h"

Widget::Widget()
{
    QFont   arial_12("Arial", 12, QFont::Normal);
    QFont   arial_14("Arial", 14, QFont::Normal);
    _notoSans11 = new QFont("Noto Sans", 11, QFont::Normal);
    this->setFont(*_notoSans11);
    // Графические объекты:
    _settingsWidget = new NetSettingsDialog(this);
    QGridLayout* grid = new QGridLayout(this);
    setLayout(grid);
    setFixedWidth(880);
//    setMinimumSize(750,420);
    // Первый ряд:
    QLabel* lb01 = new QLabel(tr("Количество объектов\n(не более 6000 шт):"));
    lb01->setFixedWidth(180);
    _leObjAmount = new QLineEdit();
    _leObjAmount->setFixedWidth(90);
    QString text;
    _leObjAmount->setText(text.setNum(_jsonObjAmount));
    _pbSendJson = new QPushButton(tr("Отправить данные\nв формате JSON"));
    _pbSendJson->setMinimumSize(100, 80);
    // Второй ряд:
    QLabel* lb02 = new QLabel(tr("Отчеты по передаче данных:"));
    _teSendingResult = new QTextEdit();
    _teSendingResult->setFixedWidth(580);
    _teSendingResult->setFont(arial_14);
    _pbSendStruct = new QPushButton(tr("Отправить данные\nв виде структур"));
    _pbSendStruct->setMinimumSize(100, 80);
    _pbOpenSettingsWidget = new QPushButton(tr("Изменить\nсетевые настройки"));
    _pbOpenSettingsWidget->setMinimumSize(100, 60);

    grid->addWidget(lb01,   0, 0);
    grid->addWidget(_leObjAmount,   0, 1,   1, 2);
    grid->addWidget(_pbOpenSettingsWidget,   0, 4,   2, 2);
    grid->addWidget(_pbSendJson,    2, 3,   2, 3, Qt::AlignTop);
    grid->addWidget(lb02,   1, 0,   1, 3, Qt::AlignBottom | Qt::AlignHCenter);
    grid->addWidget(_teSendingResult,       2, 0,   8, 3);
    grid->addWidget(_pbSendStruct,  4, 3,   2, 3, Qt::AlignTop);
    // ------------------------------
    this->initHeaderParts();
    this->slotClientRestart();
    _sendingTimer = new AverageTime();

    QObject::connect(_pbSendJson, SIGNAL(clicked()),        this, SLOT(sendJsonData()) );
    QObject::connect(_pbSendStruct, SIGNAL(clicked()),      this, SLOT(sendStructData()) );
//    QObject::connect(_smartClient.get(), SIGNAL(connected()),     this, SLOT(slotConnected()) );
    QObject::connect(_pbOpenSettingsWidget, SIGNAL(clicked()), this, SLOT(slotOpenSettingsWidget()) );

}

// TODO: Memory leak for some layouts, проверить на walgrind:
Widget::~Widget()
{
//    delete _settingsWidget;
    delete _pbSendJson;
    delete _leObjAmount;
    delete _teSendingResult;
}

void Widget::initHeaderParts()
{
    // 2 байта, определяющие начало сообщения:
    auto pref = htons(_prefix);
    _prefixStr.resize(sizeof(pref));
    memcpy(&_prefixStr[0], &pref, sizeof(pref));
    // 2 байта, определяющие конец сообщения:
    auto postf = htons(_postfix);
    _postfixStr.resize(sizeof(postf));
    memcpy(&_postfixStr[0], &postf, sizeof(postf));
    // 2 байта, определяющие тип передаваемых данных:
    _types.insert({DataType::Json,   TypeInfo{"JJ", tr("JSON-объекты")}} );
    _types.insert({DataType::Struct, TypeInfo{"SS", tr("структуры")}} );
}

void Widget::slotOpenSettingsWidget()
{
    _settingsWidget->show();
    qDebug() << _settingsWidget->size();
}

void Widget::slotClientRestart()
{
    auto uptr = std::make_unique<TcpSmartClient>(
                QHostAddress(_settingsWidget->ip()), _settingsWidget->port());

    _smartClient = std::move(uptr);
    QObject::connect(_smartClient.get(), SIGNAL(connected()),   this, SLOT(slotConnected()) );
    _settingsWidget->hide();
}


// TODO: Дописать шаблон так, чтобы он работал только
// с простыми типами (use decltype или др.):
template<typename T>
std::string Widget::convertLen(T len) const
{
    std::string dataLen;
    dataLen.resize(sizeof(T));
    memcpy(&dataLen[0], &len, sizeof(T));
    std::reverse(dataLen.begin(), dataLen.end());
//    qDebug() << len;
    return dataLen;
}

void Widget::sendJsonData()
{
//    ptree tree = jsonFromFile("../json_files/network.json");
    ptree tree = jsonFromFile("../json_files/some_struct.json");
//    ptree tree = jsonFromFile("../json_files/network_agent.json");
//    qDebug() << tree.get<ushort>("agentLocalPort");

    std::stringstream oss;
    uint filesAmount = _leObjAmount->text().toUInt();
    for (uint i = 0; i < filesAmount; ++i)
        boost::property_tree::json_parser::write_json(oss, tree);

    std::string data = oss.str();
    if (filesAmount > 0)
    {
        qDebug() << "data.size() = " << data.size() << "\nРазмер json-файла = "
                 << static_cast<double>(data.size()/filesAmount);
    }

    // Удаляем пробелы из массива данных (это увеличивает скорость разбора
    // сообщения на стороне сервера ~ на 18%):
//    _sendingTimer->fixStartTime();
//    boost::erase_all(data, " ");
//    _sendingTimer->fixEndTime();
//    auto delta = _sendingTimer->getAverage();
//    qDebug() << "Time of whitespace-removing = " << QString::number(delta);

    //    data.erase(boost::remove_if(data, ::isspace), data.end() - 2);
    //    boost::remove_erase_if(str, boost::is_any_of("123")); // is_any_of(" \n")
    // Формируем сообщение:
    data = _prefixStr + std::get<0>(_types.at(DataType::Json)) +
            convertLen(static_cast<long>(data.size()) ) + data + _postfixStr;
    qDebug() << "packss size() WITH HEADER = " << data.size();
    // -------------------------------
    auto res = _smartClient->sendToServer(data.c_str(),
                                          static_cast<qint64>(data.size()));
    updateGui(res, DataType::Json);
}


ptree Widget::jsonFromFile(const std::string &fileName) const
{
    // Удаление "пробелов" из считанного файла перед вызовом функции
    // read_json(). Решение не имеет смысла, т.к. write_json() возвращает
    // удаленные ранее "пробелы".
//    std::ifstream stream;
//    stream.open(fileName);
//    std::stringstream data;
//    std::string s;
//    while (getline(stream, s))
//    {
//        std::stringstream line(s);
//        while (line >> s)
//            data << s;
//    }
//    qDebug() << "Data: " << data.str().size() << '\n' << data.str().c_str();

    ptree jsonTree;
    try
    {   // Обрабатываем ситуацию "нечитаемый файл":
//        boost::property_tree::read_json(data, jsonTree);
        boost::property_tree::read_json(fileName, jsonTree);
    }
    catch (json_parser_error jsonError)
    {
        qDebug() << "Can't parse file" << jsonError.filename().c_str()
                 << "\nMay be, syntax error or another one.";
        return jsonTree; // Стоит ли возвращать пустое дерево?
    }
    return jsonTree;
}

void Widget::updateGui(qint64 dataSize, DataType type)
{
    auto size = static_cast<long double>(dataSize);
    long double value;
    QString infoUnits;
    if (size > 1024)
    {
        value = size / 1024;
        infoUnits = tr(" Кбайт");
    }
    else
    {
        value = size;
        infoUnits = tr(" Байт");
    }

    if (value > 1024)
    {
        value = value / 1024;
        infoUnits = tr(" Мбайт");
    }

    QString amount;
    if (_leObjAmount->text().toInt() > 0)
        amount = _leObjAmount->text() + tr(" шт.");
    else {
        amount = QString::number(0);
    }

    QString text("Формат данных: ");
    text += std::get<1>(_types.at(type)) + ", " + amount + ",  " +
            QString::number(static_cast<double>(value)) + infoUnits;
    _teSendingResult->append(text);
}

void Widget::slotConnected()
{
    qDebug() << "connected with port" << QString::number(_settingsWidget->port());
}

void Widget::sendStructData()
{
    SomeStruct testStruct{};
    addTestData(testStruct);

    std::string data;
    // Вместо шаблонной функции здесь можно использовать htonl():
    std::string structLen = this->convertLen(static_cast<int>(sizeof(testStruct)) );
    auto dataBlockLen = structLen.size() + sizeof(testStruct);
    uint filesAmount = _leObjAmount->text().toUInt();
    data.resize(filesAmount * dataBlockLen);
    for (ulong i = 0; i < data.size(); i = i + dataBlockLen)
    {
        // Для двух объектов std::string можно использовать replace:
        memcpy(&data[i], &structLen[0], structLen.size());
        memcpy(&data[i + structLen.size()], &testStruct, sizeof(testStruct));
    }

    if (filesAmount > 0)
        qDebug() << "\nРазмер одной структуры с блоком длины ="
                 << sizeof(testStruct) + structLen.size();
    data = _prefixStr + std::get<0>(_types.at(DataType::Struct)) +
            convertLen(static_cast<long>(data.size())) + data + _postfixStr;
    qDebug() << "Общая длина сообщения =" << data.size();
    auto res = _smartClient->sendToServer(data.c_str(),
                                          static_cast<qint64>(data.size()));
    updateGui(res, DataType::Struct);
}


void Widget::addTestData(SomeStruct &mySturct)
{
    mySturct.a = 0xCC;
    mySturct.b = 0xDD;
    mySturct.c = 0xAA;
    mySturct.d = 0xBB;
    mySturct.e = 0x0BB8;
    mySturct.f = 0xEE;

    mySturct.g = 0x30;
    mySturct.h = 0x01;
    mySturct.i = 0x31;
    mySturct.j = 0x32;
    mySturct.k = 0x04;
}

