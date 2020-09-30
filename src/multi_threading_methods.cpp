#include "multi_threading_methods.h"


OpenExcelFileAsync::OpenExcelFileAsync(const QString &path)
{
    this->path = path;
}

void OpenExcelFileAsync::run()
{
    IO::OpenExelFile(path);

}

RunningBarAsync::RunningBarAsync()
{

}


void RunningBarAsync::run()
{
    DataProcessing::bar->show();
    DataProcessing::bar->setMaximum(DataProcessing::WeightsTable.size());

    DataProcessing::bar->setValue(500);

}


CalculateRatingsAsync::CalculateRatingsAsync(){}

CalculateRatingsAsync::CalculateRatingsAsync(int firstIterationIndex,  int lastIterationIndex)
{
    FirstIterationIndex = firstIterationIndex;
    LastIterationIndex = lastIterationIndex;
}


QVector<double> CalculateRatingsAsync::GetHardRatings()
{
    return HardRatings;
}

QVector<double> CalculateRatingsAsync::GetSoftRatings()
{
    return SoftRatings;
}

int CalculateRatingsAsync::GetCount()
{
    return Count;
}

void CalculateRatingsAsync::run()
{
    bool isSuitable;
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


    for (int count = FirstIterationIndex; count <= LastIterationIndex; count++)
    {
        auto res = DataProcessing::GetLinearConvolutionResult(DataProcessing::WeightsTable[count]);

        //Проверка метрик
        if ((DataProcessing::PrefferedMetrics.size() != 0) && (DataProcessing::RejectedMetrics.size() != 0))
        {

            isSuitable = true;
            for (int i = 0; i < DataProcessing::PrefferedMetrics.size(); i++)
                for (int j = 0; j < DataProcessing::RejectedMetrics.size(); j++)
                {
                    isSuitable = isSuitable &&
                            (DataProcessing::WeightsTable[count][DataProcessing::PrefferedMetrics[i]] > DataProcessing::WeightsTable[count][DataProcessing::RejectedMetrics[j]]);

                }
            if (!isSuitable)
            {
                DataProcessing::missed_variation++;
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


        Count++;

        emit CountChanged(Count);
    }

    HardRatings = hardRatings;
    SoftRatings = softRatings;
}

QMutex CalculateRatingsAsync::mutex;


BAR::BAR()
{

}

void BAR::run()
{
    ProgressBarForm form;
    form.show();

    for (int i=0; i <=100; i++)
    {
        form.bar->setValue(i);
        msleep(1000);
    }
}
