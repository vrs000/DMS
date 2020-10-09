#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QLabel>
#include <QFileDialog>
#include <io.h>
#include <dataprocessing.h>
#include <QMessageBox>
#include <QThread>
#include <info.h>
#include <chartsform.h>
#include <solution.h>
#include <solutiondb.h>
#include <openprojectslistform.h>
#include <startupconfigform.h>
#include <preselectionchartsform.h>
#include <selectforexportform.h>
#include <aboutprogramform.h>
//#include <loadingform.h>
#include <QDialog>
#include <QtConcurrent/QtConcurrent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QAction* open;

    void MakeToolTip();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    void OpenSolution(const QString fileName);
    void UpdateDeleteMenu(const QString solutionName);
    void UpdateOpenMenu(const QString solutionName);

    QList<QAction*> OpenMenuActions;
    QList<QAction*> DeleteMenuActions;

    QString path = "./";


    template<typename FormType> void OpenForm(FormType* form);
    template<typename FormType> void OpenForm(FormType& form);


    //forms
private:
    StartupConfigForm* startupconfigform = new StartupConfigForm();
    Info* infoform = new Info();
    SelectForExportForm* selectforexportForm = new SelectForExportForm();
    PreSelectionChartsForm* preselectionchartsform = new PreSelectionChartsForm();
    PreSelectionChartsForm preselection;

    AboutProgramForm* aboutProgramForm = new AboutProgramForm();
   // LoadingForm* loadingForm = new LoadingForm();

signals:
    void OpenedLoadingForm();
    void ClosedLoadingForm();


private slots:
    void OpenLoadingForm();
    void CloseLoadingForm();

    void OpenAboutProgramForm();

    void OpenExportFileForm();

    void ToggleFullscreen();

    void ResizeToWindowInputTable();
    void ResizeToWindowOutputTable();
    void ResizeToWindowBothTables();

    void ResizeToContentInputTable();
    void ResizeToContentOutputTable();
    void ResizeToContentBothTables();

    void DeleteAllAction();

    void OpenAction();

    void DeleteAction();

    void onCustomContextMenu(const QPoint &point);

    void OpenFile();

    void OpenFileDialog();

    void on_MoreButton_clicked();

    void on_CloseMenuButton_clicked();

    void on_PlusButton_clicked();

    void on_MinusButton_clicked();

    void on_ExploreButton_clicked();

    void on_ConfiugreButton_clicked();

    void on_InfoButton_clicked();

    void on_GraphicsButton_clicked();

    void on_ListButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
