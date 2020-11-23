#include "startupconfigform.h"
#include "ui_startupconfigform.h"

QString StartupConfigForm::NotParsedImportanceGroupOfIndicators = "";
QString StartupConfigForm::NotParsedImportanceGroupOfProjects = "";
bool StartupConfigForm::IsSuccessFinished = true;



StartupConfigForm::StartupConfigForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupConfigForm)
{
    ui->setupUi(this);

    IsSuccessFinished = true;

    NotParsedImportanceGroupOfIndicators = "";
    NotParsedImportanceGroupOfProjects = "";

    SetPrioritiesList(IO::IndicatorsNames);
    SetMetricsLists(IO::IndicatorsNames);
    SetProjectsLists(IO::ProjectsNames);

    DataProcessing::ParettoSetProjects.clear();

    DataProcessing::bar = ui->progressBar;

    ui->progressBar->hide();

    ui->ParretoCountInfoLabel->setText("");
    ui->StepValueTextEdit_1->setText(QString::number(DataProcessing::CrushingStep));


    int K = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), DataProcessing::CrushingStep);

    if (K < 0)
    {
        ui->IterationCountLabel_2->setText(QString("Уменьшите шаг"));
        ui->OkButton->setEnabled(false);
    }
    else
    {
        ui->IterationCountLabel_2->setText(QString("Количество итераций: %1").arg(K));
        ui->OkButton->setEnabled(true);
    }
}

StartupConfigForm::~StartupConfigForm()
{
    delete ui;
}

void StartupConfigForm::SetMetricsLists(QVector<QString> indicatorsNames)
{
    foreach (QString param, indicatorsNames)
    {
        QCheckBox* pref = new QCheckBox(param);
        QCheckBox* rejec = new QCheckBox(param);

        PreferredCheckBoxes << pref;
        RejectedCheckBoxes << rejec;
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

        connect(comboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(ComboBoxChanged()));

        layout->addWidget(nameLabel);
        layout->addWidget(comboBox);

        ui->PriorityLayout->addWidget(widget);

        PrioritiesComboBoxes << comboBox;
    }

}

void StartupConfigForm::on_CancelButton_clicked()
{
    IsSuccessFinished = false;
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


    //Считывание данных с ??чекбоксов?? и комбобоксов
    //===========================================
    for (auto p : PrioritiesComboBoxes)
        priority << (p->currentText() == max ? _max : _min);


    //LEGACY???????????????????????
    //------------------------------------------------------------------
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
    //------------------------------------------------------------------

    //===========================================


    DataProcessing::CrushingStep = ui->StepValueTextEdit_1->text().toDouble();
    double h = DataProcessing::CrushingStep;
    int m = IO::IndicatorsNames.size();
    int K = std::floor(DataProcessing::Fact(std::floor(1 /h) + m - 1)
                       / (DataProcessing::Fact(m - 1) * DataProcessing::Fact(std::floor(1 / h))));


    DataProcessing::CurrentIterationCount = K;


    //Задание групп важности
    //-------------------------------------------------------------------------------------------
    DataProcessing::NotParsedImportanceGroupOfIndicators = NotParsedImportanceGroupOfIndicators;
    DataProcessing::NotParsedImportanceGroupOfProjects = NotParsedImportanceGroupOfProjects;
    //-------------------------------------------------------------------------------------------


    DataProcessing::CrushingStep = ui->StepValueTextEdit_1->text().toDouble();
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

void StartupConfigForm::ComboBoxChanged()
{
    if (ui->ParettoOnBtn->isChecked())
        on_ParettoOnBtn_clicked();
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
    DataProcessing::CurrentIterationCount = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), arg1);
    int K = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), arg1);

    if (K < 0)
    {
        ui->OkButton->setEnabled(false);
    }
    else
    {
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


void StartupConfigForm::on_UpButton_clicked()
{
    QList<double> list = {0.001, 0.002, 0.004, 0.005, 0.008, 0.01, 0.02, 0.025, 0.04, 0.05, 0.1, 0.125, 0.2, 0.25, 0.5};

    double step =  ui->StepValueTextEdit_1->text().toDouble();

    int index = list.indexOf(step);

    if (index != list.size() - 1)
    {
        ui->StepValueTextEdit_1->setText(QString::number(list[index+1]));
        DataProcessing::CurrentIterationCount = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), list[index+1]);
        int K = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), list[index+1]);

        if (K < 0)
        {
            ui->IterationCountLabel_2->setText(QString("Уменьшите шаг"));
            ui->OkButton->setEnabled(false);
        }
        else
        {
            ui->IterationCountLabel_2->setText(QString("Количество итераций: %1").arg(K));
            ui->OkButton->setEnabled(true);
        }
    }
}

void StartupConfigForm::on_DownButton_clicked()
{
    QList<double> list = {0.001, 0.002, 0.004, 0.005, 0.008, 0.01, 0.02, 0.025, 0.04, 0.05, 0.1, 0.125, 0.2, 0.25, 0.5};

    double step =  ui->StepValueTextEdit_1->text().toDouble();
    int index = list.indexOf(step);


    if (index != 0)
    {
        if (index == -1) index = 1;
        ui->StepValueTextEdit_1->setText(QString::number(list[index-1]));
        DataProcessing::CurrentIterationCount = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), list[index-1]);
        int K = DataProcessing::GetTheoreticalWeightsCount(IO::IndicatorsNames.size(), list[index-1]);

        if (K < 0)
        {
            ui->IterationCountLabel_2->setText(QString("Уменьшите шаг"));
            ui->OkButton->setEnabled(false);
        }
        else
        {
            ui->IterationCountLabel_2->setText(QString("Количество итераций: %1").arg(K));
            ui->OkButton->setEnabled(true);
        }
    }
}

void StartupConfigForm::on_SetProjectImportanceGroupsBtn_clicked()
{
    SetProjectGroupImportanceForm *form = new SetProjectGroupImportanceForm(this, Projects);
    form->btnProjects = ui->SetProjectImportanceGroupsBtn;
    form->setModal(true);
    form->exec();
}

void StartupConfigForm::on_SetIndicatorImportanceGroupsBtn_clicked()
{
    SetProjectGroupImportanceForm *form = new SetProjectGroupImportanceForm(nullptr, Indicators);
    form->btnIndicatators = ui->SetIndicatorImportanceGroupsBtn;
    form->setModal(true);
    form->exec();
}

void StartupConfigForm::on_ParettoOffBtn_clicked()
{
    ui->ParretoCountInfoLabel->setText("");
    ui->ParretoCountInfoLabel->setStyleSheet("");

    DataProcessing::ParettoSetProjects.clear();
    DataProcessing::ParettoSetProjectsIndexes.clear();
    DataProcessing::NormalizedTable.clear();
}

void StartupConfigForm::on_ParettoOnBtn_clicked()
{
    const double ThresholdPercent = 40;
    const QString _max= "max";
    const QString _min= "min";


    QVector<QString> priority;
    for (auto p : PrioritiesComboBoxes)
        priority << (p->currentText() == max ? _max : _min);


    DataProcessing::SetPriorityList(priority);
    DataProcessing::FindMaxMinIndicators(IO::BaseTable);
    DataProcessing::CalculateNormalizedTable(IO::BaseTable);
    DataProcessing::FindParettoSet();


    int RealProjectsCount = IO::ProjectsNames.size();
    int ParretoSetProjectsCount = DataProcessing::ParettoSetProjects.size();
    int percent = round(ParretoSetProjectsCount * 100.0/(RealProjectsCount*1.0));



    ui->ParretoCountInfoLabel->setText(QString("%1/%2 (%3%)")
                                       .arg(ParretoSetProjectsCount)
                                       .arg(RealProjectsCount)
                                       .arg(percent));


    //Not needed ????????????????????
    //-----------------------------------------------------------------------------------
    foreach (auto project, DataProcessing::ParettoSetProjects)
        DataProcessing::ParettoSetProjectsIndexes << IO::ProjectsNames.indexOf(project);

    qDebug() << DataProcessing::ParettoSetProjectsIndexes;
    //-----------------------------------------------------------------------------------
}
