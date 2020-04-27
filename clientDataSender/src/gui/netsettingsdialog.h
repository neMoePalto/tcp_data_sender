#ifndef NETSETTINGSDIALOG_H
#define NETSETTINGSDIALOG_H
#include <QDialog>

class QLabel;
class QLineEdit;
class QGridLayout;
class QSettings;
class Widget;
class NetSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    NetSettingsDialog(Widget* wgt);
    ~NetSettingsDialog();
    ushort  port() const;
    QString ip() const;
private:
    QGridLayout* _grid2;
    QLabel*      _lb2;
    QLabel*      _lbIp;
    QLabel*      _lbPort;
    QLineEdit*   _leIp;
    QLineEdit*   _lePort;
    QPushButton* _pbApplySettings;
    QFont* _notoSans11; // Font from common widget

    Widget*    _widget;
    QSettings* _settings;

    void usePreviousSettings(QLineEdit* leForIp, QLineEdit* leForPort);
};

#endif // NETSETTINGSDIALOG_H
