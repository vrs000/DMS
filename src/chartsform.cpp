#include "chartsform.h"
#include "ui_chartsform.h"

#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>
#include <QHorizontalStackedBarSeries>
#include <QTableWidget>
#include <QValueAxis>
#include <QChartView>
#include <QWidget>
#include <QHBoxLayout>
#include <QSplitter>

using namespace QtCharts;

ChartsForm::ChartsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartsForm)
{
    ui->setupUi(this);

    this->showMaximized();
    font.setPointSize(10);
    font.setBold(true);


    listViews.clear();


}

ChartsForm::~ChartsForm()
{
    delete ui;
}

void ChartsForm::FillingCharts()
{

//    if (PreSelectionChartsForm::SelectedSolutions.size() == 0)
//    {
//        AddSolutionsCharts(SolutionDB::currentSolutionName);
//    }

//    if (PreSelectionChartsForm::SelectedSolutions.size() != 0)

        for (auto sol : PreSelectionChartsForm::SelectedSolutions)
        {
            AddSolutionsCharts(sol);
        }

    PreSelectionChartsForm::SelectedSolutions.clear();
}


void ChartsForm::AddSolutionsCharts(Solution solution)
{
    QChartView* DiagramsView = new QChartView();
    QChartView* ScatterView = new QChartView();
    QChartView* HorizontalBarsView = new QChartView();


    QFont font;
    font.setBold(true);
    font.setItalic(true);


    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);


    QGroupBox *groupBox = new QGroupBox();
    QVBoxLayout* layout = new QVBoxLayout();



    groupBox->setLayout(layout);
    groupBox->setTitle(solution.SolutionName);
    groupBox->setFont(font);

    ui->ChartsField->addWidget(groupBox);


    QScrollArea* diagramsArea = new QScrollArea();
    QScrollArea* horizontalBarsArea = new QScrollArea();


    diagramsArea->setWidget(DiagramsView);
    diagramsArea->setWidgetResizable(true);


    horizontalBarsArea->setWidget(HorizontalBarsView);
    horizontalBarsArea->setWidgetResizable(true);


    splitter->addWidget(diagramsArea);
    splitter->addWidget(ScatterView);
    splitter->addWidget(horizontalBarsArea);


    layout->addWidget(splitter);



    QChart *diagrams;
    QChart* scatter;
    QChart* HorizontalBars;

    //Если количество проектов больше заданного топа
    if ((solution.ProjectsNames.size() > (int)top) && (top != All))
    {
        QVector<double> hardRatings;
        QVector<double> softRatings;
        QVector<QString> projectsNames;
        QVector<QVector<double>> normalizedTable;
        FindTopProjects(hardRatings, softRatings, projectsNames, normalizedTable, top, solution);


        diagrams = GetBarChart(hardRatings, softRatings, projectsNames);
        scatter = GetScatterChart(hardRatings, softRatings, projectsNames);
        HorizontalBars = GetHorizontalStackedBarChart(normalizedTable, solution.IndicatorsNames, projectsNames);
    }

    //Если количество проектов меньше заданнаго топа
    if ((solution.ProjectsNames.size() <= (int)top) || (top == All))
    {
        diagrams = GetBarChart(solution.HardRatings, solution.SoftRatings, solution.ProjectsNames);
        scatter = GetScatterChart(solution.HardRatings, solution.SoftRatings, solution.ProjectsNames);
        HorizontalBars = GetHorizontalStackedBarChart(solution.NormalizedTable, solution.IndicatorsNames, solution.ProjectsNames);
    }


    DiagramsView->setChart(diagrams);
    ScatterView->setChart(scatter);
    HorizontalBarsView->setChart(HorizontalBars);


    diagramsArea->setMinimumHeight(230);
    horizontalBarsArea->setMinimumHeight(230);
    ScatterView->setMinimumHeight(230);


    //save
    Controls c;
    c.DiagramsView = DiagramsView;
    c.HorizontalBarsView = HorizontalBarsView;
    listViews << c;
}

void ChartsForm::AddSolutionsCharts(const QString solutionName)
{


    QChartView* DiagramsView = new QChartView();
    QChartView* ScatterView = new QChartView();
    QChartView* HorizontalBarsView = new QChartView();



    QFont font;
    font.setBold(true);
    font.setUnderline(true);
    font.setPixelSize(12);


    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);


    QGroupBox *groupBox = new QGroupBox();
    QVBoxLayout* layout = new QVBoxLayout();



    groupBox->setLayout(layout);
    groupBox->setTitle(solutionName);
    groupBox->setFont(font);

    ui->ChartsField->addWidget(groupBox);

    QScrollArea* diagramsArea = new QScrollArea();
    QScrollArea* horizontalBarsArea = new QScrollArea();

    QVBoxLayout* diagramsAreaLayout = new QVBoxLayout();
    QVBoxLayout* horizontalBarsAreaLayout = new QVBoxLayout();



    //diagramsArea->setLayout(diagramsAreaLayout);

    diagramsArea->setWidget(DiagramsView);
    diagramsArea->setWidgetResizable(true);


    horizontalBarsArea->setWidget(HorizontalBarsView);
    horizontalBarsArea->setWidgetResizable(true);


    //splitter->addWidget(DiagramsView);

    splitter->addWidget(diagramsArea);
    splitter->addWidget(ScatterView);
    splitter->addWidget(horizontalBarsArea);

    //splitter->addWidget(HorizontalBarsView);


    layout->addWidget(splitter);

    QChart *diagrams;
    QChart* scatter;
    QChart* HorizontalBars;

    //Если количество проектов больше заданного топа
    if ((IO::ProjectsNames.size() > (int)top) && (top != All))
    {
        QVector<double> hardRatings;
        QVector<double> softRatings;
        QVector<QString> projectsNames;
        QVector<QVector<double>> normalizedTable;

        Solution solution(solutionName, DataProcessing::CrushingStep,
                          IO::IndicatorsNames, IO::ProjectsNames,
                          IO::BaseTable, DataProcessing::NormalizedTable,
                          DataProcessing::HardRatings, DataProcessing::SoftRatings,
                          DataProcessing::PriorityList, DataProcessing::PrefferedMetrics,
                          DataProcessing::RejectedMetrics);

        FindTopProjects(hardRatings, softRatings, projectsNames, normalizedTable, top, solution);

        diagrams = GetBarChart(hardRatings, softRatings, projectsNames);
        scatter = GetScatterChart(hardRatings, softRatings, projectsNames);
        HorizontalBars = GetHorizontalStackedBarChart(normalizedTable, solution.IndicatorsNames, projectsNames);
    }

    //Если количество проектов меньше заданнаго топа
    if ((IO::ProjectsNames.size() <= (int)top) || (top == All))
    {
        diagrams = GetBarChart(DataProcessing::HardRatings, DataProcessing::SoftRatings, IO::ProjectsNames);
        scatter = GetScatterChart(DataProcessing::HardRatings, DataProcessing::SoftRatings, IO::ProjectsNames);
        HorizontalBars = GetHorizontalStackedBarChart(DataProcessing::NormalizedTable, IO::IndicatorsNames, IO::ProjectsNames);
    }




    //    QChart *diagrams = GetBarChart(DataProcessing::HardRatings, DataProcessing::SoftRatings, IO::ProjectsNames);
    //    QChart* scatter = GetScatterChart(DataProcessing::HardRatings, DataProcessing::SoftRatings, IO::ProjectsNames);
    //    QChart* HorizontalBars = GetHorizontalStackedBarChart(DataProcessing::NormalizedTable, IO::IndicatorsNames, IO::ProjectsNames);





    DiagramsView->setChart(diagrams);
    ScatterView->setChart(scatter);
    HorizontalBarsView->setChart(HorizontalBars);



    diagramsArea->setMinimumHeight(230);
    horizontalBarsArea->setMinimumHeight(230);

    //DiagramsView->setMinimumHeight(230);
    ScatterView->setMinimumHeight(230);
    //HorizontalBarsView->setMinimumHeight(230);

    //save
    Controls c;
    c.DiagramsView = DiagramsView;
    c.HorizontalBarsView = HorizontalBarsView;
    listViews << c;
}


QChart* ChartsForm::GetBarChart(QVector<double> hardRating, QVector<double> softRating, QVector<QString> projectsNames)
{
    QChart *diagrams = new QChart();
    diagrams->setTitle("");
    QColor color;

    QBarSeries *series = new QBarSeries();
    QBarSet* hardSet = new QBarSet("Жёсткий рейтинг");
    QBarSet* softSet = new QBarSet("Мягкий рейтинг");

    color.setRgb(255, 0, 0, 180);
    hardSet->setColor(color);
    color.setRgb(0,0,255,180);
    softSet->setColor(color);

    //Формирование наборов данных
    for (int i=0; i < projectsNames.count(); i++)
    {
        *hardSet<< hardRating[i];
        *softSet << softRating[i];
    }

    series->append(hardSet);
    series->append(softSet);

    diagrams->addSeries(series);

    //Формирование категорий
    QStringList categories;

    for (auto name: projectsNames)
        categories << name;
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    diagrams->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0,1.5);
    diagrams->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);


    diagrams->setAnimationOptions(QChart::SeriesAnimations);


    diagrams->legend()->setFont(font);
    diagrams->legend()->setAlignment(Qt::AlignRight);

    return diagrams;
}
QChart* ChartsForm::GetScatterChart(QVector<double> hardRating, QVector<double> softRating, QVector<QString> projectsNames)
{
    QColor color;

    QChart* scatter = new QChart();

    for (int i=0; i < projectsNames.size(); i++)
    {
        QScatterSeries* scatterSeries = new QScatterSeries();
        scatterSeries->setName(projectsNames[i]);
        scatterSeries->append(hardRating[i], softRating[i]);
        color.setRgb(rand()%255, rand()%255, rand()%255);
        scatterSeries->setColor(color);
        scatter->addSeries(scatterSeries);
    }


    scatter->createDefaultAxes();
    scatter->axes(Qt::Vertical).first()->setRange(0,1.5);
    scatter->axes(Qt::Horizontal).first()->setRange(0,1.5);
    scatter->axes(Qt::Vertical).first()->setTitleText("Мягкий рейтинг");
    scatter->axes(Qt::Horizontal).first()->setTitleText("Жёсткий рейтинг");

    scatter->legend()->setAlignment(Qt::AlignRight);
    scatter->legend()->setFont(font);

    scatter->setAnimationOptions(QChart::SeriesAnimations);


    return scatter;
}
QChart* ChartsForm::GetHorizontalStackedBarChart(QVector<QVector<double> > normalizedTable, QVector<QString> indicatorsNames, QVector<QString> projectsNames)
{
    QChart* HorizontalBars = new QChart();
    QList<QBarSet*> listSet;

    QStringList categories;

    for (auto name: projectsNames)
        categories << name;

    for (auto param : indicatorsNames)
        listSet << new QBarSet(param);

    QHorizontalStackedBarSeries* StackedBarSeries = new QHorizontalStackedBarSeries();

    for (int j=0; j < indicatorsNames.size(); j++)
    {
        for (int i=0; i < projectsNames.size(); i++)
        {

            listSet[j]->append(normalizedTable[i][j]);
        }
        StackedBarSeries->append(listSet[j]);
    }


    HorizontalBars->addSeries(StackedBarSeries);


    QBarCategoryAxis* _axisY = new QBarCategoryAxis();
    _axisY->append(categories);
    HorizontalBars->addAxis(_axisY, Qt::AlignLeft);
    StackedBarSeries->attachAxis(_axisY);

    //QValueAxis *_axisX = new QValueAxis();
    //HorizontalBars->addAxis(_axisX, Qt::AlignBottom);
    //StackedBarSeries->attachAxis(_axisX);

    HorizontalBars->legend()->setAlignment(Qt::AlignRight);
    HorizontalBars->legend()->setFont(font);


    HorizontalBars->setAnimationOptions(QChart::SeriesAnimations);


    return HorizontalBars;
}


void ChartsForm::FindTopProjects(QVector<double>& hardRatings, QVector<double>& softRatings,
                                 QVector<QString>& projectsNames, QVector<QVector<double> >& normalizedTable,
                                 Top top, Solution solution)
{
    QList<int> topIndex;


    int max = 0;

    auto _HardRatings = solution.HardRatings;

    qSort(_HardRatings.begin(), _HardRatings.end());


    for (int i=0; i < (int)top; i++)
    {
        int index = solution.HardRatings.indexOf(_HardRatings[_HardRatings.size() - 1 - i]);

        //Если имеют топы с одинаковыми значениями
        if (topIndex.contains(index))
        {
            for (int j=0; j < solution.HardRatings.size(); j++)
                //Если нашли такое же значение и индекс не содержится в topIndex
                if ((solution.HardRatings[j] == _HardRatings[_HardRatings.size() - 1 - i]) && (!topIndex.contains(j)))
                {
                    topIndex << j;
                    break;
                }
        }
        else
            topIndex << index;
    }



    for (int i=0; i < topIndex.size(); i++)
    {
        hardRatings << solution.HardRatings[topIndex[i]];
        softRatings << solution.SoftRatings[topIndex[i]];
        projectsNames << solution.ProjectsNames[topIndex[i]];
        normalizedTable << solution.NormalizedTable[topIndex[i]];
    }

}


void ChartsForm::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();


    const int StepWidth = 300;
    const int StepHeight = 300;

    switch (key)
    {
    case Qt::Key_A:
        for (auto c : listViews)
            c.DiagramsView->setMinimumWidth(c.DiagramsView->width() - StepWidth);
        //c.DiagramsView->setMinimumWidth(c.DiagramsView->minimumWidth() - StepWidth);
        break;

    case Qt::Key_D:
        for (auto c : listViews)
            c.DiagramsView->setMinimumWidth(c.DiagramsView->width() + StepWidth);
        break;

    case Qt::Key_W:
        for (auto c : listViews)
            c.HorizontalBarsView->setMinimumHeight(c.HorizontalBarsView->height() - StepHeight);
        break;

    case Qt::Key_S:
        for (auto c : listViews)
            c.HorizontalBarsView->setMinimumHeight(c.HorizontalBarsView->height() + StepHeight);
        break;

    case Qt::Key_R:
        for (auto c : listViews)
        {
            c.DiagramsView->setMinimumWidth(0);
            c.HorizontalBarsView->setMinimumHeight(0);
        }
        break;

    }


}
