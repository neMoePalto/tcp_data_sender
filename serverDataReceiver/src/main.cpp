#include <QApplication>
#include <QTimer>
#include "widget.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto w = std::make_shared<Widget>();
    QObject::connect(w.get(), &Widget::quitFromApp,
                     QCoreApplication::quit);
    QTimer::singleShot(0, w.get(), &Widget::slotStartServer);
    w->show();

    return app.exec();
}
