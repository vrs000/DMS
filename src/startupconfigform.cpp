#include "startupconfigform.h"
#include "ui_startupconfigform.h"

StartupConfigForm::StartupConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupConfigForm)
{
    ui->setupUi(this);


    SetPrioritiesList(IO::IndicatorsNames);
    SetMetricsLists(IO::IndicatorsNames);

    DataProcessing::bar = ui->progressBar;
    ui->progressBar->hide();
    ui->StepSpinBox->setValue(DataProcessing::CrushingStep);

    int m = IO::IndicatorsNames.size();
    uint K = fact(m + 1/ui->StepSpinBox->value() - 1)/fact(1/ui->StepSpinBox->value())/fact(m-1);
    ui->IterationCountLabel->setText(QString("Количество итераций: %1").arg(K));

}

StartupConfigForm::~StartupConfigForm()
{
    delete ui;
}

void StartupConfigForm::on_ToggleGroupCheckBox_stateChanged(int arg1)
{
    ui->ImportanceGroups_groupBox->setEnabled(ui->ToggleGroupCheckBox->isChecked());
}

void StartupConfigForm::SetMetricsLists(QVector<QString> indicatorsNames)
{
    foreach (QString param, indicatorsNames)
    {
        QCheckBox* pref = new QCheckBox(param);
        QCheckBox* rejec = new QCheckBox(param);



        PreferredCheckBoxes << pref;
        RejectedCheckBoxes << rejec;

        ui->PreffredLayout->addWidget(pref);
        ui->RejectedLayout->addWidget(rejec);
    }
}

void StartupConfigForm::SetPrioritiesList(QVector<QString> indicatorsNames)
{


    foreach (QString param, indicatorsNames)
    {

        QHBoxLayout* layout = new QHBoxLayout();

        QWidget* widget = new QWidget;
        widget->setLayout(layout);

        QLabel* nameLabel =new QLabel(param);

        QComboBox* comboBox = new QComboBox();
        comboBox->addItem(max);
        comboBox->addItem(min);

        layout->addWidget(nameLabel);
        layout->addWidget(comboBox);

        ui->PriorityLayout->addWidget(widget);

        PrioritiesComboBoxes << comboBox;
    }

}

void StartupConfigForm::on_CancelButton_clicked()
{
    close();
}

void StartupConfigForm::on_OkButton_clicked()
{
    const QString _max= "max";
    const QString _min= "min";

    QVector<QString> priority;
    QVector<int> prefered;
    QVector<int> rejected;

    //Считывание данных с чекбоксов и комбобоксов
    //===========================================
    for (auto p : PrioritiesComboBoxes)
        priority << (p->currentText() == max ? _max : _min);

    for (int i = 0; i < PreferredCheckBoxes.size(); i++)
        if (PreferredCheckBoxes[i]->isChecked())
            prefered << i;

    for (int i = 0; i < RejectedCheckBoxes.size(); i++)
        if (RejectedCheckBoxes[i]->isChecked())
            rejected << i;
    //===========================================


    DataProcessing::CrushingStep = ui->StepSpinBox->value();
    DataProcessing::OpenedSolutionName = solutionName;
    DataProcessing::MakeCalculations(priority, prefered, rejected);
    DataProcessing::bar = ui->progressBar;
    setEnabled(false);
//    IO::FillingTables(input, output);

//    Solution solution(
//                solutionName, DataProcessing::CrushingStep,
//                IO::IndicatorsNames, IO::ProjectsNames,
//                IO::BaseTable, DataProcessing::NormalizedTable,
//                DataProcessing::HardRatings, DataProcessing::SoftRatings,
//                DataProcessing::PriorityList,
//                DataProcessing::PrefferedMetrics, DataProcessing::RejectedMetrics
//                );


//    //?????????????????????????????????????????????????????????
//    if (SolutionDB::IsContained(solutionName))
//    {
//        auto oldSolution = SolutionDB::GetSolution(solutionName);
//        SolutionDB::UpdateSolution(oldSolution, solution);
//        qDebug() << "Solution updated";
//    }
//    else
//    {
//        SolutionDB::AddSolution(solution);
//    }
//    //?????????????????????????????????????????????????????????


    //SolutionDB::AddSolution(solution);
   // close();
}

long double StartupConfigForm::fact(int N)
{
    if (N < 0) return 0;
    if (N == 0) return 1;
    else
        return N * fact(N - 1);
}

void StartupConfigForm::SetProgressBarValue(int value, int maximum)
{
    Q_UNUSED(value);
    Q_UNUSED(maximum);

    if (ui->progressBar->isHidden())
        ui->progressBar->show();

}

void StartupConfigForm::on_StepSpinBox_valueChanged(double arg1)
{

    int m = IO::IndicatorsNames.size();
    uint K = fact(m + 1/arg1 - 1)/fact(1/arg1)/fact(m-1);
    ui->IterationCountLabel->setText(QString("Количество итераций: %1").arg(K));
}

void StartupConfigForm::on_progressBar_valueChanged(int value)
{
    if (value == ui->progressBar->maximum())
    {
        close();
    }
}
