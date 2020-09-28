#ifndef INFO_H
#define INFO_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <io.h>
#include <dataprocessing.h>

namespace Ui {
class Info;
}

class Info : public QWidget
{
    Q_OBJECT

public:
    explicit Info(QWidget *parent = nullptr);
    ~Info();

    void Set();

    //===========================================
private slots:
    void on_CloseButton_clicked();

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
    //===========================================
private:
    Ui::Info *ui;
};

#endif // INFO_H
