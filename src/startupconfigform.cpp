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

    int K = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), DataProcessing::CrushingStep);

    if (K < 0)
    {
        ui->IterationCountLabel->setText(QString("Уменьшите шаг"));
        ui->OkButton->setEnabled(false);
    }
    else
    {
        ui->IterationCountLabel->setText(QString("Количество итераций: %1").arg(K));
        ui->OkButton->setEnabled(true);
    }
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

    int m = IO::IndicatorsNames.size();
    int K = std::floor(DataProcessing::Fact(std::floor(1 / ui->StepSpinBox->value()) + m - 1)
                       / (DataProcessing::Fact(m - 1) * DataProcessing::Fact(std::floor(1 / ui->StepSpinBox->value()))));

    //    if (K<0)
    //    {
    //        DataProcessing::CrushingStep = 0.3;
    //        DataProcessing::CurrentIterationCount = DataProcessing::GetTheoreticalWeightsCount(m, DataProcessing::CrushingStep);

    //        qDebug() <<":::::" <<DataProcessing::CurrentIterationCount;
    //    }
    //    else
    DataProcessing::CurrentIterationCount = K;



    DataProcessing::CrushingStep = ui->StepSpinBox->value();
    DataProcessing::OpenedSolutionName = solutionName;
    DataProcessing::MakeCalculations(priority, prefered, rejected);
    DataProcessing::bar = ui->progressBar;
    setEnabled(false);
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

    //    int m = IO::IndicatorsNames.size();
    //    int K = std::floor(DataProcessing::Fact(std::floor(1 / arg1) + m - 1)
    //               / (DataProcessing::Fact(m - 1) * DataProcessing::Fact(std::floor(1 / arg1))));
    //    DataProcessing::CurrentIterationCount = K;

    DataProcessing::CurrentIterationCount = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), arg1);
    int K = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), arg1);

    if (K < 0)
    {
        ui->IterationCountLabel->setText(QString("Уменьшите шаг"));
        ui->OkButton->setEnabled(false);
    }
    else
    {
        ui->IterationCountLabel->setText(QString("Количество итераций: %1").arg(K));
        ui->OkButton->setEnabled(true);
    }
}

void StartupConfigForm::on_progressBar_valueChanged(int value)
{
    if (value == ui->progressBar->maximum())
    {
        close();
    }
}
