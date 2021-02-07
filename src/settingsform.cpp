#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    ui->MaxThreadCountSlider->setMaximum(QThread::idealThreadCount());
    ui->MaxThreadCountSlider->setValue(DataProcessing::MaxThreadCount);
    ui->MaxThreadCountLabel->setText(QString::number(DataProcessing::MaxThreadCount));

    ui->CurrentThreadCountSlider->setMaximum(DataProcessing::MaxThreadCount);
    ui->CurrentThreadCountSlider->setValue(DataProcessing::CurrentThreadCount == -1 ? 0 : DataProcessing::CurrentThreadCount);
    ui->CurrentThreadCountLabel->setText(DataProcessing::CurrentThreadCount == -1 ? "авто" : QString::number(DataProcessing::CurrentThreadCount));

    ui->TextLabel->hide();
    ui->TextLabel_2->hide();

    ui->MaxIterCountEdit->setText(QString::number(DataProcessing::MaxIterCountForLogging));
    ui->LoggingCheckBox->setChecked(DataProcessing::IsLoggingUsed);

    if (ui->LoggingCheckBox->isChecked())
    {
        ui->TextLabel_3->show();
        ui->TextLabel_4->show();
    }
    else
    {
        ui->TextLabel_3->hide();
        ui->TextLabel_4->hide();
    }
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_MaxThreadCountSlider_sliderMoved(int position)
{
    ui->MaxThreadCountLabel->setText(QString::number(position));
    ui->CurrentThreadCountSlider->setMaximum(position);
    ui->CurrentThreadCountLabel->setText(ui->CurrentThreadCountSlider->value() ? QString::number(position) : "авто");
}

void SettingsForm::on_CurrentThreadCountSlider_sliderMoved(int position)
{
    ui->CurrentThreadCountLabel->setText(position ? QString::number(position) : "авто");

    if (position == QThread::idealThreadCount())
    {
        ui->TextLabel->show();
        ui->TextLabel_2->show();
    }
    else
    {
        ui->TextLabel->hide();
        ui->TextLabel_2->hide();
    }
}

void SettingsForm::on_CancelButton_clicked()
{
    close();
}

void SettingsForm::on_OkButton_clicked()
{
    int MaxCount = ui->MaxThreadCountSlider->value();
    int CurrentCount = ui->CurrentThreadCountSlider->value();
    int itercount = ui->MaxIterCountEdit->text().toInt();

    DataProcessing::MaxThreadCount = MaxCount;
    DataProcessing::CurrentThreadCount = CurrentCount ? CurrentCount : -1;
    DataProcessing::MaxIterCountForLogging = itercount;
    DataProcessing::IsLoggingUsed = ui->LoggingCheckBox->isChecked();

    close();
}

void SettingsForm::on_LoggingCheckBox_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        ui->MaxIterCountEdit->setEnabled(true);
        ui->TextLabel_3->show();
        ui->TextLabel_4->show();
    }


    if (arg1 == 0)
    {
        ui->MaxIterCountEdit->setEnabled(false);
        ui->TextLabel_3->hide();
        ui->TextLabel_4->hide();
    }
}
