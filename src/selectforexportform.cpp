#include "selectforexportform.h"
#include "ui_selectforexportform.h"

SelectForExportForm::SelectForExportForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectForExportForm)
{
    ui->setupUi(this);

    checkboxesList.clear();

    foreach (auto sol, SolutionDB::DB)
    {
        QCheckBox* checkBox = new QCheckBox(sol.SolutionName);
        ui->ListLayout->addWidget(checkBox);
        checkboxesList << checkBox;
    }

}

SelectForExportForm::~SelectForExportForm()
{
    delete ui;
}

void SelectForExportForm::on_CancelButton_clicked()
{
    close();
}

void SelectForExportForm::on_OkButton_clicked()
{
    QList<Solution> solutions;

    foreach(auto checkBox, checkboxesList)
    {
        if (checkBox->isChecked())
        {
            solutions << (SolutionDB::GetSolution(checkBox->text()));

        }
    }



    IO::IsBuild = ui->BuildChartOnRadioButton->isChecked();
    IO::IsPaint = ui->PaintOnRadioButton->isChecked();

    IO::SaveExcelFile(solutions);
    close();
}
