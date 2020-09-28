#ifndef MULTI_THREADING_METHODS_H
#define MULTI_THREADING_METHODS_H


#include <QThread>
#include <QList>
#include <QWidget>
#include <io.h>
#include <dataprocessing.h>
#include <startupconfigform.h>
#include <QMutex>
#include <progressbarform.h>

class OpenExcelFileAsync : public QThread
{
private:
    QString path;
public:
    OpenExcelFileAsync(const QString& path);

protected:
    void run() override;
};


class RunningBarAsync : public QThread // sucked
{

public:
    RunningBarAsync();



protected:
    void run() override;
};




class CalculateRatingsAsync : public QThread
{
private:
    QVector<double> HardRatings;
    QVector<double> SoftRatings;

    int FirstIterationIndex;
    int LastIterationIndex;

    int Count = 0;

public:
    CalculateRatingsAsync();
    CalculateRatingsAsync(int firstIterationIndex,  int lastIterationIndex);

    QVector<double> GetHardRatings();
    QVector<double> GetSoftRatings();

    int GetCount();

    static QMutex mutex;

protected:
    void run() override;
};


class BAR : public QThread
{
public:
    BAR();

protected:
    void run() override;
};


#endif // MULTI_THREADING_METHODS_H
