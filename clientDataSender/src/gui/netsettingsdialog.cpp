#include "netsettingsdialog.h"
#include "widget.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QDebug>

NetSettingsDialog::NetSettingsDialog(Widget* parent)
    : QDialog(parent) // Для центровки данного виджета и его автоматического
    , _widget(parent) // удаления через иерархию QObject
{
    setModal(true);
    setMaximumSize(275, 160);
    _notoSans11 = new QFont("Noto Sans", 11, QFont::Normal);
    setFont(*_notoSans11);
    _grid2 = new QGridLayout();
    setLayout(_grid2);
    _lb2 = new QLabel(tr("Параметры сервера:"));
    _lbIp = new QLabel(tr("IP-адрес:"));
    _lbPort = new QLabel(tr("Tcp-порт:"));
    _leIp = new QLineEdit();
    _leIp->setFixedWidth(180);
    _lePort = new QLineEdit();
    _lePort->setFixedWidth(180);
    usePreviousSettings(_leIp, _lePort);

    _pbApplySettings = new QPushButton(tr("Применить настройки"));
    _grid2->addWidget(_lb2,     0, 0,   1, 2);
    _grid2->addWidget(_lbIp,    1, 0,   1, 1);
    _grid2->addWidget(_leIp,    1, 1,   1, 1);
    _grid2->addWidget(_lbPort,  2, 0,   1, 1);
    _grid2->addWidget(_lePort,  2, 1,   1, 1);
    _grid2->addWidget(_pbApplySettings, 3, 1,   1, 1);

    QObject::connect(_pbApplySettings, &QPushButton::clicked,
                     _widget, &Widget::slotClientRestart);
}

NetSettingsDialog::~NetSettingsDialog()
{   // TODO: Заменить на custom deleter:
    _settings->setValue("IP", _leIp->text());
    _settings->setValue("port", _lePort->text());
    delete _settings;
    qDebug() << "NetSettingsDialog::~dtor()";
}

void NetSettingsDialog::usePreviousSettings(QLineEdit *leForIp, QLineEdit *leForPort)
{
    _settings = new QSettings("./settings.ini", QSettings::IniFormat);
    QString ip = _settings->value("IP").toString();
    leForIp->setText(ip);
    QString port = _settings->value("port").toString();
    leForPort->setText(port);
}

ushort NetSettingsDialog::port() const
{
    return _lePort->text().toUShort();
}

QString NetSettingsDialog::ip() const
{
    return _leIp->text();
}


