#ifndef FILELOADINGDIALOG_H
#define FILELOADINGDIALOG_H

#include <QObject>
#include <QDialog>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>

class FileLoadingDialog : public QDialog
{
    Q_OBJECT
public:
    FileLoadingDialog(QString fileName);

public slots:
    void closeDialog();

private:
    QProgressBar* bar;
};

#endif // FILELOADINGDIALOG_H
