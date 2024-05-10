#include "gamingwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GamingWidget w;
    w.show();

    return a.exec();
}
