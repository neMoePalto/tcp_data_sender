#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QLineEdit>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include "somestruct.h"

using namespace boost::property_tree;
class QTextEdit;
class QPushButton;
class TcpSmartClient;
class AverageTime;
class NetSettingsDialog;
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget();
    ~Widget();
public slots:
    void slotClientRestart();
private slots:
    void sendJsonData();
    void sendStructData();
    void slotConnected();
    void slotOpenSettingsWidget();
private:
    std::unique_ptr<TcpSmartClient> _smartClient;
    QPushButton*    _pbSendJson;
    QPushButton*    _pbSendStruct;
    QPushButton*    _pbOpenSettingsWidget;
    QTextEdit*      _teSendingResult;
    QLineEdit*      _leObjAmount;
    QFont* _notoSans11;

    NetSettingsDialog* _settingsWidget;
    const ushort    _prefix  = 0x1002;
    const ushort    _postfix = 0x1003;
    std::string _prefixStr{};
    std::string _postfixStr{};
    enum class DataType
    {
        Json,
        Struct
    };
    using TypeInfo = std::tuple<std::string, QString>;
    std::map<DataType, TypeInfo> _types;
    const uint      _jsonObjAmount = 1;
    AverageTime*    _sendingTimer;

    ptree jsonFromFile(const std::string& fileName) const;
    void updateGui(qint64 dataSize, DataType type);
    void initHeaderParts();
    void addTestData(SomeStruct& mySturct);
    template<typename T>
    std::string convertLen(T len) const;
};

#endif // WIDGET_H
