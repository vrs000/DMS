#include "progressbarform.h"
#include "ui_progressbarform.h"

ProgressBarForm::ProgressBarForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressBarForm)
{
    ui->setupUi(this);

    bar = ui->progressBar;
}

ProgressBarForm::~ProgressBarForm()
{
    delete ui;
}
