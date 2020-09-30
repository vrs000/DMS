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



class CalculateRatingsAsync;

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

    static QTableWidget* inputTable;
    static QTableWidget* outputTable;

    static QString OpenedSolutionName;

    static DataProcessing* instance;

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


    static CalculateRatingsAsync* thread1;
    static CalculateRatingsAsync* thread2;
    static CalculateRatingsAsync* thread3;
    static CalculateRatingsAsync* thread4;
    static CalculateRatingsAsync* thread5;
    static CalculateRatingsAsync* thread6;
    static CalculateRatingsAsync* thread7;
    static CalculateRatingsAsync* thread8;

    static int Count1;
    static int Count2;
    static int Count3;
    static int Count4;
    static int Count5;
    static int Count6;
    static int Count7;
    static int Count8;

    static void ResetCounts();

private:
    static void GetNextNum(QVector<double>& currentSet, int maxN, int curPosIndex);
    static double Sum(QVector<double> set, int elementsCount);


private slots:
    void UpdateProgressBar();

    void UpdateCount1(int count);
    void UpdateCount2(int count);
    void UpdateCount3(int count);
    void UpdateCount4(int count);
    void UpdateCount5(int count);
    void UpdateCount6(int count);
    void UpdateCount7(int count);
    void UpdateCount8(int count);

    //OK
    void Finished2Threads();

    //Need Test
    void Finished4Threads();

    //Need Test
    void Finished8Threads();
};




#endif // DATAPROCESSING_H
