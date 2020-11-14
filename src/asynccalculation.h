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
    //legacy
    //-------------------------
    int *PrefferedMetrics;
    int *RejectedMetrics;

    int *PrefferedProjects;
    int *RejectedProjects;
    //-------------------------


    //In work
    //-------------------------
    char* ProjectsRelation;
    char* IndicatorsRelation;

    int *PrefferedMetricsV2;
    int *RejectedMetricsV2;

    int *PrefferedProjectsV2;
    int *RejectedProjectsV2;

    int ProjectsGroupCount = 0;
    int IndicatorsGroupCount = 0;
    //-------------------------




    double h;
    double sum(double set[], int index);


    QVector<double> hardRatings;
    QVector<double> softRatings;


    int PrefferedMetricsCount = 0;
    int RejectedMetricsCount = 0;


    int PrefferedProjectsCount = 0;
    int RejectedProjectsCount = 0;


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
    int missed = 0;
    void Calculate(double currentSet[], int maxN, int curPosIndex);

    static QMutex mutex;

signals:
    void CountChanged(int count);

protected:
    void run() override;
};

#endif // ASYNCCALCULATION_H
