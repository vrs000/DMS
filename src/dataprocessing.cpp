#include "dataprocessing.h"
#include <QMessageBox>

int DataProcessing::UsedThreadCount = 0;
int DataProcessing::TimeElapsed = 0;
QString DataProcessing::timeElapsedParsed = "";


QList<QList<double>> DataProcessing::weights;
QList<QList<double>> DataProcessing::LinearConv;
QList<QList<double>> DataProcessing::hard;
QList<QList<double>> DataProcessing::soft;


int DataProcessing::MaxIterCountForLogging = 200;
bool DataProcessing::IsLoggingUsed = false;
int DataProcessing::Generetaed_count = 0;

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

void DataProcessing::MakeLogsAsXlsx()
{
    weights.clear();
    LinearConv.clear();
    hard.clear();
    soft.clear();


    Generetaed_count = 0;

    //CONSTANTS
    //-----------------------------------------------------------------------------------
    const QString DirName = "./logs";
    const QString filePath = QString("%1/%2.xlsx")
            .arg(DirName)
            .arg(OpenedSolutionName + "_" + QTime().currentTime().toString("hh_mm_ss"));

    const int StartColForInfo = 1;
    const int StartRowForInfo = 1;
    //-----------------------------------------------------------------------------------

    if (!QDir(DirName).exists())
        QDir().mkdir(DirName);


    Document xlsx;

    RichString rich_text;

    //FORMATS
    //----------------------------------------------------

    Format format;
    format.setFont(QFont("Times New Roman"));
    format.setFontSize(14);
    format.setFontItalic(true);

    Format index_format;
    index_format.setFont(QFont("Times New Roman"));
    index_format.setFontSize(9);
    index_format.setFontItalic(true);

    Format center;
    center.setHorizontalAlignment(Format::AlignHCenter);

    Format HVcenter;
    HVcenter.setHorizontalAlignment(Format::AlignHCenter);
    HVcenter.setVerticalAlignment(Format::AlignVCenter);
    HVcenter.setFontName("Times New Roman");
    HVcenter.setFontSize(14);
    HVcenter.setTextWarp(true);
    HVcenter.setTopBorderStyle(Format::BorderMedium);

    Format upper_board;
    upper_board.setTopBorderStyle(Format::BorderMedium);
    upper_board.setHorizontalAlignment(Format::AlignHCenter);

    Format lower_board;
    lower_board.setBottomBorderStyle(Format::BorderMedium);
    lower_board.setHorizontalAlignment(Format::AlignHCenter);
    //----------------------------------------------------




    xlsx.setColumnWidth(CellRange(StartRowForInfo, StartColForInfo,
                                  StartRowForInfo, StartColForInfo+PriorityList.size()), 16);


//    xlsx.setColumnWidth(CellRange(StartRowForInfo, StartColForInfo,
//                                  StartRowForInfo, StartColForInfo+PriorityList.size()), 16);

    //Trends
    for (int i=0; i<PriorityList.size(); i++)
    {
        rich_text = RichString();
        rich_text.addFragment("x", format);
        rich_text.addFragment(tr("i%1").arg(i+1), index_format);
        rich_text.addFragment(tr(" → %1").arg(PriorityList[i]), format);
        xlsx.write(StartRowForInfo, StartColForInfo+i, rich_text, center);
    }

    //Importance indicator groups
    if (NotParsedImportanceGroupOfIndicators != "")
    {
        QStringList pairs = NotParsedImportanceGroupOfIndicators.split(",");

        for (int i=0; i<pairs.size(); i++)
        {
            QString first, second, sign;

            if (pairs[i].contains("≥"))
            {
                first = pairs[i].split("≥").first();
                sign = "≥";
                second = pairs[i].split("≥").last();
            }

            if (pairs[i].contains(">"))
            {
                first = pairs[i].split(">").first();
                sign = ">";
                second = pairs[i].split(">").last();
            }

            if (pairs[i].contains("="))
            {
                first = pairs[i].split("=").first();
                sign = "=";
                second = pairs[i].split("=").last();
            }


            rich_text = RichString();
            rich_text.addFragment("a", format);
            rich_text.addFragment(tr("%1l").arg(first.toInt()+1), index_format);
            rich_text.addFragment(tr(" %1 ").arg(sign), format);
            rich_text.addFragment("a", format);
            rich_text.addFragment(tr("%1l").arg(second.toInt()+1), index_format);
            xlsx.write(StartRowForInfo+1, StartColForInfo+i, rich_text, center);
        }
    }

    //Importance projects groups
    if (NotParsedImportanceGroupOfProjects != "")
    {
        QStringList pairs = NotParsedImportanceGroupOfProjects.split(",");


        for (int i=0; i<pairs.size(); i++)
        {
            QString first, second, sign;

            if (pairs[i].contains("≥"))
            {
                first = pairs[i].split("≥").first();
                sign = "≥";
                second = pairs[i].split("≥").last();
            }

            if (pairs[i].contains(">"))
            {
                first = pairs[i].split(">").first();
                sign = ">";
                second = pairs[i].split(">").last();
            }

            if (pairs[i].contains("="))
            {
                first = pairs[i].split("=").first();
                sign = "=";
                second = pairs[i].split("=").last();
            }


            rich_text = RichString();
            rich_text.addFragment("F(x)", format);
            rich_text.addFragment(tr("%1").arg(first.toInt()+1), index_format);
            rich_text.addFragment(tr(" %1 ").arg(sign), format);
            rich_text.addFragment("F(x)", format);
            rich_text.addFragment(tr("%1").arg(second.toInt()+1), index_format);
            xlsx.write(StartRowForInfo+2, StartColForInfo+i, rich_text, center);
        }
    }


    // Расшифровка показателей
    for (int i=0; i<IndicatorsCount; i++)
    {
        rich_text = RichString();
        rich_text.addFragment("x", format);
        rich_text.addFragment(tr("i%1").arg(i+1), index_format);
        rich_text.addFragment(tr(" → %1").arg(IO::IndicatorsNames[i]), index_format);
        xlsx.write(StartRowForInfo+3, StartColForInfo+i, rich_text, center);
        xlsx.setColumnWidth(StartColForInfo+i, IO::IndicatorsNames[i].size()*2);
    }

    const int startX = StartColForInfo;
    const int startY= StartRowForInfo + 5;

    //start Filling


    // Заголовки таблицы


    // l
    //--------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX, startY+1, startX));
    HVcenter.setFontItalic(true);
    HVcenter.setFontSize(18);
    xlsx.write(startY, startX, "l", HVcenter);
    //--------------------------------------------------------------


    // Проекты
    //--------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX+1, startY+1, startX+1));
    HVcenter.setFontItalic(false);
    HVcenter.setFontSize(11);
    xlsx.write(startY, startX+1, "Проекты", HVcenter);
    xlsx.write(startY+1, startX+1, "Проекты", lower_board);
    //--------------------------------------------------------------


    // Исходные данные
    //--------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX+2, startY, startX+2 + PriorityList.size()-1));
    xlsx.write(startY, startX+2, "Исходные данные", HVcenter);
    xlsx.write(startY, startX+3, "", upper_board);
    //--------------------------------------------------------------


    // Нормированные данные
    //--------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX+2+PriorityList.size(), startY, startX+1+2*PriorityList.size()));
    xlsx.write(startY, startX+2+PriorityList.size(), "Нормированные данные", HVcenter);
    xlsx.write(startY, startX+2+PriorityList.size()+1, "", upper_board);
    //--------------------------------------------------------------


    // Коэффициенты линейной свертки
    //--------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX+2+2*PriorityList.size(), startY, startX+1+3*PriorityList.size()));
    xlsx.write(startY, startX+2+2*PriorityList.size(), "Коэффициенты линейной свертки", HVcenter);
    xlsx.write(startY, startX+2+2*PriorityList.size()+1, "", upper_board);
    //--------------------------------------------------------------


    // X ij  и a ij
    //--------------------------------------------------------------------
    for (int i=0; i<PriorityList.size(); i++)
    {
        if (i>=1)
        {
            xlsx.write(startY, startX+2+i, "", upper_board);
            xlsx.write(startY, startX+2+PriorityList.size()+i, "", upper_board);
            xlsx.write(startY, startX+2+2*PriorityList.size()+i, "", upper_board);
        }
        rich_text = RichString();
        rich_text.addFragment("x", format);
        rich_text.addFragment(tr("i%1").arg(i+1), index_format);
        xlsx.write(startY+1, startX+2+i, rich_text, lower_board);
        xlsx.write(startY+1, startX+2+PriorityList.size()+i, rich_text, lower_board);

        rich_text = RichString();
        rich_text.addFragment("a", format);
        rich_text.addFragment(tr("%1l").arg(i+1), index_format);
        xlsx.write(startY+1, startX+2+2*PriorityList.size()+i, rich_text, lower_board);
    }
    //--------------------------------------------------------------------


    // F(x)
    //--------------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX+2+3*PriorityList.size(), startY+1, startX+2+3*PriorityList.size()));
    xlsx.write(startY, startX+2+3*PriorityList.size(), "F(x)", HVcenter);
    xlsx.write(startY+1, startX+2+3*PriorityList.size(), "", lower_board);
    //--------------------------------------------------------------------


    // Выигрыш
    //--------------------------------------------------------------------
    xlsx.mergeCells(CellRange(startY, startX+2+3*PriorityList.size()+1, startY+1, startX+2+3*PriorityList.size()+1));
    xlsx.write(startY, startX+2+3*PriorityList.size()+1, "Выигрыш", HVcenter);
    xlsx.write(startY+1, startX+2+3*PriorityList.size()+1, "", lower_board);
    //--------------------------------------------------------------------


    // F(x)/F(x)max
    //----------------------------------------------------------------------------------------------------------------
    HVcenter.setRightBorderStyle(Format::BorderMedium);
    lower_board.setRightBorderStyle(Format::BorderMedium);

    xlsx.mergeCells(CellRange(startY, startX+2+3*PriorityList.size()+2, startY+1, startX+2+3*PriorityList.size()+2));
    xlsx.write(startY, startX+2+3*PriorityList.size()+2, "F(x)/F(x)max", HVcenter);
    xlsx.write(startY+1, startX+2+3*PriorityList.size()+2, "", lower_board);

    HVcenter.setRightBorderStyle(Format::BorderNone);
    lower_board.setRightBorderStyle(Format::BorderNone);
    //----------------------------------------------------------------------------------------------------------------

    int pr_groups = NotParsedImportanceGroupOfProjects.split(",").size();
    int ind_groups = NotParsedImportanceGroupOfIndicators.split(",").size();

    auto IndicatorsGroups = NotParsedImportanceGroupOfIndicators.split(",");
    auto ProjectsGroups = NotParsedImportanceGroupOfProjects.split(",");

    int *prefferedProject = new int[pr_groups];
    int *rejectedProejct = new int[pr_groups];
    char *signProject = new char[pr_groups];

    int *prefferedIndicator = new int[ind_groups];
    int *rejectedIndicator = new int[ind_groups];
    char *signIndicator = new char[ind_groups];


    if (NotParsedImportanceGroupOfProjects != "")
    {
        for (int i=0; i<ProjectsGroups.size(); i++)
        {

            if (ProjectsGroups[i].contains("="))
            {
                auto numbs = ProjectsGroups[i].split("=");

                prefferedProject[i] = numbs[0].toInt();
                rejectedProejct[i] = numbs[1].toInt();
                signProject[i] = '=';

                continue;
            }


            if (ProjectsGroups[i].contains(">"))
            {
                auto numbs = ProjectsGroups[i].split(">");

                prefferedProject[i] = numbs[0].toInt();
                rejectedProejct[i] = numbs[1].toInt();
                signProject[i] = '>';

                continue;
            }


            if (ProjectsGroups[i].contains("≥"))
            {
                auto numbs = ProjectsGroups[i].split("≥");

                prefferedProject[i] = numbs[0].toInt();
                rejectedProejct[i] = numbs[1].toInt();
                signProject[i] = 'b';

                continue;
            }
        }

    }

    if (NotParsedImportanceGroupOfIndicators!= "")
    {

        for (int i=0; i<IndicatorsGroups.size(); i++)
        {

            if (IndicatorsGroups[i].contains("="))
            {
                auto numbs = IndicatorsGroups[i].split("=");

                prefferedIndicator[i] = numbs[0].toInt();
                rejectedIndicator[i] = numbs[1].toInt();
                signIndicator[i] = '=';

                continue;
            }


            if (IndicatorsGroups[i].contains(">"))
            {
                auto numbs = IndicatorsGroups[i].split(">");

                prefferedIndicator[i] = numbs[0].toInt();
                rejectedIndicator[i] = numbs[1].toInt();
                signIndicator[i] = '>';

                continue;
            }


            if (IndicatorsGroups[i].contains("≥"))
            {
                auto numbs = IndicatorsGroups[i].split("≥");

                prefferedIndicator[i] = numbs[0].toInt();
                rejectedIndicator[i] = numbs[1].toInt();
                signIndicator[i] = 'b';

                continue;
            }
        }
    }

    double *set = new double[IndicatorsCount];

    for (int i=0; i<IndicatorsCount; i++)
        set[i]=0;

    GetNextNum(set, IndicatorsCount, 0,
               prefferedProject, rejectedProejct, signProject, pr_groups,
               prefferedIndicator, rejectedIndicator, signIndicator, ind_groups);


    int RealCount = MaxIterCountForLogging > CurrentIterationCount ? CurrentIterationCount : MaxIterCountForLogging;
    int startRow = startY+2;
    int startCol = startX;


    // заполнение итераций по блокам
    for (int i=0; i<weights.size(); i++)
    {
        // Iter number
        //--------------------------------------------------------------------
        xlsx.mergeCells(CellRange(startRow, startCol, startRow+ProjectsCount-1, startCol));
        if (i==weights.size()-1)
            xlsx.write(startRow+ProjectsCount-1, startCol, "", lower_board);

        xlsx.write(startRow, startCol, i+1, HVcenter);
        //--------------------------------------------------------------------


        //Project names
        //--------------------------------------------------------------------
        for (int index=0; index<ProjectsCount; index++)
        {
            if (index == 0)
            {
                upper_board.setHorizontalAlignment(Format::AlignLeft);
                xlsx.write(startRow+index, startCol+1, ParettoSetProjectsIndexes.size() ? ParettoSetProjects[index] : IO::ProjectsNames[index], upper_board);
                upper_board.setHorizontalAlignment(Format::AlignHCenter);
            }
            else
                xlsx.write(startRow+index, startCol+1, ParettoSetProjectsIndexes.size() ? ParettoSetProjects[index] : IO::ProjectsNames[index]);

            if (index == ProjectsCount-1 && i == weights.size()-1)
            {
                lower_board.setHorizontalAlignment(Format::AlignLeft);
                xlsx.write(startRow+index, startCol+1, ParettoSetProjectsIndexes.size() ? ParettoSetProjects[index] : IO::ProjectsNames[index], lower_board);
                lower_board.setHorizontalAlignment(Format::AlignHCenter);
            }

        }
        //--------------------------------------------------------------------


        // исходные и нормированные значения
        //--------------------------------------------------------------------
        int r = 0;
        for (int row=0; row < ProjectsCount; row++)
        {
            for (int col=0; col < IndicatorsCount; col++)
            {
                if (row == 0)
                {
                    if (ParettoSetProjectsIndexes.size() != 0)
                    {
                        xlsx.write(startRow+row, startCol+2+col, IO::BaseTable[ParettoSetProjectsIndexes[row]][col], upper_board);
                        xlsx.write(startRow+row, startCol+2+PriorityList.size()+col, NormalizedTable[ParettoSetProjectsIndexes[row]][col], upper_board);
                    }
                    else
                    {
                        xlsx.write(startRow+row, startCol+2+col, IO::BaseTable[row][col], upper_board);
                        xlsx.write(startRow+row, startCol+2+PriorityList.size()+col, NormalizedTable[row][col], upper_board);
                    }

                }
                else
                {
                    if (ParettoSetProjectsIndexes.size() != 0)
                    {
                        xlsx.write(startRow+row, startCol+2+col, IO::BaseTable[ParettoSetProjectsIndexes[row]][col], center);
                        xlsx.write(startRow+row, startCol+2+PriorityList.size()+col, NormalizedTable[ParettoSetProjectsIndexes[row]][col], center);
                    }
                    else
                    {
                        xlsx.write(startRow+row, startCol+2+col, IO::BaseTable[row][col], center);
                        xlsx.write(startRow+row, startCol+2+PriorityList.size()+col, NormalizedTable[row][col], center);
                    }
                }


                if (row == ProjectsCount-1 && i == weights.size()-1)
                {
                    if (ParettoSetProjectsIndexes.size() != 0)
                    {
                        xlsx.write(startRow+row, startCol+2+col, IO::BaseTable[ParettoSetProjectsIndexes[row]][col], lower_board);
                        xlsx.write(startRow+row, startCol+2+PriorityList.size()+col, NormalizedTable[ParettoSetProjectsIndexes[row]][col], lower_board);
                    }
                    else
                    {
                        xlsx.write(startRow+row, startCol+2+col, IO::BaseTable[row][col], lower_board);
                        xlsx.write(startRow+row, startCol+2+PriorityList.size()+col, NormalizedTable[row][col], lower_board);
                    }
                }
            }

            r++;
        }
        //--------------------------------------------------------------------

        // коэффициэнты линейной свертки
        for (int pr=0; pr < ProjectsCount; pr++)
            for (int j=0; j<weights[i].size(); j++)
            {
                if (pr == 0)
                    xlsx.write(startRow+pr, startCol+2+2*IndicatorsCount+j, weights[i][j], upper_board);
                else
                    xlsx.write(startRow+pr, startCol+2+2*IndicatorsCount+j, weights[i][j], center);


                if (pr == ProjectsCount-1 && i == weights.size()-1)
                    xlsx.write(startRow+pr, startCol+2+2*IndicatorsCount+j, weights[i][j], lower_board);
            }

        // F(x)
        for (int pr=0; pr < ProjectsCount; pr++)
        {
            if (pr == 0)
                xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount, LinearConv[i][pr], upper_board);
            else
                xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount, LinearConv[i][pr], center);

            if (pr == ProjectsCount-1 && i == weights.size()-1)
                xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount, LinearConv[i][pr], lower_board);
        }

        // Хард
        for (int pr=0; pr < ProjectsCount; pr++)
        {
            if (pr == 0)
            xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount+1, hard[i][pr], upper_board);
            else
            xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount+1, hard[i][pr], center);

            if (pr == ProjectsCount-1 && i == weights.size()-1)
            xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount+1, hard[i][pr], lower_board);
        }

        // F(x)/F(x)max
        for (int pr=0; pr < ProjectsCount; pr++)
        {
            upper_board.setRightBorderStyle(Format::BorderMedium);
            center.setRightBorderStyle(Format::BorderMedium);

            if (pr == 0)
                xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount+2, soft[i][pr], upper_board);
            else
                xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount+2, soft[i][pr], center);

            if (pr == ProjectsCount-1 && i == weights.size()-1)
            {
                center.setBottomBorderStyle(Format::BorderMedium);
                xlsx.write(startRow+pr, startCol+2+3*IndicatorsCount+2, soft[i][pr], center);
                center.setBottomBorderStyle(Format::BorderNone);
            }

            upper_board.setRightBorderStyle(Format::BorderNone);
            center.setRightBorderStyle(Format::BorderNone);
        }

        startRow += ProjectsCount;
    }


    startRow = startY+2;
    startCol = startX+2;

    startRow += weights.size()*ProjectsCount;

    startRow += 2;
    startCol = 2;

//    startRow = startY + 1;
//    startCol = 3+3*IndicatorsCount+3+1;


    xlsx.mergeCells(CellRange(startRow-1, startCol-1, startRow, startCol-1));
    xlsx.mergeCells(CellRange(startRow-1, startCol, startRow, startCol));
    xlsx.mergeCells(CellRange(startRow-1, startCol+1, startRow, startCol+1));

    HVcenter.setFontBold(true);
    HVcenter.setFontSize(14);
    HVcenter.setBorderStyle(Format::BorderDouble);


    xlsx.write(startRow-1, startCol-1, "Проекты", HVcenter);
    xlsx.write(startRow-1, startCol, "HR", HVcenter);
    xlsx.write(startRow-1, startCol+1, "SR", HVcenter);


    xlsx.write(startRow, startCol-1, "", HVcenter);
    xlsx.write(startRow, startCol, "", HVcenter);
    xlsx.write(startRow, startCol+1, "", HVcenter);


    double HR = 0;
    double SR = 0;

    center.setBorderStyle(Format::BorderThin);

    for (int i=0; i<ProjectsCount; i++)
    {
        HR = 0;
        SR = 0;

        for (int l=0; l<hard.size(); l++)
        {
            HR += hard[l][i];
            SR += soft[l][i];
        }

        HR *= 1.0/hard.size();
        SR *= 1.0/soft.size();


        xlsx.write(startRow+1+i, startCol-1, IO::ProjectsNames[i], center);
        xlsx.write(startRow+1+i, startCol, HR, center);
        xlsx.write(startRow+1+i, startCol+1, SR, center);
    }

    xlsx.saveAs(filePath);
}

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


void DataProcessing::GetNextNum(double currentSet[], int maxN, int curPosIndex,
                                int *preffered_Project, int *rejected_Project, char *sign_Project, int p_count,
                                int *preffered_Indicator, int *rejected_Indicator, char *sign_Indicator, int i_count)
{
    if (curPosIndex < maxN - 1)
    {
        double lim = 1  - Sum(currentSet, curPosIndex);
        for (double i = 0; i <= lim + 0.00001; i += CrushingStep)
        {
            currentSet[curPosIndex] = i;
            GetNextNum(currentSet, maxN, curPosIndex + 1,
                       preffered_Project, rejected_Project, sign_Project, p_count,
                       preffered_Indicator, rejected_Indicator, sign_Indicator, i_count);
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

//            Generetaed_count++;

//            if (Generetaed_count <= MaxIterCountForLogging)

                double *res = GetLinearConvolutionResult(currentSet);
                bool isSuitable = true;

                // Проверка проектов
                //----------------------------------
                if (NotParsedImportanceGroupOfProjects != "")
                {
                    for (int i=0; i<p_count; i++)
                    {
                        if (sign_Project[i] == '=')
                            if (!(res[preffered_Project[i]] == res[rejected_Project[i]]))
                            {
                                isSuitable = false;
                                break;
                            }

                        if (sign_Project[i] == '>')
                            if (!(res[preffered_Project[i]] > res[rejected_Project[i]]))
                            {
                                isSuitable = false;
                                break;
                            }

                        if (sign_Project[i] == 'b')
                            if (!(res[preffered_Project[i]] >= res[rejected_Project[i]]))
                            {
                                isSuitable = false;
                                break;
                            }
                    }
                }
                //----------------------------------

                // Проверка показателей
                //----------------------------------
                if (NotParsedImportanceGroupOfIndicators != "")
                {
                    for (int i = 0; i < i_count; i++)
                    {
                        if (sign_Indicator[i] == '=')
                            if (!(currentSet[preffered_Indicator[i]] == currentSet[rejected_Indicator[i]]))
                            {
                                isSuitable = false;
                                break;
                            }

                        if (sign_Indicator[i] == '>')
                            if (!(currentSet[preffered_Indicator[i]] > currentSet[rejected_Indicator[i]]))
                            {
                                isSuitable = false;
                                break;
                            }

                        if (sign_Indicator[i] == 'b')
                            if (!(currentSet[preffered_Indicator[i]] >= currentSet[rejected_Indicator[i]]))
                            {
                                isSuitable = false;
                                break;
                            }
                    }
                }
                //----------------------------------


                if (isSuitable && Generetaed_count < MaxIterCountForLogging)
                {
                    Generetaed_count++;

                    QVector<double> softRatings(ProjectsCount, 0);
                    QVector<double> hardRatings(ProjectsCount, 0);

                    //Мягкий рейтинг
                    //-----------------------------------------------
                    for (int i = 0; i < ProjectsCount; i++)
                    {
                        softRatings[i] = res[i] / (getMax(res));
                        hardRatings[i] = 0;
                    }
                    //-----------------------------------------------


                    //Жёсткий рейтинг
                    //-----------------------------------------------------------
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
                    //-----------------------------------------------------------


                    //Занесение результатов

                    hard << hardRatings.toList();
                    soft << softRatings.toList();

                    QList<double> l, l1;
                    for (int i=0; i < ProjectsCount; i++)
                        l << res[i];
                    LinearConv << l;

                    for (int i=0; i < IndicatorsCount; i++)
                        l1 << currentSet[i];
                    weights << l1;

                }


        }
}

double DataProcessing::getMax(double *list)
{
    double max = list[0];

    for (int i = 0; i < IO::ProjectsNames.size(); i++)
        max = list[i] > max ? list[i] : max;

    return max;

}

int DataProcessing::COUNT(double *list, double value)
{
    int k = 0;

    for (int i = 0; i < IO::ProjectsNames.size(); i++)
        if (list[i] == value) k++;

    return k;

}

int DataProcessing::IndexOf(double *list, double value)
{
    for (int i = 0; i < IO::ProjectsNames.size() ; i++)
        if (list[i] == value) return i;

    return -1;
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

    //    qDebug() << res;
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
    //    qDebug()<<TotalStepsCount;
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
                    timeElapsedParsed, TimeElapsed, UsedThreadCount,
                    ParettoSetProjects);


        if (SolutionDB::IsContained(OpenedSolutionName))
            SolutionDB::UpdateSolution(SolutionDB::GetSolution(OpenedSolutionName),
                                       solution);
        else
            SolutionDB::AddSolution(solution);


        QThreadPool::globalInstance()->waitForDone();
        DeleteThreadInstances();
        NormalizedTable.clear();
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
    //    GetNextNum(set, weight_count, 0);
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
        //        qDebug() << a << b;
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

            //            qDebug() << a << b;
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

    if (IsLoggingUsed)
    {
        QThreadPool::globalInstance()->start(MakeLogsAsXlsx);
    }

    time.start();

    threadInstances = getThreadsList(CurrentThreadCount, MaxThreadCount);

    for (int i=0; i<threadInstances.size(); i++)
        threadInstances[i]->start();

}
