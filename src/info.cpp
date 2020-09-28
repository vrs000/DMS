#include "info.h"
#include "ui_info.h"

void Info::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void Info::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

Info::Info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);


    this->setWindowFlag(Qt::FramelessWindowHint);
    ui->Step->setText(QString("Шаг дробления: %1").arg(DataProcessing::CrushingStep));
    ui->ProjectsCount->setText(QString("Количество проектов: %1").arg(IO::ProjectsNames.size()));
    ui->IterationsCount->setText(QString("Количество итераций: %1").arg(DataProcessing::WeightsTable.size() - DataProcessing::missed_variation));

    QString priority;

    for (int i=0; i<IO::IndicatorsNames.size(); i++)
    {
        priority = DataProcessing::PriorityList[i] == "max" ? "Больше - лучше" : "Меньше - лучше";
        QLabel *label = new QLabel();

        label->setText(QString("%1: %2").arg(IO::IndicatorsNames[i]).arg(priority));
        ui->layout->addWidget(label);
    }

    for (int i=0; i < DataProcessing::PrefferedMetrics.size(); i++)
    {
        for (int j = 0; j < DataProcessing::RejectedMetrics.size(); j++)
        {
            int p = DataProcessing::PrefferedMetrics[i];
            int r = DataProcessing::RejectedMetrics[j];

            QLabel *label = new QLabel();
            label->setText(QString("%1 > %2").arg(IO::IndicatorsNames[p]).arg(IO::IndicatorsNames[r]));
            ui->layout->addWidget(label);
        }
    }
}

Info::~Info()
{
    delete ui;
}

void Info::Set()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    ui->Step->setText(QString("Шаг дробления: %1").arg(DataProcessing::CrushingStep));
    ui->ProjectsCount->setText(QString("Количество проектов: %1").arg(IO::ProjectsNames.size()));
    ui->IterationsCount->setText(QString("Количество итераций: %1").arg(DataProcessing::WeightsTable.size() - DataProcessing::missed_variation));

    QString priority;

    for (int i=0; i<IO::IndicatorsNames.size(); i++)
    {
        priority = DataProcessing::PriorityList[i] == "max" ? "Больше - лучше" : "Меньше - лучше";
        QLabel *label = new QLabel();

        label->setText(QString("%1: %2").arg(IO::IndicatorsNames[i]).arg(priority));
        ui->layout->addWidget(label);
    }

    for (int i=0; i < DataProcessing::PrefferedMetrics.size(); i++)
    {
        for (int j = 0; j < DataProcessing::RejectedMetrics.size(); j++)
        {
            int p = DataProcessing::PrefferedMetrics[i];
            int r = DataProcessing::RejectedMetrics[j];

            QLabel *label = new QLabel();
            //label->setAlignment(Qt::AlignCenter);
            label->setText(QString("%1 > %2").arg(IO::IndicatorsNames[p]).arg(IO::IndicatorsNames[r]));
            ui->layout->addWidget(label);
        }
    }
}

void Info::on_CloseButton_clicked()
{
    this->close();
}
