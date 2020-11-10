#include "asynccalculation.h"

double GenerateWeightsAndCalculateRatingsAsync::sum(double set[], int index)
{
    double Sum = 0.0;

    for (int i = 0; i < index; ++i)
    {
        Sum += set[i];
    }

    return Sum;
}

double GenerateWeightsAndCalculateRatingsAsync::getMax(double *list)
{
    double max = list[0];

    for (int i = 0; i < ProjectsCount; i++)
        max = list[i] > max ? list[i] : max;

    return max;
}
double GenerateWeightsAndCalculateRatingsAsync::getMin(double *list)
{
    double min = list[0];

    for (int i = 0; i < ProjectsCount; i++)
        min = list[i] < min ? list[i] : min;

    return min;
}
int GenerateWeightsAndCalculateRatingsAsync::COUNT(double *list, double value)
{
    int k = 0;

    for (int i = 0; i < ProjectsCount; i++)
        if (list[i] == value) k++;

    return k;
}
int GenerateWeightsAndCalculateRatingsAsync::IndexOf(double *list, double value)
{
    for (int i = 0; i < ProjectsCount; i++)
        if (list[i] == value) return i;

    return -1;
}

GenerateWeightsAndCalculateRatingsAsync::GenerateWeightsAndCalculateRatingsAsync(int firstIterationIndex, int lastIterationIndex, double step)
{
    FirstIterationIndex = firstIterationIndex;
    LastIterationIndex = lastIterationIndex;


    HardRatings = QVector<double>(IO::ProjectsNames.size(), 0);
    SoftRatings = QVector<double>(IO::ProjectsNames.size(), 0);

    h = step;

    hardRatings = QVector<double>(IO::ProjectsNames.size(), 0);
    softRatings = QVector<double>(IO::ProjectsNames.size(), 0);

    ProjectsCount = DataProcessing::ProjectsCount;


    //Парсинг строчек групп важности для проектов и для показателей если имеется
    //==========================================================================
    auto IndicatorsGroups = DataProcessing::NotParsedImportanceGroupOfIndicators.split(',');
    auto ProjectsGroups = DataProcessing::NotParsedImportanceGroupOfProjects.split(',');


    ProjectsGroupCount = ProjectsGroups.size();
    IndicatorsGroupCount = IndicatorsGroups.size();



    //Показатели
    //---------------------------------------------------------------
    PrefferedMetricsV2 = new int[IndicatorsGroupCount];
    RejectedMetricsV2 = new int[IndicatorsGroupCount];

    for (int i=0; i < IndicatorsGroupCount; i++)
    {
        if (IndicatorsGroups[i].contains(">"))
        {
            auto numbs = IndicatorsGroups[i].split(">");

            PrefferedMetricsV2[i] = numbs[0].toInt();
            RejectedMetricsV2[i] = numbs[1].toInt();

            continue;
        }


        if (IndicatorsGroups[i].contains("<"))
        {
            auto numbs = IndicatorsGroups[i].split("<");

            RejectedMetricsV2[i] = numbs[0].toInt();
            PrefferedMetricsV2[i] = numbs[1].toInt();

            continue;
        }
    }

    //---------------------------------------------------------------


    //Проекты
    //---------------------------------------------------------------

    PrefferedProjectsV2 = new int[ProjectsGroupCount];
    RejectedProjectsV2 = new int[ProjectsGroupCount];

    for (int i=0; i < ProjectsGroupCount; i++)
    {
        if (ProjectsGroups[i].contains(">"))
        {
            auto numbs = ProjectsGroups[i].split(">");

            PrefferedProjectsV2[i] = numbs[0].toInt();
            RejectedProjectsV2[i] = numbs[1].toInt();

            continue;
        }


        if (ProjectsGroups[i].contains("<"))
        {
            auto numbs = ProjectsGroups[i].split("<");

            RejectedProjectsV2[i] = numbs[0].toInt();
            PrefferedProjectsV2[i] = numbs[1].toInt();

            continue;
        }
    }

    //---------------------------------------------------------------


    if (DataProcessing::NotParsedImportanceGroupOfIndicators == "") IndicatorsGroupCount = -1;
    if (DataProcessing::NotParsedImportanceGroupOfProjects == "") ProjectsGroupCount = -1;
    //==========================================================================





    //################################################################################
    //################################################################################
    //################################################################################

    //legacy
    //Установка количества показателей групп важности
    //----------------------------------------------------------------
    PrefferedMetricsCount = DataProcessing::PrefferedMetrics.size();
    RejectedMetricsCount = DataProcessing::RejectedMetrics.size();

    PrefferedProjectsCount = DataProcessing::PrefferedProjects.size();
    RejectedProjectsCount = DataProcessing::RejectedProjects.size();
    //----------------------------------------------------------------


    //legacy
    //Перевод QVector в double* для скорости
    //----------------------------------------------------------------
    PrefferedMetrics = new int[PrefferedMetricsCount];
    RejectedMetrics = new int[RejectedMetricsCount];

    for (int i=0; i<PrefferedMetricsCount; i++)
        PrefferedMetrics[i] = DataProcessing::PrefferedMetrics[i];

    for (int i=0; i<RejectedMetricsCount; i++)
        RejectedMetrics[i] = DataProcessing::RejectedMetrics[i];
    //----------------------------------------------------------------


    //legacy
    //Перевод QVector в double* для скорости
    //----------------------------------------------------------------
    PrefferedProjects = new int[PrefferedProjectsCount];
    RejectedProjects = new int[RejectedProjectsCount];

    for (int i=0; i<PrefferedProjectsCount; i++)
        PrefferedProjects[i] = IO::ProjectsNames.indexOf(DataProcessing::PrefferedProjects[i]);

    for (int i=0; i<RejectedProjectsCount; i++)
        RejectedProjects[i] = IO::ProjectsNames.indexOf(DataProcessing::RejectedProjects[i]);
    //----------------------------------------------------------------

    //################################################################################
    //################################################################################
    //################################################################################
}

GenerateWeightsAndCalculateRatingsAsync::~GenerateWeightsAndCalculateRatingsAsync()
{
    delete [] PrefferedMetrics;
    delete [] RejectedMetrics;
    delete [] PrefferedProjects;
    delete [] RejectedProjects;

    delete [] PrefferedMetricsV2;
    delete [] RejectedMetricsV2;
    delete [] PrefferedProjectsV2;
    delete [] RejectedProjectsV2;
}

QVector<double> GenerateWeightsAndCalculateRatingsAsync::GetHardRatings()
{
    return HardRatings;
}

QVector<double> GenerateWeightsAndCalculateRatingsAsync::GetSoftRatings()
{
    return SoftRatings;
}

int GenerateWeightsAndCalculateRatingsAsync::GetCount()
{
    return Count;
}

void GenerateWeightsAndCalculateRatingsAsync::Calculate(QVector<double>& currentSet, int maxN, int curPosIndex)
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


            GeneratedIterCount++;

            if (GeneratedIterCount > LastIterationIndex) IsNeedToFinish = true;

            if(GeneratedIterCount < FirstIterationIndex || GeneratedIterCount > LastIterationIndex)
                return;

            Count++;

            emit CountChanged(Count);


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




            auto res = DataProcessing::GetLinearConvolutionResult(currentSet);

            //Проверка метрик
            if ((DataProcessing::PrefferedMetrics.size() != 0) && (DataProcessing::RejectedMetrics.size() != 0))
            {
                isSuitable = true;
                for (int i = 0; i < DataProcessing::PrefferedMetrics.size(); i++)
                    for (int j = 0; j < DataProcessing::RejectedMetrics.size(); j++)
                    {
                        isSuitable = isSuitable &&
                                (currentSet[DataProcessing::PrefferedMetrics[i]] > currentSet[DataProcessing::RejectedMetrics[j]]);

                    }

                if (!isSuitable)
                {
                    DataProcessing::missed_variation++;
                    return;
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




            for (int i = 0; i < hardRatings.size(); i++)
            {
                HardRatings[i] += hardRatings[i];
                SoftRatings[i] += softRatings[i];
            }


        }
}

void GenerateWeightsAndCalculateRatingsAsync::Calculate(double currentSet[], int maxN, int curPosIndex)
{
    if((curPosIndex < maxN - 1))
    {

        double lim = 1 - sum(currentSet, curPosIndex);



        for(double i = 0; i <= lim + 0.00001; i += h)
        {
            currentSet[curPosIndex] = i;
            Calculate(currentSet, maxN, curPosIndex + 1);
        }
    }
    else
        if(curPosIndex == maxN - 1)
        {
            double rest = 1 - sum(currentSet, curPosIndex);
            if (rest - 0.00001 < 0)
                rest = 0;

            if (fabs(static_cast<double>(rest - h)) < 0.00001)
                rest = h;

            currentSet[curPosIndex] = rest;


            GeneratedIterCount++;



            if(GeneratedIterCount < FirstIterationIndex || GeneratedIterCount > LastIterationIndex)
            {
//                qDebug() << GeneratedIterCount << QThread::currentThreadId();

                return;
            }


            Count++;

            if (Count % 10000 == 0)
                emit CountChanged(Count);







            double* res = DataProcessing::GetLinearConvolutionResult(currentSet);


            bool IsSuitable = true;


            //Проверка проектов
            //---------------------------------------
            if (ProjectsGroupCount != -1)
            {
                for (int i = 0; i < ProjectsGroupCount; i++)
                {
                    if (!(res[PrefferedProjectsV2[i]] > res[RejectedProjectsV2[i]]))
                    {
                        DataProcessing::missed_variation++;
                        missed++;
                        IsSuitable = false;
                    }
                }

            }
            //---------------------------------------


            //Проверка индикаторов
            //---------------------------------------
            if (IndicatorsGroupCount != -1)
            {
                for (int i = 0; i < IndicatorsGroupCount; i++)
                {
                    if (!(currentSet[PrefferedMetricsV2[i]] > currentSet[RejectedMetricsV2[i]]))
                    {
                        DataProcessing::missed_variation++;
                        missed++;
                        IsSuitable = false;
                    }
                }
            }
            //---------------------------------------




            if (IsSuitable)
            {



                //Мягкий рейтинг
                for (int i = 0; i < ProjectsCount; i++)
                {

                    softRatings[i] = res[i] / (getMax(res));
                    hardRatings[i] = 0;
                }


                //Жёсткий рейтинг
                if (COUNT(res, getMax(res)) > 1)
                {
                    int q = COUNT(res, getMax(res));
                    for (int i = 0; i < ProjectsCount; i++)
                        hardRatings[i] = (res[i] == getMax(res)) ? 1.0/q : 0;
                }
                else
                {
                    hardRatings[IndexOf(res, getMax(res))] = 1;
                }


                //Занесение результата
                for (int i = 0; i < ProjectsCount; i++)
                {
                    HardRatings[i] += hardRatings[i];
                    SoftRatings[i] += softRatings[i];
                }
//                qDebug() <<QThread::currentThreadId() << HardRatings;

            }

            delete res;
        }
}

void GenerateWeightsAndCalculateRatingsAsync::run()
{
    //QVector<double> set(IO::IndicatorsNames.size(), 0);

    double *set1 = new double[IO::IndicatorsNames.size()];

    for (int i = 0; i < IO::IndicatorsNames.size(); i++)
        set1[i] = 0;

    Calculate(set1, IO::IndicatorsNames.size(), 0);
}
