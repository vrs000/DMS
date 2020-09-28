#ifndef OPENPROJECTSLISTFORM_H
#define OPENPROJECTSLISTFORM_H

#include <QWidget>
#include <QRadioButton>
#include <QSpacerItem>
#include <solutiondb.h>
#include <QList>
#include <mainwindow.h>



namespace Ui {
class OpenProjectsListForm;
}

class OpenProjectsListForm : public QWidget
{
    Q_OBJECT

public:
    static QTableWidget* input;
    static QTableWidget* output;

public:
    explicit OpenProjectsListForm(QWidget *parent = nullptr);
    ~OpenProjectsListForm();

    QList<QRadioButton*> radioBtnList;

private slots:
    void on_LoadButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::OpenProjectsListForm *ui;
};

#endif // OPENPROJECTSLISTFORM_H
