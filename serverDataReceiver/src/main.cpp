#include <QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto w = std::make_shared<Widget>();
    w->show();

    return app.exec();
}
