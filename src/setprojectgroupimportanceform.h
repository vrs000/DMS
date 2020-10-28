#ifndef SETPROJECTGROUPIMPORTANCEFORM_H
#define SETPROJECTGROUPIMPORTANCEFORM_H

#include <QWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <dataprocessing.h>
#include <QScrollArea>
#include <QGridLayout>
#include <QPushButton>
enum StartupConfig
{
    Projects,
    Indicators
};

namespace Ui {
class SetProjectGroupImportanceForm;
}

class IO;
class DataProcessing;
class StartupConfigForm;

class SetProjectGroupImportanceForm : public QWidget
{
    Q_OBJECT

public:
    explicit SetProjectGroupImportanceForm(QWidget *parent = nullptr, StartupConfig config = Projects);
    ~SetProjectGroupImportanceForm();

    QPushButton* btnProjects;
    QPushButton* btnIndicatators;

private slots:
    void on_AddGroupButton_clicked();

    void on_RemoveGroupButton_clicked();


private:
    StartupConfig Config;


    QString GetImportanceGroupString();

    //Containers
    //----------------------------------------
    QStringList checkedList;
    QVector<QComboBox*> comboxes;
    QVector<QVector<QCheckBox*>> checkboxes;
    QVector<QVBoxLayout*> VLayouts;
    QVector<QScrollArea*> scrollAreas;
    QVector<QWidget*> widgets;
    //----------------------------------------

private slots:
    void Checked(int state);
    void UpdateVisibility();

    void on_OkButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::SetProjectGroupImportanceForm *ui;
    void UpdateCheckBoxesVisibility();
    void SetStartupProjects();
    void SetStartupIndicators();

};

#endif // SETPROJECTGROUPIMPORTANCEFORM_H
