#ifndef ASYNCCALCULATION_H
#define ASYNCCALCULATION_H


#include <QThread>
#include <QList>
#include <QWidget>
#include <io.h>
#include <dataprocessing.h>
#include <startupconfigform.h>
#include <QMutex>
#include <QTime>

class DataProcessing;
class IO;

class GenerateWeightsAndCalculateRatingsAsync : public QThread
{
    Q_OBJECT

private:
    QVector<double> HardRatings;
    QVector<double> SoftRatings;

    QTime time;

    int FirstIterationIndex;
    int LastIterationIndex;

    int Count = 0;
    int GeneratedIterCount = 0;

    bool IsNeedToFinish = false;

private:
    int *PrefferedMetrics;
    int *RejectedMetrics;

    double h;
    double sum(double set[], int index);

    QVector<double> hardRatings;
    QVector<double> softRatings;

    int PrefferedMetricsCount = 0;
    int RejectedMetricsCount = 0;

    int ProjectsCount = 0;


private:
    double getMax(double *list);
    double getMin(double *list);
    int COUNT(double* list, double value);
    int IndexOf(double* list, double value);

public:
    GenerateWeightsAndCalculateRatingsAsync();
    GenerateWeightsAndCalculateRatingsAsync(int firstIterationIndex,  int lastIterationIndex, double step);


    ~GenerateWeightsAndCalculateRatingsAsync();


    QVector<double> GetHardRatings();
    QVector<double> GetSoftRatings();

    int GetCount();

    void Calculate(QVector<double>& currentSet, int maxN, int curPosIndex);
    void Calculate(double currentSet[], int maxN, int curPosIndex);

    static QMutex mutex;

signals:
    void CountChanged(int count);

protected:
    void run() override;
};

#endif // ASYNCCALCULATION_H
