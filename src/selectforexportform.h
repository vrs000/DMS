#ifndef SELECTFOREXPORTFORM_H
#define SELECTFOREXPORTFORM_H

#include <QWidget>
#include "solutiondb.h"
#include <QCheckBox>
namespace Ui {
class SelectForExportForm;
}

class SelectForExportForm : public QWidget
{
    Q_OBJECT

public:
    explicit SelectForExportForm(QWidget *parent = nullptr);
    ~SelectForExportForm();

private:
    QList<QCheckBox*> checkboxesList;
    QString path;


private slots:
    void on_CancelButton_clicked();

    void on_OkButton_clicked();

private:
    Ui::SelectForExportForm *ui;
};

#endif // SELECTFOREXPORTFORM_H
