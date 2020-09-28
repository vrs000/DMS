#ifndef PRESELECTIONCHARTSFORM_H
#define PRESELECTIONCHARTSFORM_H

#include <QWidget>
#include <solutiondb.h>
#include <QCheckBox>
#include <chartsform.h>
class ChartsForm;






namespace Ui {
class PreSelectionChartsForm;
}

class PreSelectionChartsForm : public QWidget
{
    Q_OBJECT

public:
    explicit PreSelectionChartsForm(QWidget *parent = nullptr);
    ~PreSelectionChartsForm();

public:
    static QList<Solution> SelectedSolutions;;

private slots:
    void on_CancelButton_clicked();

    void on_OkButton_clicked();

private:
    void MakeView();
    void OpenCharts();
    QList<QCheckBox*> checkBoxes;


    QList<ChartsForm*> l;
    std::unique_ptr<ChartsForm> chartsFormPtr;

private:
    Ui::PreSelectionChartsForm *ui;
};

#endif // PRESELECTIONCHARTSFORM_H
