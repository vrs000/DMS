#include "fileloadingdialog.h"

FileLoadingDialog::FileLoadingDialog(QString fileName)
{
    setWindowTitle("Загрузка...");
    setMinimumWidth(300);
    bar = new QProgressBar(this);
    bar->setRange(0, 0);
    QLabel* label = new QLabel("Загрузка файла " + fileName);
    label->setParent(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(bar);
    setLayout(layout);
}

void FileLoadingDialog::closeDialog()
{
    this->close();
}
