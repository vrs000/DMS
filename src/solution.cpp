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

Solution::Solution(QString solutionName, double crushingStep,
                   QVector<QString> indicatorsNames, QVector<QString> projectsNames,
                   QVector<QVector<double> > baseTable, QVector<QVector<double> > normalizedTable,
                   QVector<double> hardRatings, QVector<double> softRatings,
                   QVector<QString> priorityList,
                   QString projectsImportanceNotParsed, QString indicatorsImportanceNotParsed,
                   QString timeElapsed)
{
    SolutionName = solutionName;
    CrushingStep = crushingStep;
    TimeElapsed = timeElapsed;

    IndicatorsNames = indicatorsNames;
    ProjectsNames = projectsNames;

    BaseTable = baseTable;
    NormalizedTable = normalizedTable;

    HardRatings = hardRatings;
    SoftRatings = softRatings;

    PriorityList = priorityList;

    ProjectsImportanceNotParsed = projectsImportanceNotParsed;
    IndicatorsImportanceNotParsed = indicatorsImportanceNotParsed;
}

Solution::Solution(QString solutionName, double crushingStep,
                   QVector<QString> indicatorsNames, QVector<QString> projectsNames,
                   QVector<QVector<double> > baseTable, QVector<QVector<double> > normalizedTable,
                   QVector<double> hardRatings, QVector<double> softRatings,
                   QVector<QString> priorityList,
                   QString projectsImportanceNotParsed, QString indicatorsImportanceNotParsed,
                   QString timeElapsed, int timeElapsedInMS, int threadUsed,
                   QVector<QString> parettoSetProjects)
{
    SolutionName = solutionName;
    CrushingStep = crushingStep;

    TimeElapsed = timeElapsed;
    TimeElapsedInMS = timeElapsedInMS;
    ThreadUsed = threadUsed;

    IndicatorsNames = indicatorsNames;
    ProjectsNames = projectsNames;

    BaseTable = baseTable;
    NormalizedTable = normalizedTable;

    HardRatings = hardRatings;
    SoftRatings = softRatings;

    PriorityList = priorityList;

    ProjectsImportanceNotParsed = projectsImportanceNotParsed;
    IndicatorsImportanceNotParsed = indicatorsImportanceNotParsed;

    ParettoSetProjects = parettoSetProjects;

    IsParettoCriterionUsed = parettoSetProjects.size() == 0 ? false : true;
}
