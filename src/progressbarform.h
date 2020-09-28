#ifndef PROGRESSBARFORM_H
#define PROGRESSBARFORM_H

#include <QWidget>
#include <QProgressBar>

namespace Ui {
class ProgressBarForm;
}

class ProgressBarForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBarForm(QWidget *parent = nullptr);
    ~ProgressBarForm();

    QProgressBar* bar;

private:
    Ui::ProgressBarForm *ui;
};

#endif // PROGRESSBARFORM_H
