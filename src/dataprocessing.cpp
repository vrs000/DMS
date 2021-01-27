#include "dataprocessing.h"
#include <QMessageBox>

int DataProcessing::UsedThreadCount = 0;
int DataProcessing::TimeElapsed = 0;
QString DataProcessing::timeElapsedParsed = "";

int DataProcessing::MaxThreadCount = QThread::idealThreadCount();
int DataProcessing::CurrentThreadCount = -1;

MainWindow* DataProcessing::mainWindow = nullptr;

QString DataProcessing::NotParsedImportanceGroupOfProjects = "";
QString DataProcessing::NotParsedImportanceGroupOfIndicators = "";


QVector<QString> DataProcessing::PriorityList;
QVector<double> DataProcessing::MaximumIndicators;
QVector<double> DataProcessing::MinimumIndicators;

double** DataProcessing::normTable = nullptr;

QVector<QVector<double>> DataProcessing::NormalizedTable;
QVector<QVector<double>> DataProcessing::WeightsTable;
QVector<QString> DataProcessing::ParettoSetProjects;
QList<int> DataProcessing::ParettoSetProjectsIndexes;

double DataProcessing::CrushingStep = 0.5;
int DataProcessing::missed_variation = 0;

QVector<double> DataProcessing::SoftRatings;
QVector<double> DataProcessing::HardRatings;

QVector<int> DataProcessing::PrefferedMetrics;
QVector<int> DataProcessing::RejectedMetrics;

QVector<QString> DataProcessing::PrefferedProjects;
QVector<QString> DataProcessing::RejectedProjects;

QProgressBar* DataProcessing::bar = nullptr;

int DataProcessing::CurrentIterationCount = 0;

QTime DataProcessing::time = QTime();



GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th1 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th2 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th3 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th4 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th5 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th6 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th7 = nullptr;
GenerateWeightsAndCalculateRatingsAsync* DataProcessing::th8 = nullptr;

QList<GenerateWeightsAndCalculateRatingsAsync*> DataProcessing::threadInstances;


int DataProcessing::Count1 = -1;
int DataProcessing::Count2 = -1;
int DataProcessing::Count3 = -1;
int DataProcessing::Count4 = -1;
int DataProcessing::Count5 = -1;
int DataProcessing::Count6 = -1;
int DataProcessing::Count7 = -1;
int DataProcessing::Count8 = -1;
int DataProcessing::TotalStepsCount = -1;


int DataProcessing::NumberOfTriggring = 0;


int DataProcessing::ProjectsCount = 0;
int DataProcessing::IndicatorsCount = 0;

int DataProcessing::Previous = 0;

QTableWidget* DataProcessing::inputTable = nullptr;
QTableWidget* DataProcessing::outputTable = nullptr;

QString DataProcessing::OpenedSolutionName = "";

DataProcessing* DataProcessing::instance = new DataProcessing;

QString DataProcessing::GetPassedTimeElapsed(int ms)
{
    QString time = "";

    int m, sec, min, hour;

    m = ms % 1000;
    sec = ms / 1000;

    min = sec / 60;
    sec = sec % 60;

    hour = min / 60;
    min = min % 60;

    if (hour > 9) time += QString::number(hour);
    else time += "0" + QString::number(hour);

    if (min > 9) time +=":" + QString::number(min);
    else time +=":0" + QString::number(min);

    if (sec > 9) time +=":" + QString::number(sec);
    else time +=":0" + QString::number(sec);

    if (m > 100) time +=":" + QString::number(m);
    else if (m > 9) time +=":0" + QString::number(m);
    else time +=":00" + QString::number(m);

    return time;
}

double DataProcessing::Fact(double value)
{
    double ans = 1;
    value = qRound(value);
    for (int i = 1; i <= value; ++i)
    {
        ans *= i;
    }
    return ans;
}

unsigned long DataProcessing::f(int n)
{
    unsigned  long res = 1;

    for (unsigned long i =1; i <= n; i++)
    {
        res *= i;
    }

    return res;
}

double DataProcessing::Factorial(double value)
{
    double ans = 1;

    value = qRound(value);

    for (int i = 1; i <= value; ++i)
    {
        ans *= i;
    }

    return ans;
}

int DataProcessing::GetTheoreticalWeightsCount(int ParametersCount, double h)
{
    return std::floor(Factorial(std::floor(1 / h) + ParametersCount - 1)
                      / (Factorial(ParametersCount - 1) * Factorial(std::floor(1 / h))));
}

void DataProcessing::FindParettoSet()
{
    ParettoSetProjects.clear();
    ParettoSetProjectsIndexes.clear();


    QList<int> WorstProjectIndexes;
    bool IsBetter;


    for (int i = 0; i < IO::ProjectsNames.size(); i++)
    {
        if (WorstProjectIndexes.contains(i)) continue;

        for (int j = 0; j < IO::ProjectsNames.size(); j++)
        {
            IsBetter = true;

            if (j == i) continue;
            if (WorstProjectIndexes.contains(j)) continue;

            for (int paramIndex = 0; paramIndex < IO::IndicatorsNames.size(); paramIndex++ )
            {
                IsBetter = IsBetter && (NormalizedTable[i][paramIndex] >= NormalizedTable[j][paramIndex]);
            }

            if (IsBetter)
                WorstProjectIndexes << j;
        }
    }


    for (int i = 0; i < IO::ProjectsNames.size(); i++)
        if (!WorstProjectIndexes.contains(i))
            ParettoSetProjects << IO::ProjectsNames[i];
}

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


    if (normTable != nullptr)
    {
        for (int i=0; i < Previous; i++)
            delete [] normTable[i];

        delete[] normTable;
    }


    normTable = new double*[Rows];
    for (int i = 0; i < Rows; i++)
        normTable[i] = new double[Columns];

    for (int i=0; i < Rows; i++)
        for (int j=0; j< Columns; j++)
            normTable[i][j]=0;

    Previous = Rows;


    for (int j = 0; j < Columns; ++j)
    {
        max = MaximumIndicators[j];
        min = MinimumIndicators[j];
        priority = PriorityList[j];

        for (int i = 0; i < Rows; i++)
        {
            //            normTable[i][j] = priority == "max" ?(BaseTable[i][j] - min)/(max-min) :  normTable[i][j];
            //            normTable[i][j] = priority == "min" ? 1 - (BaseTable[i][j] - min)/(max-min) :  normTable[i][j];

            if (priority == "max")
                normTable[i][j] = (BaseTable[i][j] - min)/(max-min);
            if (priority == "min")
                normTable[i][j] = 1-(BaseTable[i][j] - min)/(max-min);

            NormalizedTable[i][j] = priority == "max" ?(BaseTable[i][j] - min)/(max-min) :  NormalizedTable[i][j];
            NormalizedTable[i][j] = priority == "min" ? 1 - (BaseTable[i][j] - min)/(max-min) :  NormalizedTable[i][j];
        }
    }
}

void DataProcessing::SetPriorityList(QVector<QString> list)
{
    PriorityList = list;
}

QString DataProcessing::GetTheDigitsOfNumber(QString number, QString splitter)
{
    QStringList triads;
    int count = number.count() - 3;

    while (count >= 0)
    {
        triads.push_front(number.mid(count, 3));
        count -= 3;
    }

    if (count == -2)
        triads.push_front(number.mid(0, 1));

    if (count == -1)
        triads.push_front(number.mid(0, 2));


    return triads.join(splitter);
}


//void DataProcessing::GetNextNum(QVector<double>& currentSet, int maxN, int curPosIndex)
//{
//    if (curPosIndex < maxN - 1)
//    {
//        double lim = 1  - Sum(currentSet, curPosIndex);
//        for (double i = 0; i <= lim + 0.00001; i += CrushingStep)
//        {
//            currentSet[curPosIndex] = i;
//            GetNextNum(currentSet, maxN, curPosIndex + 1);
//        }
//    }
//    else
//        if (curPosIndex == maxN - 1)
//        {
//            double rest = 1 - Sum(currentSet, curPosIndex);
//            if (rest < 0.00001)
//                rest = 0;

//            if (fabs(static_cast<double>(rest - CrushingStep)) < 0.00001)
//                rest = CrushingStep;


//            currentSet[curPosIndex] = rest;

//            WeightsTable.append(currentSet);
//        }

//}

void DataProcessing::GetNextNum(double currentSet[], int maxN, int curPosIndex)
{
    if (curPosIndex < maxN - 1)
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

            QVector<double> s;
            for (int i=0; i<maxN; i++)
            {
                s << currentSet[i];
            }

            WeightsTable.append(s);

        }
}

void DataProcessing::SetMainWindowTitle(QString title)
{
    mainWindow->setWindowTitle(QString("Система поддержки принятия решений | %1   %2").arg(title).arg(timeElapsedParsed));

    double sum = 0;

    foreach (double el, HardRatings)
    {
        sum += el;
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
double DataProcessing::Sum(double set[], int elementsCount)
{
    double Sum = 0.0;

    for (int i = 0; i < elementsCount; ++i)
    {
        Sum += set[i];
    }

    return Sum;
}

void DataProcessing::DeleteThreadInstances()
{
    if (th1 != nullptr) delete th1;
    if (th2 != nullptr) delete th2;
    if (th3 != nullptr) delete th3;
    if (th4 != nullptr) delete th4;
    if (th5 != nullptr) delete th5;
    if (th6 != nullptr) delete th6;
    if (th7 != nullptr) delete th7;
    if (th8 != nullptr) delete th8;


    th1 = nullptr;
    th2 = nullptr;
    th3 = nullptr;
    th4 = nullptr;
    th5 = nullptr;
    th6 = nullptr;
    th7 = nullptr;
    th8 = nullptr;
}


void DataProcessing::UpdateProgressBar()
{
    if (bar->isHidden())
        bar->show();

    if (WeightsTable.size() == 0)
    {
        if (bar->maximum() != CurrentIterationCount)
            bar->setMaximum(CurrentIterationCount);
    }
    else
        if (bar->maximum() != WeightsTable.size())
            bar->setMaximum(WeightsTable.size());

    int res = 0;


    foreach (auto thread, threadInstances)
    {
        res += thread->GetCount();
    }

    //    if (Count1 != -1) res += Count1;
    //    if (Count2 != -1) res += Count2;
    //    if (Count3 != -1) res += Count3;
    //    if (Count4 != -1) res += Count4;
    //    if (Count5 != -1) res += Count5;
    //    if (Count6 != -1) res += Count6;
    //    if (Count7 != -1) res += Count7;
    //    if (Count8 != -1) res += Count8;


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

void DataProcessing::UpdateTotalStepCount(int count)
{

    TotalStepsCount++;
}

void DataProcessing::Finished1Threads()
{
    if (NumberOfTriggring == 0)
        if (th1->isFinished())
        {
            NumberOfTriggring++;

            TimeElapsed = time.elapsed();

            bar->setValue(bar->maximum());

            auto hard1 = th1->GetHardRatings();
            auto soft1 = th1->GetSoftRatings();


            QVector<double> hardRatings(hard1.size(), 0);
            QVector<double> softRatings(soft1.size(), 0);


            for (int i=0; i < ProjectsCount; i++)
            {
                hardRatings[i] = hard1[i];
                softRatings[i] = soft1[i];
            }

            int WeightCount = th1->GetCount();
            int MissedCount = th1->missed;

            double count = Sum(hardRatings, hardRatings.size());

            int iter = WeightCount - MissedCount;


            for (int i = 0; i < ProjectsCount; i++)
            {
                hardRatings[i] *= 1.0 / count;
                softRatings[i] *= 1.0 / count;
            }

            missed_variation = MissedCount;

            HardRatings = hardRatings;
            SoftRatings = softRatings;

            IO::FillingTables(inputTable, outputTable);

            timeElapsedParsed = GetPassedTimeElapsed(TimeElapsed);

            Solution solution(
                        OpenedSolutionName, CrushingStep,
                        IO::IndicatorsNames, IO::ProjectsNames,
                        IO::BaseTable, NormalizedTable,
                        HardRatings, SoftRatings,
                        PriorityList,
                        NotParsedImportanceGroupOfProjects, NotParsedImportanceGroupOfIndicators,
                        timeElapsedParsed,
                        ParettoSetProjects);

            NormalizedTable.clear();

            if (SolutionDB::IsContained(OpenedSolutionName))
                SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                           solution);
            else
                SolutionDB::AddSolution(solution);

            DeleteThreadInstances();
            SetMainWindowTitle(SolutionDB::currentSolutionName);
        }
}
void DataProcessing::Finished2Threads()
{
    if (NumberOfTriggring == 0)
        if (th1->isFinished() && th2->isFinished())
        {
            NumberOfTriggring++;

            TimeElapsed = time.elapsed();


            bar->setValue(bar->maximum());

            auto hard1 = th1->GetHardRatings();
            auto soft1 = th1->GetSoftRatings();

            auto hard2 = th2->GetHardRatings();
            auto soft2 = th2->GetSoftRatings();


            QVector<double> hardRatings(hard1.size(), 0);
            QVector<double> softRatings(soft1.size(), 0);


            for (int i = 0; i < ProjectsCount; i++)
            {
                hardRatings[i] = hard1[i] + hard2[i];
                softRatings[i] = soft1[i] + soft2[i];
            }



            double count = Sum(hardRatings, hardRatings.size());



            for (int i = 0; i < ProjectsCount; i++)
            {
                hardRatings[i] *= 1.0 / count;
                softRatings[i] *= 1.0 / count;
            }


            HardRatings = hardRatings;
            SoftRatings = softRatings;

            IO::FillingTables(inputTable, outputTable);

            timeElapsedParsed = GetPassedTimeElapsed(TimeElapsed);

            Solution solution(
                        OpenedSolutionName, CrushingStep,
                        IO::IndicatorsNames, IO::ProjectsNames,
                        IO::BaseTable, NormalizedTable,
                        HardRatings, SoftRatings,
                        PriorityList,
                        NotParsedImportanceGroupOfProjects, NotParsedImportanceGroupOfIndicators,
                        timeElapsedParsed,
                        ParettoSetProjects);
            NormalizedTable.clear();
            if (SolutionDB::IsContained(OpenedSolutionName))
                SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                           solution);
            else
                SolutionDB::AddSolution(solution);


            DeleteThreadInstances();
            SetMainWindowTitle(SolutionDB::currentSolutionName);
        }
}
void DataProcessing::Finished4Threads()
{

    if (NumberOfTriggring == 0)
        if (th1->isFinished() && th2->isFinished()
                && th3->isFinished() && th4->isFinished())
        {
            NumberOfTriggring++;

            TimeElapsed = time.elapsed();


            bar->setValue(bar->maximum());

            auto hard1 = th1->GetHardRatings();
            auto soft1 = th1->GetSoftRatings();

            auto hard2 = th2->GetHardRatings();
            auto soft2 = th2->GetSoftRatings();

            auto hard3 = th3->GetHardRatings();
            auto soft3 = th3->GetSoftRatings();

            auto hard4 = th4->GetHardRatings();
            auto soft4 = th4->GetSoftRatings();


            QVector<double> hardRatings(hard1.size(), 0);
            QVector<double> softRatings(soft1.size(), 0);


            for (int i=0; i < ProjectsCount; i++)
            {
                hardRatings[i] = hard1[i] + hard2[i] + hard3[i] + hard4[i];
                softRatings[i] = soft1[i] + soft2[i] + soft3[i] + soft4[i];
            }

            int WeightCount = th1->GetCount() + th2->GetCount()
                    + th3->GetCount() + th4->GetCount();

            int MissedCount = th1->missed + th2->missed
                    + th3->missed + th4->missed;


            double count = Sum(hardRatings, hardRatings.size());


            for (int i = 0; i < ProjectsCount; i++)
            {
                hardRatings[i] *= 1.0 / count;
                softRatings[i] *= 1.0 / count;
            }

            missed_variation = MissedCount;

            HardRatings = hardRatings;
            SoftRatings = softRatings;

            IO::FillingTables(inputTable, outputTable);

            timeElapsedParsed = GetPassedTimeElapsed(TimeElapsed);

            Solution solution(
                        OpenedSolutionName, CrushingStep,
                        IO::IndicatorsNames, IO::ProjectsNames,
                        IO::BaseTable, NormalizedTable,
                        HardRatings, SoftRatings,
                        PriorityList,
                        NotParsedImportanceGroupOfProjects, NotParsedImportanceGroupOfIndicators,
                        timeElapsedParsed,
                        ParettoSetProjects);
            NormalizedTable.clear();
            if (SolutionDB::IsContained(OpenedSolutionName))
                SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                           solution);
            else
                SolutionDB::AddSolution(solution);


            DeleteThreadInstances();
            SetMainWindowTitle(SolutionDB::currentSolutionName);
        }
}
void DataProcessing::Finished8Threads()
{
    if (NumberOfTriggring == 0)
        if (th1->isFinished() && th2->isFinished()
                && th3->isFinished() && th4->isFinished()
                && th5->isFinished() && th6->isFinished()
                && th7->isFinished() && th8->isFinished())
        {
            NumberOfTriggring++;

            TimeElapsed = time.elapsed();


            bar->setValue(bar->maximum());

            auto hard1 = th1->GetHardRatings();
            auto soft1 = th1->GetSoftRatings();

            auto hard2 = th2->GetHardRatings();
            auto soft2 = th2->GetSoftRatings();

            auto hard3 = th3->GetHardRatings();
            auto soft3 = th3->GetSoftRatings();

            auto hard4 = th4->GetHardRatings();
            auto soft4 = th4->GetSoftRatings();

            auto hard5 = th5->GetHardRatings();
            auto soft5 = th5->GetSoftRatings();

            auto hard6 = th6->GetHardRatings();
            auto soft6 = th6->GetSoftRatings();

            auto hard7 = th7->GetHardRatings();
            auto soft7 = th7->GetSoftRatings();

            auto hard8 = th8->GetHardRatings();
            auto soft8 = th8->GetSoftRatings();


            QVector<double> hardRatings(hard1.size(), 0);
            QVector<double> softRatings(soft1.size(), 0);


            for (int i=0; i < ProjectsCount; i++)
            {
                hardRatings[i] = hard1[i] + hard2[i] + hard3[i] + hard4[i]
                        + hard5[i] + hard6[i] + hard7[i] + hard8[i];

                softRatings[i] = soft1[i] + soft2[i] + soft3[i] + soft4[i]
                        + soft5[i] + soft6[i] + soft7[i] + soft8[i];
            }


            int WeightCount = th1->GetCount() + th2->GetCount()
                    + th3->GetCount() + th4->GetCount()
                    + th5->GetCount() + th6->GetCount()
                    + th7->GetCount() + th8->GetCount();


            int MissedCount = th1->missed + th2->missed
                    + th3->missed + th4->missed
                    + th5->missed + th6->missed
                    + th7->missed + th8->missed;


            int sum=0;
            foreach (double i, hardRatings)
                sum += i;



            double count = Sum(hardRatings, hardRatings.size());


            for (int i = 0; i < ProjectsCount; i++)
            {
                hardRatings[i] *= 1.0 / count;
                softRatings[i] *= 1.0 / count;
            }

            missed_variation = MissedCount;

            HardRatings = hardRatings;
            SoftRatings = softRatings;

            IO::FillingTables(inputTable, outputTable);

            timeElapsedParsed = GetPassedTimeElapsed(TimeElapsed);

            Solution solution(
                        OpenedSolutionName, CrushingStep,
                        IO::IndicatorsNames, IO::ProjectsNames,
                        IO::BaseTable, NormalizedTable,
                        HardRatings, SoftRatings,
                        PriorityList,
                        NotParsedImportanceGroupOfProjects, NotParsedImportanceGroupOfIndicators,
                        timeElapsedParsed,
                        ParettoSetProjects);

            NormalizedTable.clear();
            if (SolutionDB::IsContained(OpenedSolutionName))
                SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                           solution);
            else
                SolutionDB::AddSolution(solution);

            DeleteThreadInstances();
            SetMainWindowTitle(SolutionDB::currentSolutionName);
        }
}

void DataProcessing::FinishedAllThreads()
{
    bool isAllFinished = true;

    foreach (auto thread, threadInstances)
    {
        isAllFinished = isAllFinished && thread->isFinished();
    }

    if ((NumberOfTriggring == 0) && isAllFinished)
    {
        NumberOfTriggring++;
        TimeElapsed = time.elapsed();

        bar->setValue(bar->maximum());


        QVector<double> hardRatings(threadInstances.first()->GetHardRatings().size(), 0);
        QVector<double> softRatings(threadInstances.first()->GetSoftRatings().size(), 0);

        QVector<double> partOfHardRating;
        QVector<double> partOfSoftRating;

        int MissedCount = 0;

        foreach (auto thread, threadInstances)
        {
            MissedCount += thread->missed;

            partOfHardRating = thread->GetHardRatings();
            partOfSoftRating = thread->GetSoftRatings();

            for (int i=0; i < ProjectsCount; i++)
            {
                hardRatings[i] += partOfHardRating[i];
                softRatings[i] += partOfSoftRating[i];
            }
        }

        double count = Sum(hardRatings, hardRatings.size());

        for (int i = 0; i < ProjectsCount; i++)
        {
            hardRatings[i] *= 1.0 / count;
            softRatings[i] *= 1.0 / count;
        }

        missed_variation = MissedCount;

        HardRatings = hardRatings;
        SoftRatings = softRatings;

        IO::FillingTables(inputTable, outputTable);

        timeElapsedParsed = GetPassedTimeElapsed(TimeElapsed);

        Solution solution(
                    OpenedSolutionName, CrushingStep,
                    IO::IndicatorsNames, IO::ProjectsNames,
                    IO::BaseTable, NormalizedTable,
                    HardRatings, SoftRatings,
                    PriorityList,
                    NotParsedImportanceGroupOfProjects, NotParsedImportanceGroupOfIndicators,
                    timeElapsedParsed,
                    ParettoSetProjects);

        NormalizedTable.clear();
        if (SolutionDB::IsContained(OpenedSolutionName))
            SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                       solution);
        else
            SolutionDB::AddSolution(solution);

        DeleteThreadInstances();
        SetMainWindowTitle(SolutionDB::currentSolutionName);
    }

}


//CHANGED!!!!!!!!!!!!!!!!!
void DataProcessing::GenerateWeightsList()
{
    WeightsTable.clear();
    int weight_count = IO::IndicatorsNames.size();
    QVector<double> arr(weight_count, 0);

    double* set = new double[weight_count];
    for (int i=0; i<weight_count; i++)
        set[i] = 0;


    //    GetNextNum(arr, weight_count, 0);
    GetNextNum(set, weight_count, 0);
}

void DataProcessing::SetMetrics(QVector<int> Preferred, QVector<int> Rejected)
{
    PrefferedMetrics = Preferred;
    RejectedMetrics = Rejected;
}

void DataProcessing::SetProjectsPriorities(QVector<QString> Preferred, QVector<QString> Rejected)
{
    PrefferedProjects = Preferred;
    RejectedProjects = Rejected;
}


void DataProcessing::CalculateRatingsIn1ThreadsWithWeights()
{
    UsedThreadCount = 1;
    missed_variation = 0;

    ResetCounts();

    Count1 = 0;



    CurrentIterationCount = GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), CrushingStep);


    th1 = new GenerateWeightsAndCalculateRatingsAsync(1, CurrentIterationCount, DataProcessing::CrushingStep);



    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));


    connect(th1, SIGNAL(finished()), instance, SLOT(Finished1Threads()));


    th1->start();
}
void DataProcessing::CalculateRatingsIn2ThreadsWithWeights()
{
    UsedThreadCount = 2;
    missed_variation = 0;

    ResetCounts();

    Count1 = 0;
    Count2 = 0;


    CurrentIterationCount = GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), CrushingStep);


    th1 = new GenerateWeightsAndCalculateRatingsAsync(1, CurrentIterationCount/2, DataProcessing::CrushingStep);
    th2 = new GenerateWeightsAndCalculateRatingsAsync(CurrentIterationCount/2 + 1, CurrentIterationCount, DataProcessing::CrushingStep);


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));
    connect(th2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount2(int)));


    connect(th1, SIGNAL(finished()), instance, SLOT(Finished2Threads()));
    connect(th2, SIGNAL(finished()), instance, SLOT(Finished2Threads()));


    th1->start();
    th2->start();
}
void DataProcessing::CalculateRatingsIn4ThreadsWithWeights()
{

    UsedThreadCount = 4;
    missed_variation = 0;

    ResetCounts();

    Count1 = 0;
    Count2 = 0;
    Count3 = 0;
    Count4 = 0;

    CurrentIterationCount = GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), CrushingStep);

    int k = CurrentIterationCount;


    int a1 = 1;
    int b1 = k / 4;

    int a2 = b1 + 1;
    int b2 =  k / 4 * 2;

    int a3 = b2 + 1;
    int b3 =  k / 4 * 3;

    int a4 = b3 + 1;
    int b4 = k;


    th1 = new GenerateWeightsAndCalculateRatingsAsync(a1, b1, DataProcessing::CrushingStep);
    th2 = new GenerateWeightsAndCalculateRatingsAsync(a2, b2, DataProcessing::CrushingStep);
    th3 = new GenerateWeightsAndCalculateRatingsAsync(a3, b3, DataProcessing::CrushingStep);
    th4 = new GenerateWeightsAndCalculateRatingsAsync(a4, b4, DataProcessing::CrushingStep);


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));
    connect(th2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount2(int)));
    connect(th3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount3(int)));
    connect(th4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount4(int)));


    connect(th1, SIGNAL(finished()), instance, SLOT(Finished4Threads()));
    connect(th2, SIGNAL(finished()), instance, SLOT(Finished4Threads()));
    connect(th3, SIGNAL(finished()), instance, SLOT(Finished4Threads()));
    connect(th4, SIGNAL(finished()), instance, SLOT(Finished4Threads()));


    th1->start();
    th2->start();
    th3->start();
    th4->start();
}
void DataProcessing::CalculateRatingsIn8ThreadsWithWeights()
{
    UsedThreadCount = 8;
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

    CurrentIterationCount = GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), CrushingStep);

    int k = CurrentIterationCount;

    int a1 = 0;
    int b1 = k / 8;

    int a2 = b1 + 1;
    int b2 = k /8 * 2;

    int a3 = b2 + 1;
    int b3 = k / 8 * 3;

    int a4 = b3 + 1;
    int b4 = k /8 * 4;

    int a5 = b4 + 1;
    int b5 = k/8 * 5;

    int a6 = b5 + 1;
    int b6 = k/8 * 6;

    int a7 = b6 + 1;
    int b7 = k /8 * 7;

    int a8 = b7 + 1;
    int b8 = k;


    th1 = new GenerateWeightsAndCalculateRatingsAsync(a1, b1, DataProcessing::CrushingStep);
    th2 = new GenerateWeightsAndCalculateRatingsAsync(a2, b2, DataProcessing::CrushingStep);
    th3 = new GenerateWeightsAndCalculateRatingsAsync(a3, b3, DataProcessing::CrushingStep);
    th4 = new GenerateWeightsAndCalculateRatingsAsync(a4, b4, DataProcessing::CrushingStep);
    th5 = new GenerateWeightsAndCalculateRatingsAsync(a5, b5, DataProcessing::CrushingStep);
    th6 = new GenerateWeightsAndCalculateRatingsAsync(a6, b6, DataProcessing::CrushingStep);
    th7 = new GenerateWeightsAndCalculateRatingsAsync(a7, b7, DataProcessing::CrushingStep);
    th8 = new GenerateWeightsAndCalculateRatingsAsync(a8, b8, DataProcessing::CrushingStep);


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th5, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th6, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th7, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
    connect(th8, SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));


    connect(th1, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount1(int)));
    connect(th2, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount2(int)));
    connect(th3, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount3(int)));
    connect(th4, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount4(int)));
    connect(th5, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount5(int)));
    connect(th6, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount6(int)));
    connect(th7, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount7(int)));
    connect(th8, SIGNAL(CountChanged(int)), instance, SLOT(UpdateCount8(int)));


    connect(th1, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th2, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th3, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th4, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th5, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th6, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th7, SIGNAL(finished()), instance, SLOT(Finished8Threads()));
    connect(th8, SIGNAL(finished()), instance, SLOT(Finished8Threads()));


    th1->start();
    th2->start();
    th3->start();
    th4->start();
    th5->start();
    th6->start();
    th7->start();
    th8->start();
}

QList<GenerateWeightsAndCalculateRatingsAsync*> DataProcessing::getThreadsList(int ThreadCount, int MaxThreadCount)
{
    QList<GenerateWeightsAndCalculateRatingsAsync*> threads;

    auto setListOfThreads = [&](int ThreadsCount){
        UsedThreadCount = ThreadsCount;

        int step = CurrentIterationCount/ThreadsCount;
        int a = 1;
        //        int b = a + step;
        int b = ThreadsCount==1 ? CurrentIterationCount : a+step;

        for (int i=0; i<ThreadsCount; i++)
        {
            threads <<new GenerateWeightsAndCalculateRatingsAsync(a,b, CrushingStep);

            connect(threads.last(), SIGNAL(CountChanged(int)), instance, SLOT(UpdateProgressBar()));
            connect(threads.last(), SIGNAL(CountChanged(int)), instance, SLOT(UpdateTotalStepCount(int)));
            connect(threads.last(), SIGNAL(finished()), instance, SLOT(FinishedAllThreads()));

            a = b + 1;
            //            b = b + step <= CurrentIterationCount ? b + step : CurrentIterationCount;

            //            if (i==ThreadsCount-2)
            //                b=CurrentIterationCount;

            b = i==ThreadsCount-1 ? CurrentIterationCount : b+step;


        }
    };

    //Если не задано количество потоков в которых нужно призвести вычисления
    if (ThreadCount == -1)
    {
        const unsigned int AlgorithmComplexity =  CurrentIterationCount * IO::ProjectsNames.size() * IO::IndicatorsNames.size();
        const int AvailableThreadCount = QThread::idealThreadCount();


        if ((AlgorithmComplexity < 500000) || (AvailableThreadCount == 1))
        {
            setListOfThreads(1);
            return threads;
        }


        if ((AlgorithmComplexity >= 500000 && AlgorithmComplexity < 1000000 && AvailableThreadCount >= 2)
                || (AlgorithmComplexity >= 1000000 && AvailableThreadCount == 2))
        {
            setListOfThreads(2);
            return threads;

        }


        if ((AlgorithmComplexity >= 1000000 && AlgorithmComplexity < 2000000 && AvailableThreadCount >= 4)
                || (AlgorithmComplexity >= 2000000 && AvailableThreadCount == 4))
        {
            setListOfThreads(4);
            return threads;

        }


        if ((AlgorithmComplexity >= 2000000 && AvailableThreadCount >= 8))
        {
            setListOfThreads(MaxThreadCount);
            return threads;
        }
    }

    if (ThreadCount != -1)
        setListOfThreads(ThreadCount);

    return threads;
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

double* DataProcessing::GetLinearConvolutionResult(double *weights)
{    
    double* results = new double[ProjectsCount];
    double sum;

    for (int i = 0; i < ProjectsCount; i++)
    {
        sum = 0;

        for (int j = 0; j < IndicatorsCount; j++)
            sum += normTable[ParettoSetProjects.size() == 0 ? i : ParettoSetProjectsIndexes[i]][j] * weights[j];

        results[i] = sum;
    }

    return results;
}


void DataProcessing::ResetCounts()
{
    TotalStepsCount = -1;

    Count1 = -1;
    Count2 = -1;
    Count3 = -1;
    Count4 = -1;
    Count5 = -1;
    Count6 = -1;
    Count7 = -1;
    Count8 = -1;
}


void DataProcessing::MakeCalculations(QVector<QString> priorityList,
                                      QVector<int> Preferred, QVector<int> Rejected,
                                      QVector<QString> preferredProjects, QVector<QString> rejectedProjects)
{
    CurrentIterationCount = 0;
    NumberOfTriggring = 0;

    ProjectsCount = ParettoSetProjects.size() == 0 ? IO::ProjectsNames.size() : ParettoSetProjects.size();
    IndicatorsCount = IO::IndicatorsNames.size();

    QVector<QString> priority = priorityList.size() == 0
            ?QVector<QString>(IO::IndicatorsNames.size(), "max")
           :priorityList;


    SetMetrics(Preferred, Rejected);
    SetProjectsPriorities(preferredProjects, rejectedProjects);
    SetPriorityList(priority);


    if (NormalizedTable.size() == 0)
    {
        FindMaxMinIndicators(IO::BaseTable);
        CalculateNormalizedTable(IO::BaseTable);
    }

    WeightsTable.clear();
    DataProcessing::CurrentIterationCount = GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), CrushingStep);

    time.start();

    threadInstances = getThreadsList(CurrentThreadCount, MaxThreadCount);

    for (int i=0; i<threadInstances.size(); i++)
        threadInstances[i]->start();


//        CalculateRatingsIn1ThreadsWithWeights();
//        CalculateRatingsIn2ThreadsWithWeights();
//        CalculateRatingsIn4ThreadsWithWeights();
//        CalculateRatingsIn8ThreadsWithWeights();
}
