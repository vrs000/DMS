#include "setprojectgroupimportanceform.h"
#include "ui_setprojectgroupimportanceform.h"

SetProjectGroupImportanceForm::SetProjectGroupImportanceForm(QWidget *parent, StartupConfig config) :
    QDialog(parent),
    ui(new Ui::SetProjectGroupImportanceForm)
{
    ui->setupUi(this);

    Config = config;

    comboxes.clear();
    groupsList.clear();

    ui->ListNamesWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->ListNamesWidget->setDragEnabled(true);
    ui->ListNamesWidget->viewport()->setAcceptDrops(true);
    ui->ListNamesWidget->setDropIndicatorShown(true);
    ui->ListNamesWidget->setDefaultDropAction(Qt::MoveAction);\


    if (config == Projects)
        SetStartupProjects();

    if (config == Indicators)
        SetStartupIndicators();

}

SetProjectGroupImportanceForm::~SetProjectGroupImportanceForm()
{
    delete ui;
}


//Методы для установки начального списка показателей
//Или проектов
//-----------------------------------------------------------
void SetProjectGroupImportanceForm::SetStartupProjects()
{
    if (DataProcessing::ParettoSetProjects.size() == 0)
        foreach (QString project, IO::ProjectsNames)
            ui->ListNamesWidget->addItem(project);
    else
        foreach (QString project, DataProcessing::ParettoSetProjects)
            ui->ListNamesWidget->addItem(project);
}

void SetProjectGroupImportanceForm::SetStartupIndicators()
{
    foreach (QString indicator, IO::IndicatorsNames)
        ui->ListNamesWidget->addItem(indicator);
}
//-----------------------------------------------------------


void SetProjectGroupImportanceForm::on_AddGroupButton_clicked()
{

    if (groupsList.size())
    {
        QComboBox* combobox = new QComboBox(this);
        combobox->addItem(">");
        combobox->addItem("≥");
        combobox->addItem("=");
        combobox->addItem(";");
        comboxes << combobox;
        ui->GroupsLayout->addWidget(combobox);
    }

    QListWidget *listWidget = new QListWidget(this);

    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    listWidget->setDragEnabled(true);
    listWidget->viewport()->setAcceptDrops(true);
    listWidget->setDropIndicatorShown(true);
    listWidget->setDefaultDropAction(Qt::MoveAction);

    groupsList << listWidget;

    ui->GroupsLayout->addWidget(listWidget);


}

void SetProjectGroupImportanceForm::on_RemoveGroupButton_clicked()
{
    if (groupsList.size() == 0) return;

    if (!comboxes.empty())
    {
        delete comboxes.last();
        comboxes.removeLast();
    }

    delete groupsList.last();
    groupsList.removeLast();
}

QString SetProjectGroupImportanceForm::GetImportanceGroupString()
{
    QString result = "";

    /*
        result будет хранить в таком виде

        0>1,1>2,1>5,
    */


    //1 и 2 номер параметра
    int fNumb;
    int sNumb;


    for (int i = 0; i < groupsList.size() - 1; i++)
    {
        //Смотрим первый список чекбоксов(i) и следующий(i+1)
        //А также значение i-го QComboBox

        for (int k = 0; k < groupsList[i]->count(); k++)
        {

            //Если в i-м QComboBox находится ;, то пропус
            if (comboxes[i]->currentText() == ";") continue;


            for (int l = 0; l < groupsList[i+1]->count(); l++)
            {

                if (Config == Projects)
                {
                    //Если выключен критерий паретто
                    if (DataProcessing::ParettoSetProjects.size() == 0)
                    {
                        fNumb = IO::ProjectsNames.indexOf(groupsList[i]->item(k)->text());
                        sNumb = IO::ProjectsNames.indexOf(groupsList[i+1]->item(l)->text());
                    }
                    else
                    {
                        fNumb = DataProcessing::ParettoSetProjects.indexOf(groupsList[i]->item(k)->text());
                        sNumb = DataProcessing::ParettoSetProjects.indexOf(groupsList[i+1]->item(l)->text());
                    }

                    result += QString("%1%2%3,").arg(fNumb).arg(comboxes[i]->currentText()).arg(sNumb);
                }



                if (Config == Indicators)
                {
                    fNumb = IO::IndicatorsNames.indexOf(groupsList[i]->item(k)->text());
                    sNumb = IO::IndicatorsNames.indexOf(groupsList[i+1]->item(l)->text());
                    result += QString("%1%2%3,").arg(fNumb).arg(comboxes[i]->currentText()).arg(sNumb);
                }

            }
        }
    }


    return result.left(result.size()-1);


}

void SetProjectGroupImportanceForm::on_OkButton_clicked()
{
    auto result = GetImportanceGroupString();



    switch (Config)
    {
    case Projects:

        StartupConfigForm::NotParsedImportanceGroupOfProjects = GetImportanceGroupString();
//        qDebug() << GetImportanceGroupString().split("≥");
        if (StartupConfigForm::NotParsedImportanceGroupOfProjects != "")
            btnProjects->setStyleSheet("color: rgb(0, 195, 13);");
        else
            btnProjects->setStyleSheet("");
        break;


    case Indicators:

        StartupConfigForm::NotParsedImportanceGroupOfIndicators = GetImportanceGroupString();
        if (StartupConfigForm::NotParsedImportanceGroupOfIndicators != "")
            btnIndicatators->setStyleSheet("color: rgb(0, 195, 13);");
        else
            btnIndicatators->setStyleSheet("");
        break;

    default:
        break;

    }



    close();
}

void SetProjectGroupImportanceForm::on_CancelButton_clicked()
{
    close();
}
