#ifndef CALCULATEASYNCWITHTHREADPOOL_H
#define CALCULATEASYNCWITHTHREADPOOL_H

#include <QRunnable>
#include <QObject>
#include <io.h>

class DataProcessing;

class CalculateAsyncWithThreadPool : public QThread

{
    Q_OBJECT

public:
    CalculateAsyncWithThreadPool();

    void Calculate(QVector<double>& currentSet, int maxN, int curPosIndex);
    void Calculate(double currentSet[], int maxN, int curPosIndex);
    void run() override;

    static int count;
    static int missed_variation;

    static QVector<double> hardRatingPart;
    static QVector<double> softRatingPart;

signals:
    void Finished();
    void CountChanged(int value);
};



class PerformOneCalculation : public QRunnable
{

public:
    PerformOneCalculation(const QVector<double>& weights);
    PerformOneCalculation(double weights[]);
    QVector<double> Weights;
    double *_Weights;

    void run() override;

};


#endif // CALCULATEASYNCWITHTHREADPOOL_H
