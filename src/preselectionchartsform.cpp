#include "preselectionchartsform.h"
#include "ui_preselectionchartsform.h"


PreSelectionChartsForm::PreSelectionChartsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreSelectionChartsForm)
{
    ui->setupUi(this);

    checkBoxes.clear();
    SelectedSolutions.clear();
    MakeView();
}

PreSelectionChartsForm::~PreSelectionChartsForm()
{
    delete ui;
}

void PreSelectionChartsForm::MakeView()
{
    for (auto solution : SolutionDB::DB)
    {
        QCheckBox* checkbox = new QCheckBox(solution.SolutionName);

        if (SolutionDB::currentSolutionName == solution.SolutionName)
            checkbox->setChecked(true);

        checkBoxes << checkbox;
        ui->ListProjectsLayout->addWidget(checkbox);
    }
}

void PreSelectionChartsForm::OpenCharts()
{
    Top topOut;

    if (ui->Top3RadioButton->isChecked()) topOut = Three;
    if (ui->Top5RadioButton->isChecked()) topOut = Five;
    if (ui->Top10RadioButton->isChecked()) topOut = Ten;
    if (ui->TopAllRadioButton->isChecked()) topOut = All;


    QList<Solution> solutions;

    for (auto box : checkBoxes)
    {
        if (box->isChecked())
        {
            SelectedSolutions << SolutionDB::GetSolution(box->text());
        }
    }




    //    delete chartsForm;
    //    chartsForm = new ChartsForm();
    //    chartsForm->SelectedSolutions = solutions;
    //    chartsForm->top = topOut;
    //    chartsForm->show();
    //    chartsForm->FillingCharts();

    chartsFormPtr = std::unique_ptr<ChartsForm>(new ChartsForm);
    chartsFormPtr->SelectedSolutions = solutions;
    chartsFormPtr->top = topOut;
    chartsFormPtr->show();
    chartsFormPtr->FillingCharts();


//    ChartsForm* form = new ChartsForm();
//    form->SelectedSolutions = solutions;
//    form->top = topOut;
//    form->show();
//    form->FillingCharts();
}

void PreSelectionChartsForm::on_CancelButton_clicked()
{
    close();
}

void PreSelectionChartsForm::on_OkButton_clicked()
{
    OpenCharts();
    close();
}

QList<Solution> PreSelectionChartsForm::SelectedSolutions;
