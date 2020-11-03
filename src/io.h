#ifndef IO_H
#define IO_H

#include <QMainWindow>
#include <QObject>
#include <QSharedDataPointer>
#include <QWidget>
#include <QList>
#include <QVector>
#include <QTableWidget>
#include <dataprocessing.h>
#include <QDebug>
#include <QHeaderView>
#include <QtMath>
#include <QAxObject>
#include <QtGui>
#include <solution.h>
#include <QTime>
#include <QDate>
#include "xlsxdocument.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include <QFileDialog>
#include <QString>
#include <startupconfigform.h>
#include <QProgressDialog>
//#include <QJSEngine>

using namespace QXlsx;

class GenerateWeightsAndCalculateRatingsAsync;

class IO
{
public:
    static QVector<QVector<double>> BaseTable;
    static QVector<QString> IndicatorsNames;
    static QVector<QString> ProjectsNames;

    static bool IsPaint;
    static bool IsBuild;

    static void SetStartData(QVector<QVector<double>> &table, QVector<QString> &IndicatorsNames, QVector<QString> &ProjectsNames);

    static void FillingTables(QTableWidget* input, QTableWidget* output);

public:
    static void OpenExelFile(QString Path);
    static void OpenExelFile1(QString Path);
    static void OpenExelFile(QString Path, int StartX, int StartY);
    static void OpenCSVFile(QString Path);

    static void MakeExcelReport(QList<Solution> solutions);

    static void SaveExcelFile(QList<Solution> solutionsList);

private:
    static QString FilePath;
    static void InitList(int rows, int columns);

};

#endif // IO_H
