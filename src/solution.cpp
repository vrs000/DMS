#include "solution.h"

Solution::Solution()
{

}

Solution::Solution(QString solutionName, double crushingStep,
                   QVector<QString> indicatorsNames, QVector<QString> projectsNames,
                   QVector<QVector<double>> baseTable, QVector<QVector<double>> normalizedTable,
                   QVector<double> hardRatings, QVector<double> softRatings,
                   QVector<QString> priorityList,
                   QVector<int> prefferedMetrics, QVector<int> rejectedMetrics)
{
    SolutionName = solutionName;
    CrushingStep = crushingStep;

    IndicatorsNames = indicatorsNames;
    ProjectsNames = projectsNames;

    BaseTable = baseTable;
    NormalizedTable = normalizedTable;

    HardRatings = hardRatings;
    SoftRatings = softRatings;

    PriorityList = priorityList;

    PrefferedMetrics = prefferedMetrics;
    RejectedMetrics = rejectedMetrics;
}
