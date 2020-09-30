#include "dataprocessing.h"
#include <QMessageBox>

QVector<QString> DataProcessing::PriorityList;
QVector<double> DataProcessing::MaximumIndicators;
QVector<double> DataProcessing::MinimumIndicators;

QVector<QVector<double>> DataProcessing::NormalizedTable;
QVector<QVector<double>> DataProcessing::WeightsTable;

double DataProcessing::CrushingStep = 0.3;
int DataProcessing::missed_variation = 0;

QVector<double> DataProcessing::SoftRatings;
QVector<double> DataProcessing::HardRatings;

QVector<int> DataProcessing::PrefferedMetrics;
QVector<int> DataProcessing::RejectedMetrics;

QProgressBar* DataProcessing::bar = nullptr;

int DataProcessing::CurrentIterationCount = 0;

CalculateRatingsAsync* DataProcessing::thread1 = nullptr;
CalculateRatingsAsync* DataProcessing::thread2 = nullptr;
CalculateRatingsAsync* DataProcessing::thread3 = nullptr;
CalculateRatingsAsync* DataProcessing::thread4 = nullptr;
CalculateRatingsAsync* DataProcessing::thread5 = nullptr;
CalculateRatingsAsync* DataProcessing::thread6 = nullptr;
CalculateRatingsAsync* DataProcessing::thread7 = nullptr;
CalculateRatingsAsync* DataProcessing::thread8 = nullptr;


int DataProcessing::Count1 = -1;
int DataProcessing::Count2 = -1;
int DataProcessing::Count3 = -1;
int DataProcessing::Count4 = -1;
int DataProcessing::Count5 = -1;
int DataProcessing::Count6 = -1;
int DataProcessing::Count7 = -1;
int DataProcessing::Count8 = -1;



QTableWidget* DataProcessing::inputTable = nullptr;
QTableWidget* DataProcessing::outputTable = nullptr;

QString DataProcessing::OpenedSolutionName = "";

DataProcessing* DataProcessing::instance = new DataProcessing;

void DataProcessing::FindMaxMinIndicators(QVector<QVector<double> > BaseTable)
{
    MaximumIndicators.clear();
    MinimumIndicators.clear();

    int Rows = BaseTable.size();
    int Columns = BaseTable[0].size();
    double MAX, MIN;

    for (int j=0; j < Columns; j++)
    {
        MAX = -1;
        MIN = 9999999999;

        for (int i = 0; i < Rows; ++i)
        {
            MAX = BaseTable[i][j] > MAX ? BaseTable[i][j] : MAX;
            MIN = BaseTable[i][j] < MIN ? BaseTable[i][j] : MIN;
        }

        MaximumIndicators << MAX;
        MinimumIndicators << MIN;
    }
}

void DataProcessing::CalculateNormalizedTable(QVector<QVector<double> > BaseTable)
{
    int Columns = BaseTable[0].size();
    int Rows = BaseTable.size();


    double max, min;
    QString priority;
    NormalizedTable = QVector<QVector<double>>(Rows, QVector<double>(Columns, 0));

    for (int j = 0; j < Columns; ++j)
    {
        max = MaximumIndicators[j];
        min = MinimumIndicators[j];
        priority = PriorityList[j];

        for (int i = 0; i < Rows; i++)
        {
            NormalizedTable[i][j] = priority == "max" ?(BaseTable[i][j] - min)/(max-min) :  NormalizedTable[i][j];
            NormalizedTable[i][j] = priority == "min" ? 1 - (BaseTable[i][j] - min)/(max-min) :  NormalizedTable[i][j];
        }
    }
}

void DataProcessing::SetPriorityList(QVector<QString> list)
{
    PriorityList = list;
}

void DataProcessing::GetNextNum(QVector<double>& currentSet, int maxN, int curPosIndex)
{
    if (curPosIndex<maxN - 1)
    {
        double lim = 1  - Sum(currentSet, curPosIndex);
        for (double i = 0; i <= lim + 0.00001; i += CrushingStep)
        {
            currentSet[curPosIndex] = i;
            GetNextNum(currentSet, maxN, curPosIndex + 1);
        }
    }
    else
        if (curPosIndex == maxN - 1)
        {
            double rest = 1 - Sum(currentSet, curPosIndex);
            if (rest < 0.00001)
                rest = 0;

            if (fabs(static_cast<double>(rest - CrushingStep)) < 0.00001)
                rest = CrushingStep;


            currentSet[curPosIndex] = rest;

            WeightsTable.append(currentSet);
        }

}

double DataProcessing::Sum(QVector<double> set, int elementsCount)
{
    double Sum = 0.0;
    if (!(set.size() == 0))
    {
        for (int i = 0; i < elementsCount; ++i)
        {
            Sum += set.at(i);
        }
    }
    return Sum;
}

void DataProcessing::UpdateProgressBar()
{
    if (bar->isHidden())
        bar->show();

    if (bar->maximum() != WeightsTable.size())
        bar->setMaximum(WeightsTable.size());

    int res = 0;

    if (Count1 != -1) res += Count1;
    if (Count2 != -1) res += Count2;
    if (Count3 != -1) res += Count3;
    if (Count4 != -1) res += Count4;
    if (Count5 != -1) res += Count5;
    if (Count6 != -1) res += Count6;
    if (Count7 != -1) res += Count7;
    if (Count8 != -1) res += Count8;

    bar->setValue(res);

}

void DataProcessing::UpdateCount1(int count)
{
    Count1 = count;
}

void DataProcessing::UpdateCount2(int count)
{
    Count2 = count;
}

void DataProcessing::UpdateCount3(int count)
{
    Count3 = count;
}

void DataProcessing::UpdateCount4(int count)
{
    Count4 = count;
}

void DataProcessing::UpdateCount5(int count)
{
    Count5 = count;
}

void DataProcessing::UpdateCount6(int count)
{
    Count6 = count;
}

void DataProcessing::UpdateCount7(int count)
{
    Count7 = count;
}

void DataProcessing::UpdateCount8(int count)
{
    Count8 = count;
}

void DataProcessing::Finished2Threads()
{
    if (thread1->isFinished() && thread2->isFinished())
    {
        bar->setValue(bar->maximum());

        auto hard1 = thread1->GetHardRatings();
        auto soft1 = thread1->GetSoftRatings();

        auto hard2 = thread2->GetHardRatings();
        auto soft2 = thread2->GetSoftRatings();

        QVector<double> hardRatings(hard1.size(), 0);
        QVector<double> softRatings(soft1.size(), 0);


        for (int i=0; i < IO::ProjectsNames.size(); i++)
        {
            hardRatings[i] = hard1[i] + hard2[i];
            softRatings[i] = soft1[i] + soft2[i];
        }


        for (int i = 0; i < IO::ProjectsNames.size(); i++)
        {
            hardRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
            softRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
        }

        qDebug() << thread1->GetCount() << thread2->GetCount() << thread1->GetCount() + thread2->GetCount();
        qDebug() << hardRatings;
        qDebug() << softRatings;


        //###########BETA
        HardRatings = hardRatings;
        SoftRatings = softRatings;


        IO::FillingTables(inputTable, outputTable);

        Solution solution(
                    OpenedSolutionName, DataProcessing::CrushingStep,
                    IO::IndicatorsNames, IO::ProjectsNames,
                    IO::BaseTable, DataProcessing::NormalizedTable,
                    DataProcessing::HardRatings, DataProcessing::SoftRatings,
                    DataProcessing::PriorityList,
                    DataProcessing::PrefferedMetrics, DataProcessing::RejectedMetrics
                    );


        if (SolutionDB::IsContained(OpenedSolutionName))
            SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                       solution);
        else
            SolutionDB::AddSolution(solution);
    }


}
void DataProcessing::Finished4Threads()
{
    if (thread1->isFinished() && thread2->isFinished()
            && thread3->isFinished() && thread4->isFinished())
    {
        bar->setValue(bar->maximum());

        auto hard1 = thread1->GetHardRatings();
        auto soft1 = thread1->GetSoftRatings();

        auto hard2 = thread2->GetHardRatings();
        auto soft2 = thread2->GetSoftRatings();

        auto hard3 = thread3->GetHardRatings();
        auto soft3 = thread3->GetSoftRatings();

        auto hard4 = thread4->GetHardRatings();
        auto soft4 = thread4->GetSoftRatings();

        QVector<double> hardRatings(hard1.size(), 0);
        QVector<double> softRatings(soft1.size(), 0);


        for (int i=0; i < IO::ProjectsNames.size(); i++)
        {
            hardRatings[i] = hard1[i] + hard2[i] + hard3[i] + hard4[i];
            softRatings[i] = soft1[i] + soft2[i] + soft3[i] + soft4[i];
        }




        for (int i = 0; i < IO::ProjectsNames.size(); i++)
        {
            hardRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
            softRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
        }

        HardRatings = hardRatings;
        SoftRatings = softRatings;

        IO::FillingTables(inputTable, outputTable);

        Solution solution(
                    OpenedSolutionName, DataProcessing::CrushingStep,
                    IO::IndicatorsNames, IO::ProjectsNames,
                    IO::BaseTable, DataProcessing::NormalizedTable,
                    DataProcessing::HardRatings, DataProcessing::SoftRatings,
                    DataProcessing::PriorityList,
                    DataProcessing::PrefferedMetrics, DataProcessing::RejectedMetrics
                    );


        if (SolutionDB::IsContained(OpenedSolutionName))
            SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                       solution);
        else
            SolutionDB::AddSolution(solution);
    }

}
void DataProcessing::Finished8Threads()
{
    if (thread1->isFinished() && thread2->isFinished()
            && thread3->isFinished() && thread4->isFinished()
            && thread5->isFinished() && thread6->isFinished()
            && thread7->isFinished() && thread8->isFinished())
    {
        bar->setValue(bar->maximum());

        auto hard1 = thread1->GetHardRatings();
        auto soft1 = thread1->GetSoftRatings();

        auto hard2 = thread2->GetHardRatings();
        auto soft2 = thread2->GetSoftRatings();

        auto hard3 = thread3->GetHardRatings();
        auto soft3 = thread3->GetSoftRatings();

        auto hard4 = thread4->GetHardRatings();
        auto soft4 = thread4->GetSoftRatings();

        auto hard5 = thread5->GetHardRatings();
        auto soft5 = thread5->GetSoftRatings();

        auto hard6 = thread6->GetHardRatings();
        auto soft6 = thread6->GetSoftRatings();

        auto hard7 = thread7->GetHardRatings();
        auto soft7 = thread7->GetSoftRatings();

        auto hard8 = thread8->GetHardRatings();
        auto soft8 = thread8->GetSoftRatings();


        QVector<double> hardRatings(hard1.size(), 0);
        QVector<double> softRatings(soft1.size(), 0);


        for (int i=0; i < IO::ProjectsNames.size(); i++)
        {
            hardRatings[i] = hard1[i] + hard2[i] + hard3[i] + hard4[i]
                    + hard5[i] + hard6[i] + hard7[i] + hard8[i];

            softRatings[i] = soft1[i] + soft2[i] + soft3[i] + soft4[i]
                    + soft5[i] + soft6[i] + soft7[i] + soft8[i];
        }




        for (int i = 0; i < IO::ProjectsNames.size(); i++)
        {
            hardRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
            softRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
        }

        HardRatings = hardRatings;
        SoftRatings = softRatings;


        IO::FillingTables(inputTable, outputTable);

        Solution solution(
                    OpenedSolutionName, DataProcessing::CrushingStep,
                    IO::IndicatorsNames, IO::ProjectsNames,
                    IO::BaseTable, DataProcessing::NormalizedTable,
                    DataProcessing::HardRatings, DataProcessing::SoftRatings,
                    DataProcessing::PriorityList,
                    DataProcessing::PrefferedMetrics, DataProcessing::RejectedMetrics
                    );


        if (SolutionDB::IsContained(OpenedSolutionName))
            SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                       solution);
        else
            SolutionDB::AddSolution(solution);
    }
}

void DataProcessing::GenerateWeightsList()
{
    WeightsTable.clear();
    int weight_count = IO::IndicatorsNames.size();
    QVector<double> arr(weight_count, 0);

    GetNextNum(arr, weight_count, 0);
}

void DataProcessing::SetMetrics(QVector<int> Preferred, QVector<int> Rejected)
{
    PrefferedMetrics = Preferred;
    RejectedMetrics = Rejected;
}

void DataProcessing::CalculateRatings()
{
    bar->setValue(bar->maximum());

    bool isSuitable;
    QVector<double> hardRatings(IO::ProjectsNames.size());
    QVector<double> softRatings(IO::ProjectsNames.size());
    missed_variation = 0;

    qDebug() << hardRatings.size();

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




    for (int count = 0; count < WeightsTable.size(); count++)
    {
        auto res = GetLinearConvolutionResult(WeightsTable[count]);

        //Проверка метрик
        if ((PrefferedMetrics.size() != 0) && (RejectedMetrics.size() != 0))
        {

            isSuitable = true;
            for (int i = 0; i < PrefferedMetrics.size(); i++)
                for (int j = 0; j < RejectedMetrics.size(); j++)
                {
                    isSuitable = isSuitable &&
                            (WeightsTable[count][PrefferedMetrics[i]] > WeightsTable[count][RejectedMetrics[j]]);

                }
            if (!isSuitable)
            {
                missed_variation++;
                continue;
            }
        }


        //Мягкий рейтинг
        for (int i = 0; i < IO::ProjectsNames.size(); i++)
            softRatings[i] += res[i] / (1.0 - getMin(res));




        //Жёсткий рейтинг
        if (res.count(getMax(res))>1)
        {
            int q = res.count(getMax(res));
            for (int i = 0; i < res.size(); i++)
                hardRatings[i] += (res[i] == getMax(res)) ? 1.0/q : 0;
        }
        else
            hardRatings[res.indexOf(getMax(res))]++;


    }



    for (int i = 0; i < softRatings.size(); i++)
    {
        hardRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
        softRatings[i] *= 1.0 / (WeightsTable.size() - missed_variation);
    }



    HardRatings = hardRatings;
    SoftRatings = softRatings;

    IO::FillingTables(inputTable, outputTable);

    Solution solution(
                OpenedSolutionName, DataProcessing::CrushingStep,
                IO::IndicatorsNames, IO::ProjectsNames,
                IO::BaseTable, DataProcessing::NormalizedTable,
                DataProcessing::HardRatings, DataProcessing::SoftRatings,
                DataProcessing::PriorityList,
                DataProcessing::PrefferedMetrics, DataProcessing::RejectedMetrics
                );


    if (SolutionDB::IsContained(OpenedSolutionName))
        SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                   solution);
    else
        SolutionDB::AddSolution(solution);
}
void DataProcessing::CalculateRatingsIn2Threads()
{
    missed_variation = 0;
    ResetCounts();
    Count1 = 0;
    Count2 = 0;

    // DataProcessing* dp = new DataProcessing;

    thread1 = new CalculateRatingsAsync(0, WeightsTable.size()/2);
    thread2 = new CalculateRatingsAsync(WeightsTable.size()/2 + 1, WeightsTable.size()-1);


    connect(thread1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(thread1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));
    connect(thread2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount2(int)));


    connect(thread1, SIGNAL(finished()), instance, SLOT(Finished2Threads()));
    connect(thread2, SIGNAL(finished()), instance, SLOT(Finished2Threads()));

    thread1->start();
    thread2->start();
}
void DataProcessing::CalculateRatingsIn4Threads()
{
    missed_variation = 0;
    ResetCounts();
    Count1 = 0;
    Count2 = 0;
    Count3 = 0;
    Count4 = 0;


    int a1 = 0;
    int b1 = WeightsTable.size() / 4;

    int a2 = b1 + 1;
    int b2 =  WeightsTable.size() / 4 * 2;

    int a3 = b2 + 1;
    int b3 =  WeightsTable.size() / 4 * 3;

    int a4 = b3 + 1;
    int b4 = WeightsTable.size() - 1;


    thread1 = new CalculateRatingsAsync(a1, b1);
    thread2 = new CalculateRatingsAsync(a2, b2);
    thread3 = new CalculateRatingsAsync(a3, b3);
    thread4 = new CalculateRatingsAsync(a4, b4);


    connect(thread1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(thread1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));
    connect(thread2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount2(int)));
    connect(thread3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount3(int)));
    connect(thread4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount4(int)));


    connect(thread1, SIGNAL(finished()), instance, SLOT(Finished4Threads()));
    connect(thread2, SIGNAL(finished()), instance, SLOT(Finished4Threads()));
    connect(thread3, SIGNAL(finished()), instance, SLOT(Finished4Threads()));
    connect(thread4, SIGNAL(finished()), instance, SLOT(Finished4Threads()));


    thread1->start();
    thread2->start();
    thread3->start();
    thread4->start();

}
void DataProcessing::CalculateRatingsIn8Threads()
{
    missed_variation = 0;
    ResetCounts();
    Count1 = 0;
    Count2 = 0;
    Count3 = 0;
    Count4 = 0;
    Count5 = 0;
    Count6 = 0;
    Count7 = 0;
    Count8 = 0;

    int a1 = 0;
    int b1 = WeightsTable.size() / 8;

    int a2 = b1 + 1;
    int b2 =  WeightsTable.size() /8 * 2;

    int a3 = b2 + 1;
    int b3 =  WeightsTable.size() / 8 * 3;

    int a4 = b3 + 1;
    int b4 = WeightsTable.size() /8 * 4;

    int a5 = b4 + 1;
    int b5 = WeightsTable.size() /8 * 5;

    int a6 = b5 + 1;
    int b6 = WeightsTable.size() /8 * 6;

    int a7 = b6 + 1;
    int b7 = WeightsTable.size() /8 * 7;

    int a8 = b7 + 1;
    int b8 = WeightsTable.size() - 1;


    thread1 = new CalculateRatingsAsync(a1, b1);
    thread2 = new CalculateRatingsAsync(a2, b2);
    thread3 = new CalculateRatingsAsync(a3, b3);
    thread4 = new CalculateRatingsAsync(a4, b4);
    thread5 = new CalculateRatingsAsync(a5, b5);
    thread6 = new CalculateRatingsAsync(a6, b6);
    thread7 = new CalculateRatingsAsync(a7, b7);
    thread8 = new CalculateRatingsAsync(a8, b8);


    connect(thread1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread5, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread6, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread7, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(thread8, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(thread1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));
    connect(thread2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount2(int)));
    connect(thread3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount3(int)));
    connect(thread4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount4(int)));
    connect(thread5, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount5(int)));
    connect(thread6, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount6(int)));
    connect(thread7, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount7(int)));
    connect(thread8, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount8(int)));


    connect(thread1, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread2, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread3, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread4, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread5, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread6, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread7, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(thread8, SIGNAL(finished()), instance, SLOT(Finished8Threads()));


    thread1->start();
    thread2->start();
    thread3->start();
    thread4->start();
    thread5->start();
    thread6->start();
    thread7->start();
    thread8->start();

}


QVector<double> DataProcessing::GetLinearConvolutionResult(QVector<double> weights)
{
    QVector<double> results;
    double sum;
    for (int i = 0; i < NormalizedTable.size(); i++)
    {
        sum = 0;
        for (int j = 0; j < NormalizedTable[0].size(); j++)
        {
            sum += NormalizedTable[i][j] * weights[j];
        }

        results.append(sum);
    }

    return results;
}


void DataProcessing::ResetCounts()
{
    Count1 = -1;
    Count2 = -1;
    Count3 = -1;
    Count4 = -1;
    Count5 = -1;
    Count6 = -1;
    Count7 = -1;
    Count8 = -1;
}


void DataProcessing::MakeCalculations(QVector<QString> priorityList, QVector<int> Preferred, QVector<int> Rejected)
{
    CurrentIterationCount = 0;


    QVector<QString> priority = priorityList.size() == 0
            ?QVector<QString>(IO::IndicatorsNames.size(), "max")
           :priorityList;

    qDebug() << "\n-------------------------\n";
    SetMetrics(Preferred, Rejected);
    SetPriorityList(priority);
    FindMaxMinIndicators(IO::BaseTable);
    CalculateNormalizedTable(IO::BaseTable);

    GenerateWeightsList();
    qDebug() << "WeightList generated";




    const int AlgorithmComplexity = WeightsTable.size() * IO::ProjectsNames.size() * IO::IndicatorsNames.size();
    const int AvailableThreadCount = QThread::idealThreadCount();


    if ((AlgorithmComplexity < 500000) || (AvailableThreadCount == 1))
    {
        CalculateRatings();
        qDebug() << "#" << "1 thread" << "AlgorithmComplexity:"
                 << AlgorithmComplexity << "AvailableThreadCount:" << AvailableThreadCount;
    }


    if ((AlgorithmComplexity >= 500000 && AlgorithmComplexity < 1000000 && AvailableThreadCount >= 2)
            || (AlgorithmComplexity >= 1000000 && AvailableThreadCount == 2))
    {
        CalculateRatingsIn2Threads();
        qDebug() << "#" << "2 threads" << "AlgorithmComplexity:"
                 << AlgorithmComplexity << "AvailableThreadCount:" << AvailableThreadCount;
    }


    if ((AlgorithmComplexity >= 1000000 && AlgorithmComplexity < 2000000 && AvailableThreadCount >= 4)
            || (AlgorithmComplexity >= 2000000 && AvailableThreadCount == 4))
    {
        CalculateRatingsIn4Threads();
        qDebug() << "#" << "4 threads" << "AlgorithmComplexity:"
                 << AlgorithmComplexity << "AvailableThreadCount:" << AvailableThreadCount;
    }


    if ((AlgorithmComplexity >= 2000000 && AvailableThreadCount >= 8))
    {
        CalculateRatingsIn8Threads();
        qDebug() << "#" << "8 threads" << "AlgorithmComplexity:"
                 << AlgorithmComplexity << "AvailableThreadCount:" << AvailableThreadCount;
    }




    qDebug() << "#############";
    qDebug() << QString("%1 x %2 x %3 = %4").arg(WeightsTable.size())
                .arg(IO::ProjectsNames.size())
                .arg(IO::IndicatorsNames.size())
                .arg(WeightsTable.size()*IO::ProjectsNames.size()*IO::IndicatorsNames.size());
    qDebug() << "#############";
    qDebug() << "-------------------------\n";


    //    QFile file("D://log_sppr.txt");
    //    if (file.open(QIODevice::Append))
    //    {
    //        const QString ThreadCount = "1";
    //        //const QString ThreadCount = "2";
    //        //const QString ThreadCount = "4";

    //        QString record = QString("%1;%2;%3").arg(ThreadCount).arg(WeightsTable.size()).arg(timer.elapsed());
    //        file.write(record.toUtf8() + "\r\n");
    //        file.close();
    //    }

}
