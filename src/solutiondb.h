#ifndef SOLUTIONDB_H
#define SOLUTIONDB_H

#include <QList>
#include <solution.h>
#include <io.h>
#include <dataprocessing.h>


class SolutionDB
{
public:
    static QList<Solution> DB;

    static QString currentSolutionName;

    static void AddSolution(Solution solution);
    static void RemoveSolution(Solution solution);
    static void RemoveSolution(QString solutionName);
    static void UpdateSolution(Solution OldSolution, Solution NewSolution);


    static bool IsContained(Solution solution);
    static bool IsContained(QString solutionName);


    static void LoadSolution(const QString solutionName);
    static void LoadSolution(const Solution& solution);
    static Solution& GetSolution(const QString solutionName);
};

#endif // SOLUTIONDB_H
