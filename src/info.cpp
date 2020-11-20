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
    ui->IterationsCount->setText(QString("Количество итераций: %1").arg(DataProcessing::CurrentIterationCount - DataProcessing::missed_variation));


    //Производительность
    //----------------------------------------------------------------------------------------------
    QLabel *label1 = new QLabel(QString("Потоков использовано: %1").arg(DataProcessing::UsedThreadCount));
    QLabel *label2 = new QLabel(QString("Затраченное время: %1 мс").arg(DataProcessing::TimeElapsed));

    ui->layout->addWidget(label1);
    ui->layout->addWidget(label2);
    //----------------------------------------------------------------------------------------------


    //Приоритеты
    //----------------------------------------------------------------------------------------------
    QString priority;
    for (int i=0; i<IO::IndicatorsNames.size(); i++)
    {
        priority = DataProcessing::PriorityList[i] == "max" ? "Больше - лучше" : "Меньше - лучше";
        QLabel *label = new QLabel();

        label->setText(QString("%1: %2").arg(IO::IndicatorsNames[i]).arg(priority));
        ui->layout->addWidget(label);
    }
    //----------------------------------------------------------------------------------------------


    //Группы важности


    //legacy
    //##############################################################################################
    //##############################################################################################
    //##############################################################################################

    // Приоритеты показателей
    //----------------------------------------------------------------------------------------------
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
    //----------------------------------------------------------------------------------------------


    // Приоритеты проектов
    //----------------------------------------------------------------------------------------------
    for (int i=0; i < DataProcessing::PrefferedProjects.size(); i++)
    {
        for (int j = 0; j < DataProcessing::RejectedProjects.size(); j++)
        {
            QString p = DataProcessing::PrefferedProjects[i];
            QString r = DataProcessing::RejectedProjects[j];

            QLabel *label = new QLabel();
            label->setText(QString("%1 > %2").arg(p).arg(r));
            ui->layout->addWidget(label);
        }
    }
    //----------------------------------------------------------------------------------------------
    //##############################################################################################
    //##############################################################################################
    //##############################################################################################




    QStringList projectsImportance = DataProcessing::NotParsedImportanceGroupOfProjects.split(",");
    QStringList indicatorsImportance = DataProcessing::NotParsedImportanceGroupOfIndicators.split(",");




    int first;
    int second;

    //Список приоритетов проектов
    //------------------------------------------------------------------------------------------------------------------
    if (DataProcessing::NotParsedImportanceGroupOfProjects != "")
        for (int i = 0; i < projectsImportance.size(); i++)
        {
            QLabel *label;

            if (projectsImportance[i].contains("<"))
            {
                second = projectsImportance[i].split("<")[0].toInt();
                first = projectsImportance[i].split("<")[1].toInt();
                label = new QLabel(QString("%1>%2").arg(IO::ProjectsNames[first]).arg(IO::ProjectsNames[second]));
            }

            if (projectsImportance[i].contains("≥"))
            {
                first = projectsImportance[i].split("≥")[0].toInt();
                second = projectsImportance[i].split("≥")[1].toInt();
                label = new QLabel(QString("%1≥%2").arg(IO::ProjectsNames[first]).arg(IO::ProjectsNames[second]));
            }

            if (projectsImportance[i].contains(">"))
            {
                first = projectsImportance[i].split(">")[0].toInt();
                second = projectsImportance[i].split(">")[1].toInt();
                label = new QLabel(QString("%1≥%2").arg(IO::ProjectsNames[first]).arg(IO::ProjectsNames[second]));
            }

            ui->layout->addWidget(label);
        }
    //------------------------------------------------------------------------------------------------------------------


    //Список приоритетов показателей
    //------------------------------------------------------------------------------------------------------------------
    if (DataProcessing::NotParsedImportanceGroupOfIndicators != "")
        for (int i = 0; i < indicatorsImportance.size(); i++)            
        {
            QLabel *label;
            if (indicatorsImportance[i].contains("<"))
            {
                second = indicatorsImportance[i].split("<")[0].toInt();
                first = indicatorsImportance[i].split("<")[1].toInt();
            }


            if (indicatorsImportance[i].contains(">"))
            {
                first = indicatorsImportance[i].split(">")[0].toInt();
                second = indicatorsImportance[i].split(">")[1].toInt();
                label = new QLabel(QString("%1>%2").arg(IO::IndicatorsNames[first]).arg(IO::IndicatorsNames[second]));
            }

            if (indicatorsImportance[i].contains("≥"))
            {
                first = indicatorsImportance[i].split("≥")[0].toInt();
                second = indicatorsImportance[i].split("≥")[1].toInt();
                label = new QLabel(QString("%1≥%2").arg(IO::IndicatorsNames[first]).arg(IO::IndicatorsNames[second]));
            }



            ui->layout->addWidget(label);
        }
    //------------------------------------------------------------------------------------------------------------------

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
