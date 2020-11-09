#ifndef SELECTFOREXPORTFORM_H
#define SELECTFOREXPORTFORM_H

#include <QWidget>
#include "solutiondb.h"
#include <QCheckBox>
#include <QDialog>

namespace Ui {
class SelectForExportForm;
}
class MainWindow;

class SelectForExportForm : public QDialog
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
