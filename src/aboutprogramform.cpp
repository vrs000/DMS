#include "aboutprogramform.h"
#include "ui_aboutprogramform.h"

void AboutProgramForm::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void AboutProgramForm::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

AboutProgramForm::AboutProgramForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutProgramForm)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
}

AboutProgramForm::~AboutProgramForm()
{
    delete ui;
}

void AboutProgramForm::on_CloseButton_clicked()
{
    close();
}
