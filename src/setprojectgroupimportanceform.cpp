#include "setprojectgroupimportanceform.h"
#include "ui_setprojectgroupimportanceform.h"

SetProjectGroupImportanceForm::SetProjectGroupImportanceForm(QWidget *parent, StartupConfig config) :
    QWidget(parent),
    ui(new Ui::SetProjectGroupImportanceForm)
{
    ui->setupUi(this);

    Config = config;

    comboxes.clear();
    checkboxes.clear();
    VLayouts.clear();
    scrollAreas.clear();

    ui->comboBox->setMinimumWidth(35);

    ui->scrollArea->hide();
    ui->scrollArea_2->hide();
    ui->comboBox->hide();

    //    comboxes << ui->comboBox;

    //    if (config == Projects)
    //        SetStartupProjects();

    //    if (config == Indicators)
    //        SetStartupIndicators();

}

SetProjectGroupImportanceForm::~SetProjectGroupImportanceForm()
{
    delete ui;
}

void SetProjectGroupImportanceForm::SetStartupProjects()
{
    QVector<QCheckBox*> v1;
    QVector<QCheckBox*> v2;
    foreach (QString project, IO::ProjectsNames)
    {
        QCheckBox* box1 = new QCheckBox(project);
        QCheckBox* box2 = new QCheckBox(project);

        connect(box1, SIGNAL(stateChanged(int)), this, SLOT(Checked(int)));
        connect(box2, SIGNAL(stateChanged(int)), this, SLOT(Checked(int)));

        ui->vLayout1->addWidget(box1);
        ui->vLayout2->addWidget(box2);

        v1 << box1;
        v2 << box2;
    }

    checkboxes << v1 << v2;

}

void SetProjectGroupImportanceForm::SetStartupIndicators()
{
    QVector<QCheckBox*> v1;
    QVector<QCheckBox*> v2;
    foreach (QString indicator, IO::IndicatorsNames)
    {
        QCheckBox* box1 = new QCheckBox(indicator);
        QCheckBox* box2 = new QCheckBox(indicator);


        connect(box1, SIGNAL(stateChanged(int)), this, SLOT(Checked(int)));
        connect(box2, SIGNAL(stateChanged(int)), this, SLOT(Checked(int)));

        ui->vLayout1->addWidget(box1);
        ui->vLayout2->addWidget(box2);

        v1 << box1;
        v2 << box2;
    }

    checkboxes << v1 << v2;
}

void SetProjectGroupImportanceForm::UpdateCheckBoxesVisibility()
{
    checkedList.clear();
    QCheckBox* chBox;


    //Ищу интервал до QComboBox с ';'
    //Обрабатываю данный интвервал

    //Функция, которая будет заполнять checkedList отмеченными вариантами QComoBox до ';'
    auto SetCheckedList = [&](int start)
    {
        checkedList.clear();

        QCheckBox* chBox;
        int end = -1;

        for (int i = start; i < comboxes.size(); i++)
        {
            if (comboxes[i]->currentText() == ";")
                break;

            if (comboxes[i]->currentText() == ">")
            {
                end = i;
            }
        }

        end++;

        for (int i = start; i <= end; i++)
        {
            for (int j = 0; j < checkboxes[i].size(); j++)
            {
                chBox = checkboxes[i][j];

                if (chBox->isChecked())
                    checkedList << chBox->text();
            }
        }




    };


    int start = 0;

    for (int i = 0; i < checkboxes.size(); i++)
    {
        if (checkedList.size() == 0)
            SetCheckedList(start);


        for (int j = 0; j < checkboxes.first().size(); j++)
        {
            chBox = checkboxes[i][j];


            //Если чекбокс в списке отмеченных, то скрыть если не отмечен checked
            if (checkedList.contains(chBox->text()) && !(chBox->isChecked()))
                chBox->hide();

            //Если нету в списке отмеченных и скрыт, то показать
            if (!checkedList.contains(chBox->text()) && chBox->isHidden())
                chBox->show();

        }


        if (i < comboxes.size())
            if (comboxes[i]->currentText() == ";")
            {
                start = i + 1;
                checkedList.clear();
            }
    }
}

void SetProjectGroupImportanceForm::on_AddGroupButton_clicked()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QScrollArea* area = new QScrollArea(this);
    QWidget* contents = new QWidget;

    area->setAlignment(Qt::AlignVCenter);
    area->setWidget(contents);
    contents->setLayout(layout);


    //Добавление QComboBox
    //----------------------------------
    if (checkboxes.size() > 0)
    {
        QComboBox* box = new QComboBox;

        //box->currentTextChanged("qwe");


        comboxes << box;
        box->addItem(">");
        box->addItem(";");
        ui->MainLayout->addWidget(box);
        connect(box, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UpdateVisibility()));
    }
    //----------------------------------

    scrollAreas << area;
    VLayouts << layout;
    widgets << contents;


    area->setMinimumWidth(180);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);



    ui->MainLayout->addWidget(area);


    QVector<QCheckBox*> v;


    const int ElementHeight = 20;


    //Добавить список проектов
    //-----------------------------------------------
    if (Config == Projects)
    {
        contents->resize(area->width(), ElementHeight * IO::ProjectsNames.size());

        foreach (QString project, IO::ProjectsNames)
        {
            QCheckBox* box = new QCheckBox(project);
            layout->addWidget(box);
            connect(box, SIGNAL(stateChanged(int)), this, SLOT(Checked(int)));
            v << box;
        }
    }
    //-----------------------------------------------



    //Добавить список показателей
    //-----------------------------------------------
    if (Config == Indicators)
    {
        contents->resize(area->width(), ElementHeight * IO::IndicatorsNames.size());

        foreach (QString indicator, IO::IndicatorsNames)
        {
            QCheckBox* box = new QCheckBox(indicator);
            layout->addWidget(box);
            connect(box, SIGNAL(stateChanged(int)), this, SLOT(Checked(int)));

            v << box;
        }
    }
    //-----------------------------------------------





    checkboxes << v;
}

void SetProjectGroupImportanceForm::on_RemoveGroupButton_clicked()
{
    if (checkboxes.size() == 0) return;


    //1. Удалить чекбоксы
    //----------------------------------------
    for (int i = 0; i < checkboxes.last().size(); i++)
        delete checkboxes.last()[i];
    checkboxes.removeLast();
    //----------------------------------------


    //2. Удалить комбобокс
    //----------------------------------------
    if (comboxes.size() > 0)
    {
        delete comboxes.last();
        comboxes.removeLast();
    }
    //----------------------------------------


    //3. удалить scrollArea
    //----------------------------------------
    delete scrollAreas.last();
    scrollAreas.removeLast();
    //----------------------------------------
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


    for (int i = 0; i < checkboxes.size() - 1; i++)
    {
        //Смотрим первый список чекбоксов(i) и следующий(i+1)
        //А также значение i-го QComboBox

        for (int k = 0; k < checkboxes[i].size(); k++)
        {

            //Если в i-м QComboBox находится ;, то пропус
            if (comboxes[i]->currentText() == ";") continue;


            for (int l = 0; l < checkboxes[i+1].size(); l++)
            {
                if (checkboxes[i][k]->isChecked() && checkboxes[i+1][l]->isChecked())
                {
                    if (Config == Projects)
                    {
                        fNumb = IO::ProjectsNames.indexOf(checkboxes[i][k]->text());
                        sNumb = IO::ProjectsNames.indexOf(checkboxes[i][l]->text());
                        result += QString("%1%2%3,").arg(fNumb).arg(comboxes[i]->currentText()).arg(sNumb);
                    }



                    if (Config == Indicators)
                    {
                        fNumb = IO::IndicatorsNames.indexOf(checkboxes[i][k]->text());
                        sNumb = IO::IndicatorsNames.indexOf(checkboxes[i][l]->text());
                        result += QString("%1%2%3,").arg(fNumb).arg(comboxes[i]->currentText()).arg(sNumb);
                    }
                }
            }
        }
    }


    return result.left(result.size()-1);


}

void SetProjectGroupImportanceForm::Checked(int state)
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(QObject::sender());
    //0 - notChecked
    //2 - Checked
    //    qDebug() << checkBox << checkBox->text() << checkBox->isChecked() << "State:" << state;

    //    if (state == 0) checkedList.removeOne(checkBox->text());
    //    if (state == 2) checkedList << checkBox->text();

    UpdateCheckBoxesVisibility();
}

void SetProjectGroupImportanceForm::UpdateVisibility()
{
    UpdateCheckBoxesVisibility();
}

void SetProjectGroupImportanceForm::on_OkButton_clicked()
{
    auto result = GetImportanceGroupString();



    switch (Config)
    {
    case Projects:
        qDebug() << "Projects";
        qDebug() << "================================================";
        qDebug() << GetImportanceGroupString();
        qDebug() << "================================================";
        StartupConfigForm::NotParsedImportanceGroupOfProjects = GetImportanceGroupString();
        if (StartupConfigForm::NotParsedImportanceGroupOfProjects != "")
            btnProjects->setStyleSheet("color: rgb(0, 195, 13);");
        else
            btnProjects->setStyleSheet("");
        break;


    case Indicators:
        qDebug() << "Indicators";
        qDebug() << "================================================";
        qDebug() << GetImportanceGroupString();
        qDebug() << "================================================";
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
