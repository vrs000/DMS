#ifndef STARTUPCONFIGFORM_H
#define STARTUPCONFIGFORM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <dataprocessing.h>
#include <io.h>
#include <solutiondb.h>
#include <solution.h>
#include <QtMath>
#include <setprojectgroupimportanceform.h>

namespace Ui {
class StartupConfigForm;
}


class SetProjectGroupImportanceForm;
class MainWindow;

class StartupConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit StartupConfigForm(QWidget *parent = nullptr);
    ~StartupConfigForm();

    const QString max = "Больше - лучше";
    const QString min = "Меньше - лучше";


    void SetPrioritiesList(QVector<QString> indicatorsNames);
    void SetMetricsLists(QVector<QString> indicatorsNames);
    void SetProjectsLists(QVector<QString> projectsNames);

    QList<QComboBox*> PrioritiesComboBoxes;

    QList<QCheckBox*> PreferredCheckBoxes;
    QList<QCheckBox*> RejectedCheckBoxes;

    QList<QCheckBox*> PreferredProjectsCheckBoxes;
    QList<QCheckBox*> RejectedProjectsCheckBoxes;

    QString solutionName;
    QTableWidget* input;
    QTableWidget* output;


    static long double fact(int N);

    static QString NotParsedImportanceGroupOfProjects;
    static QString NotParsedImportanceGroupOfIndicators;


private slots:
    void SetProgressBarValue(int value, int maximum);

    void on_ToggleGroupCheckBox_stateChanged(int arg1);

    void on_CancelButton_clicked();

    void on_OkButton_clicked();

    void on_StepSpinBox_valueChanged(double arg1);

    void on_progressBar_valueChanged(int value);

    void on_ToggleProjectsGroupCheckBox_stateChanged(int arg1);

    void on_UpButton_clicked();

    void on_DownButton_clicked();

    void on_SetProjectImportanceGroupsBtn_clicked();

    void on_SetIndicatorImportanceGroupsBtn_clicked();

private:
    Ui::StartupConfigForm *ui;

};

#endif // STARTUPCONFIGFORM_H