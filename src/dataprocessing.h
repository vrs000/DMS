#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QVector>
#include <QList>
#include <math.h>
#include <io.h>
#include <QDebug>
#include <multi_threading_methods.h>
#include <QProgressBar>
#include <QProgressDialog>
#include <progressbarform.h>

//#include <thread>
//#include <chrono>

class DataProcessing : public QObject
{
    Q_OBJECT

public:
    static QVector<QString> PriorityList;
    static QVector<double> MaximumIndicators;
    static QVector<double> MinimumIndicators;

    static QVector<QVector<double>> NormalizedTable;
    static QVector<QVector<double>> WeightsTable;

    static double CrushingStep;
    static int missed_variation;

    static QVector<double> SoftRatings;
    static QVector<double> HardRatings;

    static QVector<int> PrefferedMetrics;
    static QVector<int> RejectedMetrics;

    static QProgressBar* bar;

    static int CurrentIterationCount;

public:

    static void FindMaxMinIndicators( QVector<QVector<double>> BaseTable);
    static void CalculateNormalizedTable( QVector<QVector<double>> BaseTable);
    static void GenerateWeightsList();

    static void CalculateRatings();
    static void CalculateRatingsIn2Threads();
    static void CalculateRatingsIn4Threads();
    static void CalculateRatingsIn8Threads();


    static void SetMetrics(QVector<int> Preferred, QVector<int> Rejected);
    static void SetPriorityList(QVector<QString> list);


    static void MakeCalculations(QVector<QString> priorityList = QVector<QString>(),
                                 QVector<int> Preferred = QVector<int>(),
                                 QVector<int> Rejected = QVector<int>());


    static QVector<double> GetLinearConvolutionResult(QVector<double> weights);


private:
    static void GetNextNum(QVector<double>& currentSet, int maxN, int curPosIndex);
    static double Sum(QVector<double> set, int elementsCount);

};

#endif // DATAPROCESSING_H
