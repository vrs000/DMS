#include "solutiondb.h"
QList<Solution> SolutionDB::DB;

QString SolutionDB::currentSolutionName = "";

void SolutionDB::AddSolution(Solution solution)
{
    currentSolutionName = solution.SolutionName;
    DB << solution;
}

void SolutionDB::RemoveSolution(Solution solution)
{
    for (int i=0; i < DB.size(); i++)
    {
        if (DB[i].SolutionName == solution.SolutionName)
        {
            DB.removeAt(i);
            break;
        }
    }
}

void SolutionDB::RemoveSolution(QString solutionName)
{
    for (int i=0; i < DB.size(); i++)
    {
        if (DB[i].SolutionName == solutionName)
        {
            DB.removeAt(i);
            break;
        }
    }
}

void SolutionDB::UpdateSolution(Solution OldSolution, Solution NewSolution)
{
    //??????????????????????????????????

    for (int i = 0; i < DB.size(); i++)
    {
        if (DB[i].SolutionName == OldSolution.SolutionName)
        {
            DB[i] = NewSolution;
            break;
        }
    }
}

bool SolutionDB::IsContained(Solution solution)
{
    for (auto sol : DB)
    {
        if (sol.SolutionName == solution.SolutionName)
            return true;
    }

    return false;
}

bool SolutionDB::IsContained(QString solutionName)
{
    for (auto sol : DB)
    {
        if (sol.SolutionName == solutionName)
            return true;
    }

    return false;
}

void SolutionDB::LoadSolution(const QString solutionName)
{
    Solution solution;

    for (auto sol : DB)
    {
        if (sol.SolutionName == solutionName)
        {
            currentSolutionName = sol.SolutionName;

            IO::IndicatorsNames = sol.IndicatorsNames;
            IO::ProjectsNames = sol.ProjectsNames;
            IO::BaseTable = sol.BaseTable;

            DataProcessing::CrushingStep = sol.CrushingStep;

            DataProcessing::NormalizedTable = sol.NormalizedTable;

            DataProcessing::HardRatings = sol.HardRatings;
            DataProcessing::SoftRatings = sol.SoftRatings;

            DataProcessing::PriorityList = sol.PriorityList;

            DataProcessing::PrefferedMetrics = sol.PrefferedMetrics;
            DataProcessing::RejectedMetrics = sol.RejectedMetrics;

            break;
        }
    }
}

void SolutionDB::LoadSolution(const Solution& solution)
{
    for (auto sol : DB)
    {
        if (sol.SolutionName == solution.SolutionName)
        {
            currentSolutionName = sol.SolutionName;

            IO::IndicatorsNames = sol.IndicatorsNames;
            IO::ProjectsNames = sol.ProjectsNames;
            IO::BaseTable = sol.BaseTable;

            DataProcessing::CrushingStep = sol.CrushingStep;

            DataProcessing::NormalizedTable = sol.NormalizedTable;

            DataProcessing::HardRatings = sol.HardRatings;
            DataProcessing::SoftRatings = sol.SoftRatings;

            DataProcessing::PriorityList = sol.PriorityList;

            DataProcessing::PrefferedMetrics = sol.PrefferedMetrics;
            DataProcessing::RejectedMetrics = sol.RejectedMetrics;

            break;
        }
    }
}

Solution& SolutionDB::GetSolution(const QString solutionName)
{
    for (int i = 0; i < DB.size(); i++)
    {
        if (DB[i].SolutionName == solutionName)
            return DB[i];
    }

    Solution s;
    return s;
}

