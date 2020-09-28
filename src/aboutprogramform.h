#ifndef ABOUTPROGRAMFORM_H
#define ABOUTPROGRAMFORM_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class AboutProgramForm;
}

class AboutProgramForm : public QWidget
{
    Q_OBJECT

public:
    explicit AboutProgramForm(QWidget *parent = nullptr);
    ~AboutProgramForm();

private slots:
    void on_CloseButton_clicked();

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

private:
    Ui::AboutProgramForm *ui;
};

#endif // ABOUTPROGRAMFORM_H
