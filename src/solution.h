#ifndef SOLUTION_H
#define SOLUTION_H

#include <QList>
#include <QVector>

class Solution
{
public:
    Solution();

    Solution(QString solutionName, double crushingStep,
             QVector<QString> indicatorsNames, QVector<QString> projectsNames,
             QVector<QVector<double>> baseTable, QVector<QVector<double>> normalizedTable,
             QVector<double> hardRatings, QVector<double> softRatings,
             QVector<QString> priorityList,
             QVector<int> prefferedMetrics, QVector<int> rejectedMetrics);

    Solution(QString solutionName, double crushingStep,
             QVector<QString> indicatorsNames, QVector<QString> projectsNames,
             QVector<QVector<double>> baseTable, QVector<QVector<double>> normalizedTable,
             QVector<double> hardRatings, QVector<double> softRatings,
             QVector<QString> priorityList,
             QString projectsImportanceNotParsed, QString indicatorsImportanceNotParsed);

    QString SolutionName;

    QVector<QString> IndicatorsNames;
    QVector<QString> ProjectsNames;

    QVector<QVector<double>> BaseTable;
    QVector<QVector<double>> NormalizedTable;

    QVector<double> HardRatings;
    QVector<double> SoftRatings;

    QVector<QString> PriorityList;

    //legacy
    //-------------------------------
    QVector<int> PrefferedMetrics;
    QVector<int> RejectedMetrics;
    //-------------------------------


    //In work
    //-------------------------------
    QString ProjectsImportanceNotParsed;
    QString IndicatorsImportanceNotParsed;
    //-------------------------------

    double CrushingStep;


};




#endif // SOLUTION_H
