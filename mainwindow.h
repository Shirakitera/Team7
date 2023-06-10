// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QElapsedTimer>
#include <QLineEdit>
#include <QSet>
#include "networkserver.h"
#include "networksocket.h"
#include <QRadioButton>
#include <QFileDialog>
#include <QFile>
#include <QElapsedTimer>
#include <QEventLoop>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:
    Ui::MainWindow *ui;

    QLineEdit *usernameEdit;

    bool send();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void Delay_MSec(unsigned int msec);
    int checkWin(int x, int y);

    int m_board[15][15] = {0};
    int m_player;

    int dx[4] = { 1, 0, 0, -1};
    int dy[4] = { 0, 1, -1, 0};
    void dfs(int x, int y);
    bool is_dead;
    bool st[15][15];
    void init();
    int n_player;

    int board = 9;
    QRadioButton btn9;
    QRadioButton btn11;
    QRadioButton btn13;
    QButtonGroup *m_buttonGroup;

    //按钮
    QPushButton* btnRePlay;
    QPushButton* btnGiveUp;
    QPushButton* btnReshow;
    QPushButton* btnLastStep;
    QPushButton* btnNextStep;
    QPushButton* btnStop;
    QFile* file;

    QLabel *m_timeLabel;
    QLabel *m_timetext;
    QPushButton *m_resetButton;
    QTimer *m_timer;
    int m_remainingTime;

    // 服务端
    NetworkServer* server;
    // 客户端
    NetworkSocket* socket;
    // 最后一个客户端
    QTcpSocket* lastOne;
    QString IP;
    int PORT;
    // 客户端池，NetworkServer 有一个 QList 的，但这里我想用 set，所以又弄了一个
    QSet<QTcpSocket*> clients;

    QPushButton * btn;
private slots:
    void updateRemainingTime();
    void resetTime();
    void resettime();

};

#endif // MAINWINDOW_H

