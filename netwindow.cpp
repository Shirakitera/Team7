#include "netwindow.h"
#include "ui_netwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QLabel>
#include <QElapsedTimer>
#include <QtDebug>
#include <QAbstractSocket>
#include <QLineEdit>
#include <QTcpSocket>
#include <QButtonGroup>
#include <string>
#include <iostream>
#include <QDateTime>
#include <QRadioButton>
#include <QtCore/QObject>
#include <cctype>
#include <QPropertyAnimation> //QPropertyAnimation类头文件
#include <QEasingCurve> //QEasingCurve类头文件（可选）
#include <QCheckBox>
#include <QHBoxLayout>
#include <QElapsedTimer>

/*m_player为自己，n为对方；
设置一个player代表目前该谁动；1黑2白0有人输；
set 为谁先行，0服务端，1客户端；
so 代表自己为服务端->0;客户端->1;
reply 为是否已申请，0未申请1已申请；
readyflag 为是否已经发送readyop;
giveupflag 为是否已发送giveupflag;
以及timeoutflag、suicideflag;
aiflag*/

netwindow::netwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::netwindow)
{
    ui->setupUi(this);

    // 本地 IP
    IP = "10.47.46.116";
    // 端口
    PORT = 16677;

    setFixedSize(880, 700);//窗口大小
    memset(m_board, 0, sizeof(m_board));
    //file = new QFile("C:\\Users\\1\\Desktop\\Info.txt");
    file = new QFile("/Users/zhangjinkai/Desktop/Info.txt");
    file->open(QIODevice::WriteOnly|QIODevice::Truncate);
    file->close();
    m_player = 0;
    n_player = 0;
    player = 1;
    set = 0;
    reply = 0;

    QWidget* widget = new QWidget(this);
    setCentralWidget(widget);

    btnRePlay = new QPushButton("再来一局", this);
    btnGiveUp = new QPushButton("认输", this);

    QVBoxLayout* vLayout = new QVBoxLayout;//垂直布局
    vLayout->addWidget(btnRePlay);
    vLayout->addWidget(btnGiveUp);
    btnRePlay->setFixedSize(QSize(80, 40));//按钮大小
    btnRePlay->move(600, 550);//按钮位置

    btnGiveUp->setFixedSize(QSize(80, 40));
    btnGiveUp->move(600, 500);

    connect(btnRePlay, &QPushButton::clicked, this, [=](){
        // 点击“再来一局”按钮后的功能实现
        resetboard();
    });

    connect(btnGiveUp, &QPushButton::clicked, this, [=]()
    {
        resettime();
        // 点击“认输”按钮后的功能实现
        if(m_player==1)
        {
            giveupflag = 1;
            if(set==1) this->socket->send(NetworkData(OPCODE::GIVEUP_OP,"","send by client"));
            else this->server->send(lastOne,NetworkData(OPCODE::GIVEUP_OP,"","send by server"));
            QMessageBox::information(this, "gameover", "您认输了！");
            file->open(QIODevice::WriteOnly|QIODevice::Append);
            QTextStream txtOutput(file);
            txtOutput << 'G';
            file->close();
        }
        else
        {
            giveupflag = 1;
            if(set==0) this->socket->send(NetworkData(OPCODE::GIVEUP_OP,"","send by client"));
            else this->server->send(lastOne,NetworkData(OPCODE::GIVEUP_OP,"","send by server"));
            QMessageBox::information(this, "gameover", "您认输了！");
            file->open(QIODevice::WriteOnly|QIODevice::Append);
            QTextStream txtOutput(file);
            txtOutput << 'G';
            file->close();
        }
    });



        m_timeLabel = new QLabel("00:00:00", this);
        m_timetext = new QLabel("黑子第一手不计时", this);
        m_timer = new QTimer(this);
        m_remainingTime = 5; // 初始倒计时为5秒

         //将定时器的timeout信号连接到updateRemainingTime槽函数
        connect(m_timer, &QTimer::timeout, this, &netwindow::updateRemainingTime);

         //设置界面布局
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(m_timeLabel);
        layout->addWidget(m_timetext);


        clientGetEdit = new QLineEdit(this);
        clientGetEdit->setFixedWidth(200);   // 设置控件宽度为 200 像素
        clientGetEdit->setFixedHeight(30);  // 设置控件高度为 30 像素
        clientGetEdit->move(590,180);
        clientGetEdit->show();

        clientSendEdit = new QLineEdit(this);
        clientSendEdit->setFixedWidth(200);
        clientSendEdit->setFixedHeight(30);
        clientSendEdit->move(590,230);
        clientSendEdit->show();

        serverGetEdit = new QLineEdit(this);
        serverGetEdit->setFixedWidth(200);
        serverGetEdit->setFixedHeight(30);
        serverGetEdit->move(590,280);
        serverGetEdit->show();

        serverSendEdit = new QLineEdit(this);
        serverSendEdit->setFixedWidth(200);
        serverSendEdit->setFixedHeight(30);
        serverSendEdit->move(590,330);
        serverSendEdit->show();


        QPushButton* btnclientGet = new QPushButton("clientget", this);
        btnclientGet->setFixedSize(QSize(70, 35));
        btnclientGet->move(800, 178);

        QPushButton* btnclientSend = new QPushButton("clientsend", this);
        btnclientSend->setFixedSize(QSize(70, 35));
        btnclientSend->move(800, 228);

        QPushButton* btnserverGet = new QPushButton("serverget", this);
        btnserverGet->setFixedSize(QSize(70, 35));
        btnserverGet->move(800, 278);

        QPushButton* btnserverSend = new QPushButton("serversend", this);
        btnserverSend->setFixedSize(QSize(70, 35));
        btnserverSend->move(800, 328);


        QFont font("Arial", 30); // 创建一个22号字体的Arial字体对象
        m_timeLabel->setFont(font); // 设置label的字体为Arial，大小为24号
        m_timeLabel->setStyleSheet("color: red;"); // 设置字体颜色
        m_timeLabel->setMinimumSize(200, 80); // 设置时间标签的最小尺寸为100x50,防遮挡
        m_timeLabel->move(600, 100);

        QFont Font("Arial", 19);
        m_timetext->setFont(Font);
        m_timetext->setStyleSheet("color: black;");
        m_timetext->move(580, 50);
        m_timetext->setMinimumSize(200, 80);
        setLayout(layout);



        QLabel *iplabel = new QLabel("IP", this); // 创建一个新的 QLabel 对象
        iplabel->setFont(QFont("Arial", 12));                // 设置字体
        iplabel->move(20,575);                              // 设置位置
        iplabel->setStyleSheet("color: black;");              // 设置显示样式
        iplabel->setFixedSize(QSize(100, 10));
        iplabel->setMinimumSize(200, 80);
        iplabel->show();

        QLabel *portlabel = new QLabel("端口", this);
        portlabel->setFont(QFont("Arial", 12));
        portlabel->move(10,630);
        portlabel->setStyleSheet("color: black;");
        portlabel->setFixedSize(QSize(100, 10));
        portlabel->setMinimumSize(200, 80);
        portlabel->show();

        iplineEdit = new QLineEdit(this);
        iplineEdit->setText(IP);
        iplineEdit->setFixedWidth(200);   // 设置控件宽度为 200 像素
        iplineEdit->setFixedHeight(30);  // 设置控件高度为 30 像素
        iplineEdit->move(50,600);
        iplineEdit->show();

        portlineEdit = new QLineEdit(this);
        portlineEdit->setText(QString::number(PORT));
        portlineEdit->setFixedWidth(200);
        portlineEdit->setFixedHeight(30);
        portlineEdit->move(50,655);
        portlineEdit->show();

        QPushButton* btnlisten = new QPushButton("监听", this);
        btnlisten->setFixedSize(QSize(80, 40));
        btnlisten->move(250, 595);

        QPushButton* btnconnect = new QPushButton("连接", this);
        btnconnect->setFixedSize(QSize(80, 40));
        btnconnect->move(250, 650);

        QRadioButton* btnblack = new QRadioButton("执黑先行", this);
        btnblack->setFixedSize(QSize(80,40));
        btnblack->move(450,595);
        QRadioButton* btnwhite = new QRadioButton("执白后行", this);
        btnwhite->setFixedSize(QSize(80,40));
        btnwhite->move(450,650);

        m_buttonGroup = new QButtonGroup(this);
        m_buttonGroup->addButton(btnblack, 1);//1黑
        m_buttonGroup->addButton(btnwhite, 2);//2白
        connect(m_buttonGroup, QOverload<int>::of(&QButtonGroup::idClicked),[=](int id)
        { ready = id; qDebug()<<"ready = id";});

        QPushButton* btnready = new QPushButton("申请对局", this);
        btnready->setFixedSize(QSize(80, 40));
        btnready->move(550, 610);

        QRadioButton* btnjj = new QRadioButton("人机托管",this);
        btnjj->setFixedSize(QSize(80,40));
        btnjj->move(600,400);
        btnjj->setChecked(false);
        QObject::connect(btnjj, &QRadioButton::toggled, [&](bool checked)
        {
            if (checked)
            {
                qDebug() << "选项A被选中";
                aiflag = 1;
                if(player = m_player)
                {
                    ai();
                }
            }
            else
            {
                qDebug() << "选项A未被选中";
                aiflag = 0;
            }
        });


        // 创建一个服务端
        this->server = new NetworkServer(this);

        lastOne = nullptr;

        // 创建一个客户端
        this->socket = new NetworkSocket(new QTcpSocket(),this);

        connect(btnclientSend,&QPushButton::clicked,this,&netwindow::onClientSendButtonClicked);
        connect(btnserverSend,&QPushButton::clicked,this,&netwindow::onServerSendButtonClicked);
        connect(this->socket,&NetworkSocket::receive,this,&netwindow::receieveDataFromServer);
        connect(this->server,&NetworkServer::receive,this,&netwindow::receieveData);
        connect(btnlisten,&QPushButton::clicked,this,&netwindow::on_listen);
        connect(btnconnect,&QPushButton::clicked,this,&netwindow::on_connect);
        connect(btnready,&QPushButton::clicked,this,&netwindow::on_ready);
        connect(this->socket->base(),&QTcpSocket::connected,this,[&](){QMessageBox::information(this, "connect", "连接成功");});

}

netwindow::~netwindow()
{
    delete ui;
}

void netwindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//抗锯齿
    painter.setPen(QPen(QColor(0, 0, 0)));//棋盘色

    //背景图
    QPixmap pixmap("C:\\Users\\1\\Desktop\\R-C (1).jpg");
    pixmap = pixmap.scaled(QSize(850,670), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(20, 20), pixmap);

    //画棋盘
    for (int i = 0; i < 9; i++)
    {
        painter.drawLine(QPoint(20, 20 + i * 40), QPoint(340, 20 + i * 40));
        painter.drawLine(QPoint(20 + i * 40, 20), QPoint(20 + i * 40, 340));
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (m_board[i][j] == 1)
            {
                painter.setPen(QPen(QColor(0, 0, 0)));
                painter.setBrush(QBrush(QColor(0, 0, 0)));//黑子色
                painter.drawEllipse(QPoint(20 + i * 40, 20 + j * 40), 18, 18);
            }
            else if (m_board[i][j] == 2)
            {
                painter.setPen(QPen(QColor(255, 255, 255)));
                painter.setBrush(QBrush(QColor(255, 255, 255)));//白子色
                painter.drawEllipse(QPoint(20 + i * 40, 20 + j * 40), 18, 18);
            }
        }
    }
}

void netwindow::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton)
    {
        if(reply == 1)
        {
            if(m_player != player)
            {
                if(player!=0) QMessageBox::information(this, "Warning!", "现在不该你下棋！");
                else QMessageBox::information(this, "Waiting!", "等待对方认输中");
            }
            else
            {
                int x = event->position().x() / 40;
                int y = event->position().y() / 40;
                if (m_board[x][y] == 0)
                {
                    m_board[x][y] = m_player;
                    int p = checkWin(x,y);
                    if(p==2 || p ==1)
                    {
                        QMessageBox::information(this, "Warning", "请勿自杀");
                        m_board[x][y] = 0;
                    }
                    else
                    {
                        m_board[x][y] = m_player;
                        if((set==1 && m_player==1)||(set==0 && m_player==2))
                        {
                            char ch = y + 65;
                            QString str = QString(ch) + QString::number(x+1);
//                            file->open(QIODevice::WriteOnly|QIODevice::Append);
//                            QTextStream txtOutput(file);
//                            txtOutput << str << ' ';
//                            file->close();
                            this->socket->send(NetworkData(OPCODE::MOVE_OP,str,""));
                            player = 3 - player;
                        }
                        else
                        {
                            char ch = y + 65;
                            QString str = QString(ch) + QString::number(x+1);
//                            file->open(QIODevice::WriteOnly|QIODevice::Append);
//                            QTextStream txtOutput(file);
//                            txtOutput << str << ' ';
//                            file->close();
                            this->server->send(lastOne,NetworkData(OPCODE::MOVE_OP,str,""));
                            player = 3 - player;
                        }
                    update();
                    resetTime();
                    }
                }
            }
        }
        else
        {
            QMessageBox::information(this, "Warning!", "请先申请对局！");
        }
    }
}

int netwindow::checkWin(int x, int y)
{
    //不围棋
    is_dead = 1;
    dfs(x, y); init();//判断自杀
    if(is_dead) return 3 - m_board[x][y];//死了

    for(int i = 0; i < 4; i++){
        int nx = x + dx[i], ny = y + dy[i];
        if(nx < 0 || nx >= 9 || ny < 0 || ny >= 9|| st[nx][ny]) continue;
        if(!m_board[nx][ny] || m_board[nx][ny] == m_board[x][y]) continue;//找到对方棋子

        //周围有异色
        if(m_board[nx][ny] + m_board[x][y] == 3){
            is_dead = 1;
            dfs(nx, ny); init();//判断对方死了没
            if(is_dead) return 3 - m_board[x][y];
       }
    }

    return 0;
}

void netwindow::dfs(int x, int y)
{
    for(int i = 0; i < 4; i++){
        int nx = x + dx[i], ny = y + dy[i];
        if(nx < 0 || nx >= 9 || ny < 0 || ny >= 9 || st[nx][ny]) continue;//四周是边界/标记过的同色子
        if(m_board[nx][ny] + m_board[x][y] == 3) continue;//周围有异子

        //周围有空子
        if(m_board[nx][ny] == 0){
            is_dead = 0; return;
        }
        //周围有未标记的同色子
        else{
            st[nx][ny] = 1;
            dfs(nx, ny);
        }
    }
}

void netwindow::init(){
    memset(st, 0, sizeof(st));//归零st
}



void netwindow::updateRemainingTime()
{
    // 计算剩余时间
    m_remainingTime--;
    int hours = m_remainingTime / 3600;
    int minutes = (m_remainingTime % 3600) / 60;
    int seconds = m_remainingTime % 60;

    // 更新QLabel的文本
    QString timeString = QString("%1:%2:%3")
            .arg(hours, 2, 10, QLatin1Char('0'))
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0'));
    m_timeLabel->setText(timeString);

    // 如果倒计时结束，停止定时器并显示提示信息
    if (m_remainingTime == 0) {
        m_timer->stop();
        if(player==n_player)//对方超时
        {
            timeoutflag = 1;
            if((set==1 && m_player==1)||(set==0 && m_player==2))//客户端
            {
                this->socket->send(NetworkData(OPCODE::TIMEOUT_END_OP,"",""));               
            }
            else //服务端
            {
                this->server->send(lastOne,NetworkData(OPCODE::TIMEOUT_END_OP,"",""));                
            }
        }
    }
}

void netwindow::resetboard()
{
    memset(m_board, 0, sizeof(m_board));
    resettime();
    reply = 0;
    player = 1;
    giveupflag = 0;
    readyflag = 0;
    timeoutflag = 0;
    suicideflag = 0;
    aiflag = 0;
    m_player = 0;
    n_player = 0;
    //btnjj->setChecked(false);
    update();
}

void netwindow::resetTime()
{
    // 停止定时器并重置倒计时
    m_timer->stop();
    m_remainingTime = 5;
    m_timeLabel->setText("00:00:05");
    // 启动定时器，每秒更新一次倒计时
    if(player==1) m_timetext->setText("黑子执子剩余时间");
    else m_timetext->setText("白子执子剩余时间");
    m_timer->start(1000);
}

void netwindow::resettime()
{
    m_timer->stop();
    m_remainingTime = 5;
    m_timeLabel->setText("00:00:00");
    m_timetext->setText("黑子第一手不计时");
}

void netwindow::onClientSendButtonClicked()
{
    this->socket->send(NetworkData(OPCODE::CHAT_OP,clientSendEdit->text(),"send by client"));
}

void netwindow::onServerSendButtonClicked()
{
    if(lastOne) this->server->send(lastOne,NetworkData(OPCODE::CHAT_OP,serverSendEdit->text(),"send by server"));
}

void netwindow::on_listen()
{
    // 设置服务器监听
    so = 0;
    QString port_ = portlineEdit->text();
    int port = port_.toInt();
    server->listen(QHostAddress::Any, port);
    qDebug()<<"111111111111111.";
}

void netwindow::on_connect()
{
    qDebug()<<"client reconnect to the server.";
    //this->socket->bye();
    so = 1;
    QString ip = iplineEdit->text();
    QString port_ = portlineEdit->text();
    // 将 str 转换为 int 类型
    int port = port_.toInt();
    socket->hello(ip,port);
    if(!this->socket->base()->waitForConnected(3000)){
    qDebug()<<"reconnect fail";
    }
}

void netwindow::on_ready()
{

    QString str;
    if(ready==1) str = "b";
    else str = "w";           //= QString::number(ready);
    qDebug()<<"ready = "<< ready;
    readyflag = 1;
    if(so == 1) this->socket->send(NetworkData(OPCODE::READY_OP,"send by server",str));
    if(so == 0) this->server->send(lastOne,NetworkData(OPCODE::READY_OP,"send by server",str));
}

int netwindow::checkdiff(int x,int y)
{
    int s=0;
    if(x-1<0||y-1<0||x+1>8||y+1>8)
    {
        if(x-1<0) s++;
        if(y-1<0) s++;
        if(x+1>8) s++;
        if(y+1>8) s++;
    }
    else
    {
        if(m_board[x-1][y]==n_player) s++;
        if(m_board[x][y-1]==n_player) s++;
        if(m_board[x+1][y]==n_player) s++;
        if(m_board[x][y+1]==n_player) s++;
    }
    return s;
}

int netwindow::checksame(int x, int y)
{
    int s=0;int b=0;
    if(x-1<0||y-1<0||x+1>8||y+1>8)
    {
        if(x-1<0||y-1<0) s++;
        if(x+1>8||y-1<0) s++;
        if(x-1<0||y+1>8) s++;
        if(x+1>8||y+1>8) s++;
    }
    else
    {
        if(m_board[x-1][y-1]==m_player) s++;
        if(m_board[x+1][y-1]==m_player) s++;
        if(m_board[x-1][y+1]==m_player) s++;
        if(m_board[x+1][y+1]==m_player) s++;
    }
    if(y-2>=0)
    {
        if(m_board[x][y-2]==m_player&&m_board[x][y-1]==0) b++;
    }
    if(x-2>=0)
    {
        if(m_board[x-2][y]==m_player&&m_board[x-1][y]==0) b++;
    }
    if(x+2<=8)
    {
        if(m_board[x+2][y]==m_player&&m_board[x+1][y]==0) b++;
    }
    if(y+2<=8)
    {
        if(m_board[x][y+2]==m_player&&m_board[x][y+1]==0) b++;
    }
    return s+b;
}

void netwindow::ai()
{
//    QElapsedTimer t;
//    t.start();
//    while(t.elapsed()<300);
    int x=0,y=0,flag=0,max=0;
    if(m_board[0][0]==0)
    {
        x = 0;
        y = 0;
        flag = 3;
    }
    else if(m_board[8][8]==0)
    {
        x = 8;
        y = 8;
        flag = 3;
    }
    else if(m_board[0][8]==0)
    {
        x = 0;
        y = 8;
        flag = 3;
    }
    else if(m_board[8][0]==0)
    {
        x = 8;
        y = 0;
        flag = 3;
    }
    else
    {
        time_t start_time = time(NULL); // 获取程序起始时间
        for(int i=0;i<9;i++)
        {
          for(int j=0;j<9;j++)
          {
            time_t current_time = time(NULL); // 获取当前时间
            double time_used = difftime(current_time, start_time); // 计算已用时间
            if(time_used >= 4.0) break;
            if(m_board[i][j]==0)
            {
                m_board[i][j]=m_player;
                int p = checkWin(i,j);
                m_board[i][j]=0;
                if(p!=0)//会死
                {
                    continue;
                }
                else
                {
                    int m = checkdiff(i,j);
                    int n = checksame(i,j);
                    if(m==3)
                    {
                        x=i;y=j;flag=1;
                        break;
                    }
                    else if(n>=1)
                    {
                        if(n>max)
                        {
                            max=n;
                            x=i;y=j;flag=2;
                            continue;
                        }
                    }
                }
            }
            else continue;
          }
          if(flag==1) break;
        }
    }
    if(flag==0)
    {
        time_t start_time = time(NULL); // 获取程序起始时间
        while(1)
        {
            time_t current_time = time(NULL); // 获取当前时间
            double time_used = difftime(current_time, start_time); // 计算已用时间
            if(time_used >= 4.0) break;
            x = rand()%9;
            y = rand()%9;
            if(m_board[x][y]==0)
            {
                m_board[x][y]=m_player;
                int p = checkWin(x,y);
                m_board[x][y]=n_player;
                int q = checkWin(x,y);
                if(p!=0)
                {
                    m_board[x][y]=0;
                    continue;
                }
                else if(p==0&&q==0)
                {
                    m_board[x][y]=0;
                    break;
                }
                else
                {
                    m_board[x][y]=0;
                    break;
                }
            }
            else continue;
        }
    }
    char ch = y + 65;
    QString _str = QString(ch) + QString::number(x+1);
    m_board[x][y] = m_player;
    if((set==1 && m_player==1)||(set==0 && m_player==2)) this->socket->send(NetworkData(OPCODE::MOVE_OP,_str,""));
    else this->server->send(lastOne,NetworkData(OPCODE::MOVE_OP,_str,""));
    player = 3 - player;
    update();
    resetTime();
}

void netwindow::receieveData(QTcpSocket* client, NetworkData data)//客户端发，服务端收
{
    lastOne=client;
    qDebug()<<"01";
    if(data.op==OPCODE::READY_OP)
    {

        if(readyflag==1)//我发过申请
        {
            if(ready==1)
            {
                m_player = 1;
                n_player = 2;
                set = 0;
                QMessageBox::information(this, tr("开始了！"), tr("对方同意了你的对局申请，你执黑先行！"));
                reply = 1;
            }

            if(ready==2)
            {
                m_player = 2;
                n_player = 1;
                set = 1;
                QMessageBox::information(this, tr("开始了！"), tr("对方同意了你的对局申请，你执白后行！"));
                reply = 1;
            }
        }
        else
        {
            QString str = data.data2;
            int a;
            if(str=="b") a = 1;
            else a = 2;
            qDebug()<<"00";
            //1b 2w
            if(a==1)
            {
                qDebug()<<"1111";
                int buttonClicked = QMessageBox::question(this, tr("ready"), tr("对方选择执黑先行"),
                                                          QMessageBox::Yes | QMessageBox::No,
                                                          QMessageBox::Yes);
                if (buttonClicked == QMessageBox::Yes)
                {
                    // 用户单击了 "是" 按钮
                    // 执行相应的操作
                    this->server->send(lastOne,NetworkData(OPCODE::READY_OP,"send by server",data.data2));
                    QMessageBox::information(this, tr("开始了！"), tr("你执白后行！"));
                    set = 1;//1客户端先行，0服务端先行
                    m_player = 2;
                    n_player = 1;
                    reply = 1;
                }
                else if (buttonClicked == QMessageBox::No)
                {
                    // 用户单击了 "否" 按钮
                    // 执行相应的操作
                    this->server->send(lastOne,NetworkData(OPCODE::REJECT_OP," ","send by server"));
                }

            }
            else if(a==2)
            {
                qDebug()<<"2222";
                int buttonClicked = QMessageBox::question(this, tr("ready"), tr("对方选择执白后行"),
                                                          QMessageBox::Yes | QMessageBox::No,
                                                          QMessageBox::Yes);
                if (buttonClicked == QMessageBox::Yes)
                {
                    // 用户单击了 "是" 按钮
                    // 执行相应的操作
                    this->server->send(lastOne,NetworkData(OPCODE::READY_OP,"send by server",data.data2));
                    QMessageBox::information(this, tr("开始了！"), tr("你执黑先行！"));
                    set = 0;//1客户端先行，0服务端先行
                    m_player = 1;
                    n_player = 2;
                    reply = 1;
                }
                else if (buttonClicked == QMessageBox::No)
                {
                    // 用户单击了 "否" 按钮
                    // 执行相应的操作
                    this->server->send(lastOne,NetworkData(OPCODE::REJECT_OP," ","send by server"));
                }
            }
        }
    }

        if(data.op==OPCODE::LEAVE_OP)
        {

        }
        if(data.op==OPCODE::MOVE_OP)
        {
            //提取x，y
            QString str = data.data1;
            file->open(QIODevice::WriteOnly|QIODevice::Append);
            QTextStream txtOutput(file);
            txtOutput << str << ' ';
            file->close();
            int x, y;
            // 过滤掉非字母部分
            QString letterPart = "";
            for (int i = 0; i < str.length(); i++) {
                if (str[i].isLetter()) {
                    letterPart.append(str[i]);
                }
            }
            if (letterPart.length() == 1 && str.length() > letterPart.length())
            {
                // 提取 x 和 y 的值
                y = letterPart.toLocal8Bit()[0] - 'A';
                x = str.mid(letterPart.length()).toInt()-1;
            }
            m_board[x][y] = n_player;
            player = 3 - player;
            update();
            resetTime();
            //判断胜负
            int ret = checkWin(x, y);
            if (ret == 1 && m_player == 1)
            {
                resettime();
                if(set==1)
                {
                    this->socket->send(NetworkData(OPCODE::SUICIDE_END_OP,"","send by client"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }
                else
                {
                    this->server->send(lastOne,NetworkData(OPCODE::SUICIDE_END_OP,"","send by server"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }
            }
            else if (ret == 2 && m_player == 2)//白子胜
            {
                resettime();
                if(set==0)
                {
                    this->socket->send(NetworkData(OPCODE::SUICIDE_END_OP,"","send by client"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }
                else
                {
                    this->server->send(lastOne,NetworkData(OPCODE::SUICIDE_END_OP,"","send by server"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }

            }
            if(aiflag==1)
            {
                ai( );
            }

        }
        if(data.op==OPCODE::TIMEOUT_END_OP)
        {
            if(timeoutflag==0)
            {
                QMessageBox::information(this, "Timeout", "执子时间超时，您输了！");
                file->open(QIODevice::WriteOnly|QIODevice::Append);
                QTextStream txtOutput(file);
                txtOutput << 'T';
                file->close();
                resetboard();
                this->server->send(lastOne,NetworkData(OPCODE::TIMEOUT_END_OP,"","send by server"));
            }
            else
            {
                if(set==0) QMessageBox::information(this, "Game Over", "您执黑子胜!");
                if(set==1) QMessageBox::information(this, "Game Over", "您执白子胜!");
                resetboard();
            }
        }

        if(data.op==OPCODE::GIVEUP_OP)
        {
            resettime();
            QMessageBox::information(this, "over", "对方认输了");
            file->open(QIODevice::WriteOnly|QIODevice::Append);
            QTextStream txtOutput(file);
            txtOutput << 'G';
            file->close();
            this->server->send(lastOne,NetworkData(OPCODE::GIVEUP_END_OP,"","send by server"));
        }
        if(data.op==OPCODE::GIVEUP_END_OP)
        {
            if(giveupflag==0)
            {
                if(set==0) QMessageBox::information(this, "Game Over", "您执黑子胜!");
                if(set==1) QMessageBox::information(this, "Game Over", "您执白子胜!");
                resetboard();
            }
            else
            {
                this->server->send(lastOne,NetworkData(OPCODE::GIVEUP_END_OP,"","send by server"));
                QMessageBox::information(this, "Game Over", "您输了!");
                resetboard();
            }
        }
        if(data.op==OPCODE::CHAT_OP)
        {
            serverGetEdit->setText(data.data1);
        }
        if(data.op==OPCODE::REJECT_OP)
        {
            readyflag = 0;
            QMessageBox::information(this, tr("被拒绝！"), tr("对方拒绝了你的对局申请！"));
        }
        if(data.op==OPCODE::SUICIDE_END_OP)
        {
            if(suicideflag==0)
            {
                resettime();
                int buttonClicked = QMessageBox::question(this, tr("gimeover"), tr("您输了！"),
                                                           QMessageBox::Yes | QMessageBox::No,
                                                           QMessageBox::Yes);
                if (buttonClicked == QMessageBox::Yes)
                {
                    // 用户单击了 "是" 按钮
                    // 执行相应的操作
                    if(set==1) QMessageBox::information(this, "Game Over", "对方黑子胜!");
                    if(set==0) QMessageBox::information(this, "Game Over", "对方白子胜!");
                    this->server->send(lastOne,NetworkData(OPCODE::SUICIDE_END_OP," ","send by server"));
                    resetboard();
                }
                else if (buttonClicked == QMessageBox::No)
                {
                    // 用户单击了 "否" 按钮
                    // 执行相应的操作
                    QMessageBox::information(this, "Game Over", "他妈的你输了还不承认是吧");
                }
            }
            else
            {
                QMessageBox::information(this, "Game Over", "对方落子判负");
                this->server->send(lastOne,NetworkData(OPCODE::SUICIDE_END_OP," ","send by server"));
                resetboard();
            }
        }
}

void netwindow::receieveDataFromServer(NetworkData data) //服务端发
{

        if(data.op==OPCODE::READY_OP)
        {
            if(readyflag==1)//我发过申请
            {
                if(ready==1)
                {
                    m_player = 1;
                    n_player = 2;
                    set = 1;
                    QMessageBox::information(this, tr("开始了！"), tr("对方同意了你的对局申请，你执黑先行！"));
                    reply = 1;
                }

                if(ready==2)
                {
                    m_player = 2;
                    n_player = 1;
                    set = 0;
                    QMessageBox::information(this, tr("开始了！"), tr("对方同意了你的对局申请，你执白后行！"));
                    reply = 1;
                }
            }
            else
            {
                QString str = data.data2;
                int a;
                if(str=="b") a = 1;
                else a = 2;
                qDebug()<<"00";
                //1b 2w
                if(a==1)
                {
                    qDebug()<<"1111";
                    int buttonClicked = QMessageBox::question(this, tr("ready"), tr("对方选择执黑先行"),
                                                              QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);
                    if (buttonClicked == QMessageBox::Yes)
                    {
                        // 用户单击了 "是" 按钮
                        // 执行相应的操作
                        this->socket->send(NetworkData(OPCODE::READY_OP,"send by socket",data.data2));
                        QMessageBox::information(this, tr("开始了！"), tr("你执白后行！"));
                        set = 0;//1客户端先行，0服务端先行
                        m_player = 2;
                        n_player = 1;
                        reply = 1;
                    }
                    else if (buttonClicked == QMessageBox::No)
                    {
                        // 用户单击了 "否" 按钮
                        // 执行相应的操作
                        this->socket->send(NetworkData(OPCODE::REJECT_OP," ","send by socket"));
                    }

                }
                else if(a==2)
                {
                    qDebug()<<"2222";
                    int buttonClicked = QMessageBox::question(this, tr("ready"), tr("对方选择执白后行"),
                                                              QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);
                    if (buttonClicked == QMessageBox::Yes)
                    {
                        // 用户单击了 "是" 按钮
                        // 执行相应的操作
                        this->socket->send(NetworkData(OPCODE::READY_OP,"send by socket",data.data2));
                        QMessageBox::information(this, tr("开始了！"), tr("你执黑先行！"));
                        set = 1;//1客户端先行，0服务端先行
                        m_player = 1;
                        n_player = 2;
                        reply = 1;
                    }
                    else if (buttonClicked == QMessageBox::No)
                    {
                        // 用户单击了 "否" 按钮
                        // 执行相应的操作
                        this->socket->send(NetworkData(OPCODE::REJECT_OP," ","send by socket"));
                    }
                }
            }
        }
        if(data.op==OPCODE::MOVE_OP)
        {
            //提取x，y
            QString str = data.data1;
            file->open(QIODevice::WriteOnly|QIODevice::Append);
            QTextStream txtOutput(file);
            txtOutput << str << ' ';
            file->close();
            int x, y;
            // 过滤掉非字母部分
            QString letterPart = "";
            for (int i = 0; i < str.length(); i++) {
                if (str[i].isLetter()) {
                    letterPart.append(str[i]);
                }
            }
            if (letterPart.length() == 1 && str.length() > letterPart.length())
            {
                // 提取 x 和 y 的值
                y = letterPart.toLocal8Bit()[0] - 'A';
                x = str.mid(letterPart.length()).toInt()-1;
            }
            m_board[x][y] = n_player;
            player = 3 - player;
            update();
            resetTime();
            //判断胜负
            int ret = checkWin(x, y);
            if (ret == 1 && m_player == 1)
            {
                resettime();
                if(set==1)
                {
                    this->socket->send(NetworkData(OPCODE::SUICIDE_END_OP,"","send by client"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }
                else
                {
                    this->server->send(lastOne,NetworkData(OPCODE::SUICIDE_END_OP,"","send by server"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }

            }
            else if (ret == 2 && m_player == 2)//白子胜
            {
                resettime();
                if(set==0)
                {
                    this->socket->send(NetworkData(OPCODE::SUICIDE_END_OP,"","send by client"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }
                else
                {
                    this->server->send(lastOne,NetworkData(OPCODE::SUICIDE_END_OP,"","send by server"));
                    QMessageBox::information(this, "Wainting", "对方落子判负，等待对方认输中");
                    player = 0;
                }

            }
            if(aiflag==1)
            {
                ai( );
            }
        }
        if(data.op==OPCODE::TIMEOUT_END_OP)
        {
            if(timeoutflag==0)
            {
                QMessageBox::information(this, "Timeout", "执子时间超时，您输了！");
                file->open(QIODevice::WriteOnly|QIODevice::Append);
                QTextStream txtOutput(file);
                txtOutput << 'T';
                resetboard();
                this->socket->send(NetworkData(OPCODE::TIMEOUT_END_OP,"","send by client"));
            }
            else
            {
                if(set==1) QMessageBox::information(this, "Game Over", "您执黑子胜!");
                if(set==0) QMessageBox::information(this, "Game Over", "您执白子胜!");
                resetboard();
            }
        }
        if(data.op==OPCODE::GIVEUP_OP)
        {
            resettime();
            QMessageBox::information(this, "over", "对方认输了");
            file->open(QIODevice::WriteOnly|QIODevice::Append);
            QTextStream txtOutput(file);
            txtOutput << 'G';
            this->socket->send(NetworkData(OPCODE::GIVEUP_END_OP," ","send by socket"));
        }
        if(data.op==OPCODE::GIVEUP_END_OP)
        {
            if(giveupflag==0)
            {
                if(set==1) QMessageBox::information(this, "Game Over", "您执黑子胜!");
                if(set==0) QMessageBox::information(this, "Game Over", "您执白子胜!");
                resetboard();
            }
            else
            {
                this->socket->send(NetworkData(OPCODE::GIVEUP_END_OP," ","send by socket"));
                QMessageBox::information(this, "Game Over", "您输了!");
                resetboard();
            }
        }
        if(data.op==OPCODE::CHAT_OP)
        {
            clientGetEdit->setText(data.data1);
        }
        if(data.op==OPCODE::REJECT_OP)
        {
            readyflag = 0;
            QMessageBox::information(this, tr("被拒绝！"), tr("对方拒绝了你的对局申请！"));
        }
        if(data.op==OPCODE::SUICIDE_END_OP)
        {
            if(suicideflag==0)
            {
                resettime();
                int buttonClicked = QMessageBox::question(this, tr("gimeover"), tr("您输了！"),
                                                           QMessageBox::Yes | QMessageBox::No,
                                                           QMessageBox::Yes);
                if (buttonClicked == QMessageBox::Yes)
                {
                    // 用户单击了 "是" 按钮
                    // 执行相应的操作
                    if(set==0) QMessageBox::information(this, "Game Over", "对方黑子胜!");
                    if(set==1) QMessageBox::information(this, "Game Over", "对方白子胜!");
                    this->socket->send(NetworkData(OPCODE::SUICIDE_END_OP," ","send by socket"));
                    resetboard();
                }
                else if (buttonClicked == QMessageBox::No)
                {
                    // 用户单击了 "否" 按钮
                    // 执行相应的操作
                    QMessageBox::information(this, "Game Over", "他妈的你输了还不承认是吧");
                }
            }
            else
            {
                QMessageBox::information(this, "Game Over", "对方落子判负");
                this->socket->send(NetworkData(OPCODE::SUICIDE_END_OP," ","send by socket"));
                resetboard();
            }
        }
}

