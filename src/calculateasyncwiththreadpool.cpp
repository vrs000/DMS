#include "calculateasyncwiththreadpool.h"

int CalculateAsyncWithThreadPool::count = 0;
int CalculateAsyncWithThreadPool::missed_variation = 0;

QVector<double> CalculateAsyncWithThreadPool::hardRatingPart;
QVector<double> CalculateAsyncWithThreadPool::softRatingPart;

CalculateAsyncWithThreadPool::CalculateAsyncWithThreadPool()
{
    hardRatingPart = QVector<double>(IO::ProjectsNames.size(), 0);
    softRatingPart = QVector<double>(IO::ProjectsNames.size(), 0);

    count = 0;
    missed_variation = 0;
}

void CalculateAsyncWithThreadPool::Calculate(QVector<double> &currentSet, int maxN, int curPosIndex)
{
    if((curPosIndex < maxN - 1))
    {
        double lim = 1 - DataProcessing::Sum(currentSet, curPosIndex);

        for(double i = 0; i <= lim + 0.00001; i += DataProcessing::CrushingStep)
        {
            currentSet[curPosIndex] = i;
            Calculate(currentSet, maxN, curPosIndex + 1);
        }
    }
    else
        if(curPosIndex == maxN - 1)
        {
            double rest = 1 - DataProcessing::Sum(currentSet, curPosIndex);
            if (rest - 0.00001 < 0)
                rest = 0;

            if (fabs(static_cast<double>(rest - DataProcessing::CrushingStep)) < 0.00001)
                rest = DataProcessing::CrushingStep;

            currentSet[curPosIndex] = rest;

            count++;
            emit CountChanged(count);


            PerformOneCalculation* thread = new PerformOneCalculation(currentSet);
            QThreadPool::globalInstance()->start(thread);
        }
}

void CalculateAsyncWithThreadPool::Calculate(double currentSet[], int maxN, int curPosIndex)
{
    if((curPosIndex < maxN - 1))
    {
        double lim = 1 - DataProcessing::Sum(currentSet, curPosIndex);

        for(double i = 0; i <= lim + 0.00001; i += DataProcessing::CrushingStep)
        {
            currentSet[curPosIndex] = i;
            Calculate(currentSet, maxN, curPosIndex + 1);
        }
    }
    else
        if(curPosIndex == maxN - 1)
        {
            double rest = 1 - DataProcessing::Sum(currentSet, curPosIndex);
            if (rest - 0.00001 < 0)
                rest = 0;

            if (fabs(static_cast<double>(rest - DataProcessing::CrushingStep)) < 0.00001)
                rest = DataProcessing::CrushingStep;

            currentSet[curPosIndex] = rest;

            count++;
            emit CountChanged(count);


            PerformOneCalculation* thread = new PerformOneCalculation(currentSet);
            QThreadPool::globalInstance()->start(thread);
        }
}


void CalculateAsyncWithThreadPool::run()
{
    QVector<double> set(IO::IndicatorsNames.size(), 0);
    double* set1 = new double[IO::IndicatorsNames.size()];

    for (int i=0; i < IO::IndicatorsNames.size(); i++)
        set1[i] = 0;

    Calculate(set1, IO::IndicatorsNames.size(), 0);
}

PerformOneCalculation::PerformOneCalculation(const QVector<double> &weights)
{
    Weights = weights;
}

PerformOneCalculation::PerformOneCalculation(double weights[])
{
    _Weights = weights;
}

void PerformOneCalculation::run()
{
    bool isSuitable = true;
    QVector<double> hardRatings(IO::ProjectsNames.size());
    QVector<double> softRatings(IO::ProjectsNames.size());

    auto getMax = [](QVector<double> list)
    {
        double max = list[0];
        for (int i = 0; i < list.size(); i++)
            max = list[i] > max ? list[i] : max;
        return max;
    };
    auto getMin = [](QVector<double> list)
    {
        double min = list[0];
        for (int i = 0; i < list.size(); i++)
            min = list[i] < min ? list[i] : min;
        return min;
    };



    auto res = DataProcessing::GetLinearConvolutionResult(_Weights);


    if ((DataProcessing::PrefferedMetrics.size() != 0) && (DataProcessing::RejectedMetrics.size() != 0))
    {
        isSuitable = true;
        for (int i = 0; i < DataProcessing::PrefferedMetrics.size(); i++)
            for (int j = 0; j < DataProcessing::RejectedMetrics.size(); j++)
            {
                isSuitable = isSuitable &&
                        (_Weights[DataProcessing::PrefferedMetrics[i]] > _Weights[DataProcessing::RejectedMetrics[j]]);

            }

        if (!isSuitable)
        {
            DataProcessing::missed_variation++;
            CalculateAsyncWithThreadPool::missed_variation++;
            return;
        }
    }




    //Мягкий рейтинг
//    for (int i = 0; i < IO::ProjectsNames.size(); i++)
//        softRatings[i] += res[i] / (1.0 - getMin(res));



//    //Жёсткий рейтинг
//    if (res.count(getMax(res))>1)
//    {
//        int q = res.count(getMax(res));
//        for (int i = 0; i < res.size(); i++)
//            hardRatings[i] += (res[i] == getMax(res)) ? 1.0/q : 0;
//    }
//    else
//        hardRatings[res.indexOf(getMax(res))]++;




    for (int i = 0; i < hardRatings.size(); i++)
    {
        CalculateAsyncWithThreadPool::hardRatingPart[i] += hardRatings[i];
        CalculateAsyncWithThreadPool::softRatingPart[i] += softRatings[i];
    }

//    bool isSuitable = true;
//    QVector<double> hardRatings(IO::ProjectsNames.size());
//    QVector<double> softRatings(IO::ProjectsNames.size());

//    auto getMax = [](QVector<double> list)
//    {
//        double max = list[0];
//        for (int i = 0; i < list.size(); i++)
//            max = list[i] > max ? list[i] : max;
//        return max;
//    };
//    auto getMin = [](QVector<double> list)
//    {
//        double min = list[0];
//        for (int i = 0; i < list.size(); i++)
//            min = list[i] < min ? list[i] : min;
//        return min;
//    };



//    auto res = DataProcessing::GetLinearConvolutionResult(Weights);


//    if ((DataProcessing::PrefferedMetrics.size() != 0) && (DataProcessing::RejectedMetrics.size() != 0))
//    {
//        isSuitable = true;
//        for (int i = 0; i < DataProcessing::PrefferedMetrics.size(); i++)
//            for (int j = 0; j < DataProcessing::RejectedMetrics.size(); j++)
//            {
//                isSuitable = isSuitable &&
//                        (Weights[DataProcessing::PrefferedMetrics[i]] > Weights[DataProcessing::RejectedMetrics[j]]);

//            }

//        if (!isSuitable)
//        {
//            DataProcessing::missed_variation++;
//            CalculateAsyncWithThreadPool::missed_variation++;
//            return;
//        }
//    }




//    //Мягкий рейтинг
//    for (int i = 0; i < IO::ProjectsNames.size(); i++)
//        softRatings[i] += res[i] / (1.0 - getMin(res));



//    //Жёсткий рейтинг
//    if (res.count(getMax(res))>1)
//    {
//        int q = res.count(getMax(res));
//        for (int i = 0; i < res.size(); i++)
//            hardRatings[i] += (res[i] == getMax(res)) ? 1.0/q : 0;
//    }
//    else
//        hardRatings[res.indexOf(getMax(res))]++;




//    for (int i = 0; i < hardRatings.size(); i++)
//    {
//        CalculateAsyncWithThreadPool::hardRatingPart[i] += hardRatings[i];
//        CalculateAsyncWithThreadPool::softRatingPart[i] += softRatings[i];
//    }
}
