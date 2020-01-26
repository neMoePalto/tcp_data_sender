#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QLineEdit>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include "somestruct.h"

using namespace boost::property_tree;
class QLabel;
class QTextEdit;
class QPushButton;
class QGridLayout;
class QSettings;
class TcpSmartClient;
class AverageTime;
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget();
    ~Widget();
private slots:
    void sendJsonData();
    void sendStructData();
    void slotConnected();
    void slotOpenSettingsWidget();
    void slotClientRestart();
private:
    std::unique_ptr<TcpSmartClient> _smartClient;
    QPushButton*    _pbSendJson;
    QPushButton*    _pbSendStruct;
    QPushButton*    _pbOpenSettingsWidget;
    QTextEdit*      _teSendingResult;
    QLineEdit*      _leObjAmount;
    QFont* _notoSans11;
    // Net settings widget:
    void initSettingsWidget();
    QDialog*        _wgtNetSettings;
    QGridLayout*    _grid2;
    QLabel*     _lb2;
    QLabel*     _lbIp;
    QLabel*     _lbPort;
    QLineEdit*  _leIp;
    QLineEdit*  _lePort;
    QPushButton*    _pbApplySettings;

    QSettings* _settings;
    void usePreviousSettings(QLineEdit* leForIp, QLineEdit* leForPort);
    // --------------------
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
