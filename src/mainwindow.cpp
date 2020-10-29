#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "###########################";
    qDebug() << "Ideal thread count" << QThread::idealThreadCount();

    ui->ListButton->hide();


    DataProcessing::mainWindow = this;

    startupconfigform = new StartupConfigForm;
    selectforexportForm = new SelectForExportForm;


    ui->InfoButton->setEnabled(false);
    ui->ConfiugreButton->setEnabled(false);
    ui->GraphicsButton->setEnabled(false);

    ui->SideBarContext->setMaximumWidth(0);

    OpenProjectsListForm::input = ui->InputTable;
    OpenProjectsListForm::output = ui->OutputTable;

    DataProcessing::inputTable = ui->InputTable;
    DataProcessing::outputTable = ui->OutputTable;

    open = new QAction(tr("&Открыть"), this);


    ui->treeView->addAction(open);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OpenFile()));
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));


    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());


    ui->treeView->setModel(model);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->hideColumn(4);

    ui->PlusButton->hide();
    ui->MinusButton->hide();


    //--------------------------------------------------------------------------------------------
    connect(this, SIGNAL(OpenedLoadingForm()), this, SLOT(OpenLoadingForm()));
    connect(this, SIGNAL(ClosedLoadingForm()), this, SLOT(CloseLoadingForm()));

    connect(ui->AboutProgramAction, SIGNAL(triggered()), this, SLOT(OpenAboutProgramForm()));

    connect(ui->ExportToExcelAction, SIGNAL(triggered()), this, SLOT(OpenExportFileForm()));

    connect(ui->OpenFileAction, SIGNAL(triggered()), this, SLOT(OpenFileDialog()));
    connect(ui->DeleteAllAction, SIGNAL(triggered()), this, SLOT(DeleteAllAction()));

    connect(ui->ResizeToWindowInputTableAction, SIGNAL(triggered()), this,  SLOT(ResizeToWindowInputTable()));
    connect(ui->ResizeToWindowOutputTableAction, SIGNAL(triggered()), this,  SLOT(ResizeToWindowOutputTable()));
    connect(ui->ResizeToWindowBothTablesAction, SIGNAL(triggered()), this,  SLOT(ResizeToWindowBothTables()));

    connect(ui->ResizeToContentInputTableAction, SIGNAL(triggered()), this, SLOT(ResizeToContentInputTable()));
    connect(ui->ResizeToContentOutputTableAction, SIGNAL(triggered()), this, SLOT(ResizeToContentOutputTable()));
    connect(ui->ResizeToContentBothTablesAction, SIGNAL(triggered()), this, SLOT(ResizeToContentBothTables()));

    connect(ui->ToggleFullscreenAction, SIGNAL(triggered()), this, SLOT(ToggleFullscreen()));
    //--------------------------------------------------------------------------------------------

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_MoreButton_clicked()
{

    if (ui->SideBar->width() == 170)
    {
        ui->SideBar->setMaximumWidth(38);
        ui->SideBar->setMinimumWidth(38);

        ui->ExploreButton->setText("");
        ui->GraphicsButton->setText("");
        ui->ConfiugreButton->setText("");
        ui->ListButton->setFlat("");
        ui->InfoButton->setText("");

    }

    if (ui->SideBar->width() == 38)
    {
        ui->SideBar->setMaximumWidth(170);
        ui->SideBar->setMinimumWidth(170);

        ui->ExploreButton->setText("Обозреватель");
        ui->GraphicsButton->setText("Графики");
        ui->ListButton->setText("Проекты");
        ui->ConfiugreButton->setText("Конфигурация");
        ui->InfoButton->setText("Инфо");
    }
}

void MainWindow::on_CloseMenuButton_clicked()
{
    ui->SideBarContext->setMaximumWidth(0);
}

void MainWindow::on_PlusButton_clicked()
{
    ui->SideBarContext->setMaximumWidth(ui->SideBarContext->maximumWidth() + 10);
}

void MainWindow::on_MinusButton_clicked()
{
    ui->SideBarContext->setMaximumWidth(ui->SideBarContext->maximumWidth() - 10);
}

void MainWindow::on_ExploreButton_clicked()
{

    if (ui->SideBarContext->width() == 0)
    {

        ui->SideBarContext->setMinimumWidth(100);
        ui->SideBarContext->setMaximumWidth(100);
        ui->SideBarContext->setMinimumWidth(0);
        ui->SideBarContext->setMaximumWidth(99999);
    }

}

void MainWindow::on_ConfiugreButton_clicked()
{
    delete startupconfigform;
    startupconfigform = new StartupConfigForm;

    startupconfigform->solutionName = SolutionDB::currentSolutionName;

    startupconfigform->input = ui->InputTable;
    startupconfigform->output = ui->OutputTable;

    startupconfigform->show();
}

void MainWindow::OpenFile()
{
    QFileSystemModel* model = new QFileSystemModel();
    QModelIndexList lst = ui->treeView->selectionModel()->selectedIndexes();
    QString path =  model->filePath(lst.at(0));

    //emit OpenedLoadingForm();
    //to future = QtConcurrent::run(OpenSolution, path); //Считывает файл в отдельном потоке
    OpenSolution(path);
}

void MainWindow::onCustomContextMenu(const QPoint &point)
{
    QMenu * menu = new QMenu(this);
    QAction * open = new QAction(tr("Открыть"), this);
    connect(open, SIGNAL(triggered()), this, SLOT(OpenFile()));
    menu->addAction(open);
    menu->popup(ui->treeView->viewport()->mapToGlobal(point));
}

void MainWindow::MakeToolTip()
{
    QString tooltip = "<p>Количество проектов: %1</p>"
                      "<p>Количество итераций: %2</p>"
                      "<p>Шаг дробления: %3</p>";

    QString str;
    for (int i=0; i < IO::IndicatorsNames.size(); i++)
    {
        str = DataProcessing::PriorityList[i] == "max" ? "Лучше - больше" : "Лучше - меньше";
        tooltip.append(QString("<p>%1: %2</p>").arg(IO::IndicatorsNames[i]).arg(str));
    }

    ui->InputTable->setToolTipDuration(1000);
    ui->InputTable->setToolTip(tooltip
                               .arg(IO::ProjectsNames.size())
                               .arg(DataProcessing::WeightsTable.size())
                               .arg(DataProcessing::CrushingStep));
}

void MainWindow::on_InfoButton_clicked()
{
    Info* info = (Info*)infoform;

    if (info != nullptr)
        delete info;

    infoform = new Info;
    info = (Info*)infoform;
    info = (Info*)infoform;
    info->show();
    info->activateWindow();
}

void MainWindow::on_GraphicsButton_clicked()
{

    preselectionchartsform = std::unique_ptr<PreSelectionChartsForm>(new PreSelectionChartsForm);
    //preselectionchartsform = new PreSelectionChartsForm();
    preselectionchartsform->show();
    preselectionchartsform->activateWindow();

    //OpenForm(preselectionchartsform);
    //OpenForm(preselection);
}

void MainWindow::on_ListButton_clicked()
{

    //    delete openprojectslistform;
    //    openprojectslistform = new OpenProjectsListForm();
    //    openprojectslistform->show();

    //OpenProjectsListForm* form = new OpenProjectsListForm();
    //form->show();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();


    if (key == Qt::Key_L)
    {

    }



}

void MainWindow::OpenFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"),
                                                    path,
                                                    tr("Excel (*.xlsx *.xls)"));


    OpenSolution(fileName);

    path = fileName.remove(fileName.split("/").last());
}

void MainWindow::UpdateDeleteMenu(const QString solutionName)
{
    QAction* action = new QAction(solutionName);
    ui->DeleteMenu->addAction(action);
    DeleteMenuActions << action;
    connect(action, SIGNAL(triggered()), this, SLOT(DeleteAction()));

}

void MainWindow::UpdateOpenMenu(const QString solutionName)
{
    QAction* action = new QAction(solutionName);
    ui->OpenMenu->addAction(action);
    OpenMenuActions << action;
    connect(action, SIGNAL(triggered()), this, SLOT(OpenAction()));
}

void MainWindow::OpenLoadingForm()
{
    //    FileLoadingDialog* loadingFileDialog = new FileLoadingDialog("fileName");
    //    loadingFileDialog->setModal(true);
    //    loadingFileDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
    //    loadingFileDialog->show();
}

void MainWindow::CloseLoadingForm()
{


}

void MainWindow::OpenAboutProgramForm()
{
    //OpenForm(aboutProgramForm);

    delete aboutProgramForm;
    aboutProgramForm = new AboutProgramForm();
    aboutProgramForm->show();
    aboutProgramForm->activateWindow();
}

void MainWindow::OpenExportFileForm()
{
    OpenForm(selectforexportForm);

    //    delete selectforexportForm;
    //    selectforexportForm = new SelectForExportForm();
    //    selectforexportForm->show();
    //    selectforexportForm->activateWindow();
}

void MainWindow::ToggleFullscreen()
{
    if (this->isFullScreen())
    {
        this->showNormal();
        return;
    }

    if (!this->isFullScreen())
    {
        this->showFullScreen();
        return;
    }
}

void MainWindow::ResizeToWindowInputTable()
{
    ui->InputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::ResizeToWindowOutputTable()
{
    ui->OutputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::ResizeToWindowBothTables()
{
    ui->InputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->OutputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::ResizeToContentInputTable()
{
    ui->InputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->InputTable->resizeColumnsToContents();
}

void MainWindow::ResizeToContentOutputTable()
{
    ui->OutputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->OutputTable->resizeColumnsToContents();
}

void MainWindow::ResizeToContentBothTables()
{
    ui->InputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->OutputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->InputTable->resizeColumnsToContents();
    ui->OutputTable->resizeColumnsToContents();
}

void MainWindow::DeleteAction()
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());

    //remove from DeleteMenu
    ui->DeleteMenu->removeAction(action);

    const QString solutionName = action->text();

    if (solutionName == SolutionDB::currentSolutionName)
        setWindowTitle(QString("Система принятия решений"));

    //remove from OpenMenu
    for (int i = 0; i < OpenMenuActions.size(); i++)
    {
        if (OpenMenuActions[i]->text() == action->text())
        {
            ui->OpenMenu->removeAction(OpenMenuActions[i]);
            delete OpenMenuActions[i];
            OpenMenuActions.removeAt(i);
            break;
        }
    }


    //remove from DB
    SolutionDB::RemoveSolution(solutionName);


    if (SolutionDB::currentSolutionName == solutionName)
    {
        if (SolutionDB::DB.size() > 0)
        {
            SolutionDB::LoadSolution(SolutionDB::DB.first());
            IO::FillingTables(ui->InputTable, ui->OutputTable);
        }

        if (SolutionDB::DB.size() == 0)
        {
            ui->InfoButton->setEnabled(false);
            ui->ConfiugreButton->setEnabled(false);
            ui->GraphicsButton->setEnabled(false);

            ui->InputTable->setColumnCount(0);
            ui->InputTable->setRowCount(0);

            ui->OutputTable->setColumnCount(0);
            ui->OutputTable->setRowCount(0);
        }
    }


    delete action;
}

void MainWindow::OpenAction()
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    const QString solutionName = action->text();
    SolutionDB::LoadSolution(solutionName);
    setWindowTitle(QString("Система принятия решений | %1   %2").arg(solutionName).arg(DataProcessing::timeElapsedParsed));
    IO::FillingTables(ui->InputTable, ui->OutputTable);


}

void MainWindow::DeleteAllAction()
{
    //Remove from DB
    SolutionDB::DB.clear();
    setWindowTitle(QString("Система принятия решений"));

    //Hide buttons
    ui->InfoButton->setEnabled(false);
    ui->ConfiugreButton->setEnabled(false);
    ui->GraphicsButton->setEnabled(false);

    //Clean IO tables
    ui->InputTable->setRowCount(0);
    ui->InputTable->setColumnCount(0);

    ui->OutputTable->setRowCount(0);
    ui->OutputTable->setColumnCount(0);


    const int ActionCount = OpenMenuActions.size();

    //Remove from openMenu and OpenMenuActions
    for (int i=0; i < ActionCount; i++)
    {
        ui->OpenMenu->removeAction(OpenMenuActions[i]);
        delete OpenMenuActions[i];
    }


    //Remove from deleteMenu and DeleteMenuActions

    for (int i=0; i < ActionCount; i++)
    {
        ui->DeleteMenu->removeAction(DeleteMenuActions[i]);
        delete DeleteMenuActions[i];
    }


    OpenMenuActions.clear();
    DeleteMenuActions.clear();
}



void MainWindow::OpenSolution(const QString fileName)
{
    if ((!fileName.contains(".xlsx"))&&(!fileName.contains(".xls"))) return;
    QString solutionName = fileName.split('/').last().split('.').first();
    QString NewSultionName;
    int n = 1;

    auto OpenStartupConfigForm = [&](QString solName)
    {
        delete startupconfigform;
        startupconfigform = new StartupConfigForm;

        startupconfigform->solutionName = solName;

        startupconfigform->input = ui->InputTable;
        startupconfigform->output = ui->OutputTable;

        startupconfigform->show();
        startupconfigform->activateWindow();
    };

    //Если содержит, загрузить входные данные с БД
    //Задать имя в формате {Name}_n, где n - цел

    if (SolutionDB::IsContained(solutionName))
    {
        while (SolutionDB::IsContained(QString("%1_%2").arg(solutionName).arg(n)))
            n++;

        Solution old = SolutionDB::GetSolution(solutionName);

        IO::BaseTable = old.BaseTable;
        IO::IndicatorsNames = old.IndicatorsNames;
        IO::ProjectsNames = old.ProjectsNames;


        QString newSolName = QString("%1_%2").arg(solutionName).arg(n);

        UpdateDeleteMenu(newSolName);
        UpdateOpenMenu(newSolName);

        OpenStartupConfigForm(newSolName);

        //        QMessageBox messageBox(QMessageBox::Question,
        //                               tr("Файл уже открыт"),
        //                               tr("Открыть заново ?"),
        //                               QMessageBox::Yes | QMessageBox::No,
        //                               this);

        //        messageBox.setButtonText(QMessageBox::Yes, tr("Да"));
        //        messageBox.setButtonText(QMessageBox::No, tr("Нет"));

        //        int result = messageBox.exec();


        //        if (result == QMessageBox::Yes)
        //        {
        //            SolutionDB::LoadSolution(solutionName);
        //            OpenStartupConfigForm();
        //        }

        //        if (result == QMessageBox::No)
        //        {
        //            SolutionDB::LoadSolution(solutionName);
        //            IO::FillingTables(ui->InputTable, ui->OutputTable);
        //        }


    }

    if (!SolutionDB::IsContained(solutionName))
    {
        //IO::OpenExelFile(fileName);

        IO::OpenExelFile1(fileName);

        UpdateDeleteMenu(solutionName);
        UpdateOpenMenu(solutionName);

        ui->InfoButton->setEnabled(true);
        ui->ConfiugreButton->setEnabled(true);
        ui->GraphicsButton->setEnabled(true);


        OpenStartupConfigForm(solutionName);
    }
}

template<typename FormType>
void MainWindow::OpenForm(FormType* form)
{
    if (form != nullptr)
    {
        delete form;
        form = nullptr;
    }
    form = new FormType();
    form->show();
    form->activateWindow();
}

template<typename FormType>
void MainWindow::OpenForm(FormType& form)
{
    //form =  FormType();
    form.show();
    form.activateWindow();
}
