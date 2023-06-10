#ifndef MAINWIN_H
#define MAINWIN_H

#include <QDialog>
#include <mainwindow.h>
#include <netwindow.h>
namespace Ui {
class mainwin;
}

class mainwin : public QDialog
{
    Q_OBJECT

public:
    explicit mainwin(QWidget *parent = nullptr);
    ~mainwin();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void paintEvent(QPaintEvent *event) override;

private:
    Ui::mainwin *ui;
};

#endif // MAINWIN_H
