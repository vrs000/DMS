#include "loadingform.h"
#include "ui_loadingform.h"

LoadingForm::LoadingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadingForm)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);
    QMovie *movie = new QMovie( ":/icons/WaitBar.gif" );
    ui->label->setMovie( movie );
    movie->start();
}

LoadingForm::~LoadingForm()
{
    delete ui;
}
