#include "mainwindow.h"
#include <QApplication>
//#include <tests/dataprocessing_test.h>
//#include <QtTest>
#include <iostream>
#include <cstdlib>
#include <cstdio>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QTest::qExec(new DataProcessing_Test, argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
