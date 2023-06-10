#ifndef NETWINDOW_H
#define NETWINDOW_H

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
#include <QTcpSocket>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QFile>
#include <QIODevice>
namespace Ui {
class netwindow;
}

class netwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit netwindow(QWidget *parent = nullptr);
    ~netwindow();

private:
    Ui::netwindow *ui;

    QLineEdit *usernameEdit;
    QFile* file;
    bool send();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    int checkWin(int x, int y);

    int m_board[9][9] = {0};
    int m_player;

    int dx[4] = { 1, 0, 0, -1};
    int dy[4] = { 0, 1, -1, 0};
    void dfs(int x, int y);
    bool is_dead;
    bool st[9][9] = {0};
    void init();
    int n_player;

    int ready = 0;
    int set = 0;
    int reset = 0;
    int so = 0;//服务端0，客户端1

    int player = 1;
    int reply = 0;
    int readyflag = 0;
    int giveupflag = 0;
    int timeoutflag = 0;
    int suicideflag = 0;
    int aiflag = 0;

    //按钮
    QPushButton* btnRePlay;
    QPushButton* btnGiveUp;


    QLabel *m_timeLabel;
    QLabel *m_timetext;

    QTimer *m_timer;
    int m_remainingTime;

//    QLabel *connectlabel;

    // 服务端
    NetworkServer* server;
    // 客户端（收用）
    NetworkSocket* socket;
    // 另一个客户端（发用）
    QTcpSocket* lastOne;
    QString IP;
    int PORT;
    // 客户端池，NetworkServer 有一个 QList 的，但这里我想用 set，所以又弄了一个
    QSet<QTcpSocket*> clients;

    QPushButton * btnclientSend;
    QPushButton * btnserverSend;

    QLineEdit *iplineEdit;
    QLineEdit *portlineEdit;
    QLineEdit *clientSendEdit;
    QLineEdit *serverSendEdit;
    QLineEdit *clientGetEdit;
    QLineEdit *serverGetEdit;

    QButtonGroup *m_buttonGroup;
    QRadioButton* btnblack;
    QRadioButton* btnwhite;

    QRadioButton* btnjj;

private slots:
    void updateRemainingTime();
    void resetTime();
    void resettime();

    void receieveData(QTcpSocket* client, NetworkData data);
    void receieveDataFromServer(NetworkData data);
    void on_listen();
    void on_connect();
    void on_ready();
    void onClientSendButtonClicked();
    void onServerSendButtonClicked();

    void resetboard();

    void ai();
    int checksame(int x,int y);
    int checkdiff(int x,int y);
};

#endif // NETWINDOW_H
