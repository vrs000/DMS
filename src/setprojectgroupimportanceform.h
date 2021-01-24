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
#include <QListWidget>
#include <QDialog>

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

class SetProjectGroupImportanceForm : public QDialog
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
    QVector<QComboBox*> comboxes;
    QVector<QListWidget*> groupsList;
    //----------------------------------------

private slots:
    void on_OkButton_clicked();
    void on_CancelButton_clicked();

private:
    Ui::SetProjectGroupImportanceForm *ui;

    void SetStartupProjects();
    void SetStartupIndicators();

};

#endif // SETPROJECTGROUPIMPORTANCEFORM_H
