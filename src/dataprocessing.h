#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QVector>
#include <QList>
#include <math.h>
#include <io.h>
#include <QDebug>

#include <multi_threading_methods.h>
#include <asynccalculation.h>
#include <QProgressBar>
#include <QProgressDialog>
#include <mainwindow.h>
#include <calculateasyncwiththreadpool.h>



class CalculateRatingsAsync;
class GenerateWeightsAndCalculateRatingsAsync;
class CalculateAsyncWithThreadPool;
class MainWindow;


class DataProcessing : public QObject
{
    Q_OBJECT

public:
    static MainWindow* mainWindow;

    static QString NotParsedImportanceGroupOfProjects;
    static QString NotParsedImportanceGroupOfIndicators;

    static QVector<QString> PriorityList;
    static QVector<double> MaximumIndicators;
    static QVector<double> MinimumIndicators;

    static double** normTable;
    static QVector<QVector<double>> NormalizedTable;
    static QVector<QVector<double>> WeightsTable;
    static QVector<QString> ParettoSetProjects;
    static QList<int> ParettoSetProjectsIndexes;

    static double CrushingStep;
    static int missed_variation;

    static QVector<double> SoftRatings;
    static QVector<double> HardRatings;

    static QVector<int> PrefferedMetrics;
    static QVector<int> RejectedMetrics;

    static QVector<QString> PrefferedProjects;
    static QVector<QString> RejectedProjects;

    static QString GetPassedTimeElapsed(int ms);

    static QProgressBar* bar;

    static int CurrentIterationCount;

    static QTableWidget* inputTable;
    static QTableWidget* outputTable;

    static QString OpenedSolutionName;

    static DataProcessing* instance;

    static QTime time;

    static int UsedThreadCount;
    static int TimeElapsed;
    static QString timeElapsedParsed;

    static int ProjectsCount;
    static int IndicatorsCount;

public:

    static double Fact(double value);
    static unsigned long f(int n);
    static double Factorial(double value);
    static int GetTheoreticalWeightsCount(int ParametersCount, double h);

    static void FindParettoSet();
    static void FindMaxMinIndicators( QVector<QVector<double>> BaseTable);
    static void CalculateNormalizedTable( QVector<QVector<double>> BaseTable);
    static void GenerateWeightsList();

    static void CalculateRatings();
    static void CalculateRatingsIn2Threads();
    static void CalculateRatingsIn4Threads();
    static void CalculateRatingsIn8Threads();


    static void CalculateRatingsIn1ThreadsWithWeights();
    static void CalculateRatingsIn2ThreadsWithWeights();
    static void CalculateRatingsIn4ThreadsWithWeights();
    static void CalculateRatingsIn8ThreadsWithWeights();
    static void CalculateRatingsInAllThreadsWithWeights();

    static void CalculateRatingsWithPool();

    static void SetMetrics(QVector<int> Preferred, QVector<int> Rejected);
    static void SetProjectsPriorities(QVector<QString> Preferred, QVector<QString> Rejected);
    static void SetPriorityList(QVector<QString> list);


    static void MakeCalculations(QVector<QString> priorityList = QVector<QString>(),
                                 QVector<int> Preferred = QVector<int>(),
                                 QVector<int> Rejected = QVector<int>(),
                                 QVector<QString> preferredProjects = QVector<QString>(),
                                 QVector<QString> rejectedProjects = QVector<QString>());


    static QVector<double> GetLinearConvolutionResult(QVector<double> weights);
    static double* GetLinearConvolutionResult(double *weights);


    static CalculateRatingsAsync* thread1;
    static CalculateRatingsAsync* thread2;
    static CalculateRatingsAsync* thread3;
    static CalculateRatingsAsync* thread4;
    static CalculateRatingsAsync* thread5;
    static CalculateRatingsAsync* thread6;
    static CalculateRatingsAsync* thread7;
    static CalculateRatingsAsync* thread8;


    static GenerateWeightsAndCalculateRatingsAsync* th1;
    static GenerateWeightsAndCalculateRatingsAsync* th2;
    static GenerateWeightsAndCalculateRatingsAsync* th3;
    static GenerateWeightsAndCalculateRatingsAsync* th4;
    static GenerateWeightsAndCalculateRatingsAsync* th5;
    static GenerateWeightsAndCalculateRatingsAsync* th6;
    static GenerateWeightsAndCalculateRatingsAsync* th7;
    static GenerateWeightsAndCalculateRatingsAsync* th8;


    static CalculateAsyncWithThreadPool* tp;


    static int Count1;
    static int Count2;
    static int Count3;
    static int Count4;
    static int Count5;
    static int Count6;
    static int Count7;
    static int Count8;

    static int NumberOfTriggring;

    static void ResetCounts();

private:
    static void GetNextNum(QVector<double>& currentSet, int maxN, int curPosIndex);
    static void GetNextNum(double currentSet[], int maxN, int curPosIndex);

    static void SetMainWindowTitle(QString title);
    static int Previous;

public:
    static double Sum(QVector<double> set, int elementsCount);
    static double Sum(double set[], int elementsCount);


private slots:
    void DeleteThreadInstances();
    void UpdateProgressBar();


    void UpdateCount1(int count);
    void UpdateCount2(int count);
    void UpdateCount3(int count);
    void UpdateCount4(int count);
    void UpdateCount5(int count);
    void UpdateCount6(int count);
    void UpdateCount7(int count);
    void UpdateCount8(int count);


    void Finished1Threads();
    void Finished2Threads();
    void Finished4Threads();
    void Finished8Threads();


    void ThreadPoolFinished();
};




#endif // DATAPROCESSING_H
