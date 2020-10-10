#include "startupconfigform.h"
#include "ui_startupconfigform.h"

StartupConfigForm::StartupConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupConfigForm)
{
    ui->setupUi(this);


    SetPrioritiesList(IO::IndicatorsNames);
    SetMetricsLists(IO::IndicatorsNames);
    SetProjectsLists(IO::ProjectsNames);


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

void StartupConfigForm::SetProjectsLists(QVector<QString> projectsNames)
{
    foreach (QString param, projectsNames)
    {
        QCheckBox* pref = new QCheckBox(param);
        QCheckBox* rejec = new QCheckBox(param);


        PreferredProjectsCheckBoxes << pref;
        RejectedProjectsCheckBoxes<< rejec;


        ui->PreffredProjectsLayout->addWidget(pref);
        ui->RejectedProjectsLayout->addWidget(rejec);
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

    QVector<QString> prefferedProjects;
    QVector<QString> rejectedProjects;

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

    for (int i = 0; i < PreferredProjectsCheckBoxes.size(); i++)
        if (PreferredProjectsCheckBoxes[i]->isChecked())
            prefferedProjects << PreferredProjectsCheckBoxes[i]->text();

    for (int i = 0; i < RejectedProjectsCheckBoxes.size(); i++)
        if (RejectedProjectsCheckBoxes[i]->isChecked())
            rejectedProjects << RejectedProjectsCheckBoxes[i]->text();
    //===========================================


    qDebug() << prefferedProjects;
    qDebug() << rejectedProjects;

    int m = IO::IndicatorsNames.size();
    int K = std::floor(DataProcessing::Fact(std::floor(1 / ui->StepSpinBox->value()) + m - 1)
                       / (DataProcessing::Fact(m - 1) * DataProcessing::Fact(std::floor(1 / ui->StepSpinBox->value()))));



    DataProcessing::CurrentIterationCount = K;



    DataProcessing::CrushingStep = ui->StepSpinBox->value();
    DataProcessing::OpenedSolutionName = solutionName;
    DataProcessing::MakeCalculations(priority, prefered, rejected, prefferedProjects, rejectedProjects);
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

void StartupConfigForm::on_ToggleProjectsGroupCheckBox_stateChanged(int arg1)
{
    ui->ImportanceProjectsGroups_groupBox->setEnabled(ui->ToggleProjectsGroupCheckBox->isChecked());
}
