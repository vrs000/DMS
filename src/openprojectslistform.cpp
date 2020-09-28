#include "openprojectslistform.h"
#include "ui_openprojectslistform.h"

QTableWidget* OpenProjectsListForm::input = nullptr;
QTableWidget* OpenProjectsListForm::output = nullptr;



OpenProjectsListForm::OpenProjectsListForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenProjectsListForm)
{
    ui->setupUi(this);

    radioBtnList.clear();

    QRadioButton* radioButton;

    for (auto sol : SolutionDB::DB)
    {
        radioButton = new QRadioButton(sol.SolutionName);
        radioBtnList << radioButton;
        ui->ListLayout->addWidget(radioButton);
    }



}

OpenProjectsListForm::~OpenProjectsListForm()
{
    delete ui;
}

void OpenProjectsListForm::on_LoadButton_clicked()
{
    for (auto btn : radioBtnList)
    {
        if (btn->isChecked())
        {
            SolutionDB::LoadSolution(btn->text());
            IO::FillingTables(input, output);
            break;
        }
    }

    close();
}

void OpenProjectsListForm::on_CancelButton_clicked()
{
    close();
}
