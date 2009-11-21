#ifdef QTOPIA

#include "qspectemu.h"
#include <qtopiaapplication.h>
QTOPIA_ADD_APPLICATION(QTOPIA_TARGET,QSpectemu)
QTOPIA_MAIN

#else

#include <QtGui/QApplication>
#include "qspectemu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSpectemu w;
    w.show();
    return a.exec();
}

#endif
