#ifndef CHARTSFORM_H
#define CHARTSFORM_H

#include <QWidget>
#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <io.h>
#include <QChart>
#include <QScrollArea>
#include <QGroupBox>
#include <QKeyEvent>
#include <QChartView>
#include <solution.h>
#include <solutiondb.h>
#include <preselectionchartsform.h>


enum Top
{
    Three = 3,
    Five = 5,
    Ten = 10,
    All = -1
};


using namespace QtCharts;
namespace Ui {
class ChartsForm;
}

struct Controls
{
public:
    QChartView* DiagramsView;
    QChartView* HorizontalBarsView;
};

class ChartsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartsForm(QWidget *parent = nullptr);
    ~ChartsForm();

    void FillingCharts();

private:
    void AddSolutionsCharts(Solution solution);
    void AddSolutionsCharts(const QString solutionNames);

    QChart* GetBarChart(QVector<double> hardRating, QVector<double> softRating, QVector<QString> projectsNames);
    QChart* GetScatterChart(QVector<double> hardRating, QVector<double> softRating, QVector<QString> projectsNames);
    QChart* GetHorizontalStackedBarChart(QVector<QVector<double>> normalizedTable, QVector<QString> indicatorsNames, QVector<QString> projectsNames);

    void FindTopProjects(QVector<double>& hardRatings, QVector<double>& softRatings,
                         QVector<QString>& projectsNames,  QVector<QVector<double>>& normalizedTable,
                         Top top, Solution solution);

private:
    QList<Controls> listViews;
    QFont font;

public:
    QList<Solution> SelectedSolutions;
    Top top;

private:
    Ui::ChartsForm *ui;

protected:
    virtual void keyPressEvent(QKeyEvent *event);
};

#endif // CHARTSFORM_H
