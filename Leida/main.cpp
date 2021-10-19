#include "leida.h"
#include "log.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);
    qInfo() << "APP START...";

    QApplication a(argc, argv);
    Leida w;
    w.show();

    return a.exec();
}
