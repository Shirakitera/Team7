#include "mainwin.h"
#include "ui_mainwin.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QLabel>
#include <QElapsedTimer>
#include <QtDebug>

mainwin::mainwin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mainwin)
{
    ui->setupUi(this);
}

mainwin::~mainwin()
{
    delete ui;
}

void mainwin::on_pushButton_2_clicked()
{
    netwindow *net = new netwindow;
    net->show();
    this->close();

}


void mainwin::on_pushButton_clicked()
{
    MainWindow *main=new MainWindow;
    main->show();
    this->close();
}

void mainwin::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pixmap("C:\\Users\\1\\Desktop\\R-C.jpg");
    pixmap = pixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(rect(), pixmap);
}
