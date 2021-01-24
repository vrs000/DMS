#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QThread>
#include <QDialog>
#include <QDebug>
#include <dataprocessing.h>

namespace Ui {
class SettingsForm;
}

class DataProcessing;
class SettingsForm : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();

private slots:
    void on_MaxThreadCountSlider_sliderMoved(int position);

    void on_CurrentThreadCountSlider_sliderMoved(int position);

    void on_CancelButton_clicked();

    void on_OkButton_clicked();

private:
    Ui::SettingsForm *ui;
};

#endif // SETTINGSFORM_H
