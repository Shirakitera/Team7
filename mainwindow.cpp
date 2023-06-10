#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QLabel>
#include <QElapsedTimer>
#include <QtDebug>
#include <QPropertyAnimation> //QPropertyAnimation类头文件
#include <QEasingCurve> //QEasingCurve类头文件（可选）
#include <QRadioButton>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    setFixedSize(800, 600);//窗口大小
    memset(m_board, 0, sizeof(m_board));
    m_player = 1;
    n_player = 1;

    QWidget* widget = new QWidget(this);
    setCentralWidget(widget);

    btnRePlay = new QPushButton("再来一局", this);
    btnGiveUp = new QPushButton("认输", this);
    btnReshow = new QPushButton("复现",this);
    btnLastStep = new QPushButton("上一步",this);
    btnNextStep = new QPushButton("下一步",this);
    btnStop = new QPushButton("播放",this);

    QVBoxLayout* vLayout = new QVBoxLayout;//垂直布局
    vLayout->addWidget(btnRePlay);
    vLayout->addWidget(btnGiveUp);
    btnRePlay->setFixedSize(QSize(80, 40));//按钮大小
    btnRePlay->move(600, 550);//按钮位置

    btnGiveUp->setFixedSize(QSize(80, 40));
    btnGiveUp->move(600, 500);

    btnReshow->setFixedSize(QSize(160,60));
    btnLastStep->setFixedSize(QSize(50,40));
    btnNextStep->setFixedSize(QSize(50,40));
    btnStop->setFixedSize(QSize(40,40));

    btnReshow->move(600,390);
    btnLastStep->move(600,450);
    btnStop->move(660,450);
    btnNextStep->move(710,450);
    connect(btnReshow, &QPushButton::clicked, this, [=](){
        QString path = QFileDialog::getOpenFileName(this,"选取文件","/Users/zhangjinkai/Desktop");
        file = new QFile(path);
        file->open(QIODevice::ReadOnly);
        char * buffer = new char[15];
        int length = 0;
        while((length = file->read(buffer,3)) != 0)
        {
            int x = buffer[0] - 'A';
            int y = buffer[1] - '1';
            m_board[y][x] = m_player;
            m_player = 3 - m_player;
            Delay_MSec(200);
            update();
//            QElapsedTimer t;
//            t.start();
//            while(t.elapsed() < 200);
            continue;
        }
//        for(int i = 0;;i+=3)
//        {
//            file.read(buffer,3);
//            if(buffer[1] < '0')
//                break;
//            int x = buffer[0] - 'A';
//            int y = buffer[1] - '1';
//            m_board[y][x] = m_player;
//            m_player = 3 - m_player;
//            //Delay_MSec(200);
//        }
    });
    connect(btnStop,&QPushButton::clicked,this,[=](){
        if(btnStop->text() == "暂停")
        {
            btnStop->setText("播放");
        }
        if(btnStop->text() == "播放")
        {
            btnStop->setText("暂停");
        }
    });
    connect(btnNextStep,&QPushButton::clicked,this,[=](){
//        char * buffer = new char[15];
//        file->read(buffer,3);
//        int x = buffer[0] - 'A';
//        int y = buffer[1] - '1';
//        m_board[y][x] = m_player;
//        m_player = 3 - m_player;
    });
    connect(btnRePlay, &QPushButton::clicked, this, [=](){
        // 点击“再来一局”按钮后的功能实现
        memset(m_board, 0, sizeof(m_board));
        m_player = 1;
        n_player = 2;
        resettime();
        update();
    });

    connect(btnGiveUp, &QPushButton::clicked, this, [=](){
        // 点击“认输”按钮后的功能实现
        if (n_player == 1){
            QMessageBox::information(this, "Game Over", "White Win!");
        } else {
            QMessageBox::information(this, "Game Over", "Black Win!");
        }
        memset(m_board, 0, sizeof(m_board));
        m_player = 1;
        n_player = 2;
        resettime();
        update();
    });



        m_timeLabel = new QLabel("00:00:00", this);
        m_timetext = new QLabel("黑子第一手不计时", this);
        m_resetButton = new QPushButton("我下好了", this);
        m_timer = new QTimer(this);
        m_remainingTime = 15; // 初始倒计时为60秒

        // 将定时器的timeout信号连接到updateRemainingTime槽函数
        connect(m_timer, &QTimer::timeout, this, &MainWindow::updateRemainingTime);

        // 将重置按钮的clicked信号连接到resetTime槽函数
        connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::resetTime);

        // 设置界面布局
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(m_timeLabel);
        layout->addWidget(m_timetext);
        layout->addWidget(m_resetButton);

        m_resetButton->setFixedSize(QSize(200, 80));//按钮大小
        m_resetButton->move(590, 200);//按钮位置

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


//        QLabel *label = new QLabel(this);
//        QPixmap blackpixmap("C:\\Users\\1\\Desktop\\微信图片_20230601184202.jpg");
//        blackpixmap = blackpixmap.scaled(QSize(100,100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//        label->setPixmap(blackpixmap);
//        label->resize(200,200);
//        label->move(100,100);
//        label->show();

        QRadioButton* btn9 = new QRadioButton("9X9", this);
        btn9->setFixedSize(QSize(80,40));
        btn9->move(700,300);
        QRadioButton* btn11 = new QRadioButton("11X11", this);
        btn11->setFixedSize(QSize(80,40));
        btn11->move(700,320);
        QRadioButton* btn13 = new QRadioButton("13X13", this);
        btn13->setFixedSize(QSize(80,40));
        btn13->move(700,340);

        m_buttonGroup = new QButtonGroup(this);
        m_buttonGroup->addButton(btn9, 9);//
        m_buttonGroup->addButton(btn11, 11);//
        m_buttonGroup->addButton(btn13, 13);//
        connect(m_buttonGroup, QOverload<int>::of(&QButtonGroup::idClicked),[=](int id)
        { board = id; update();});
//        label->setGeometry(0, 0, 100, 100);

//        QPropertyAnimation *animation = new QPropertyAnimation(label, "size", this);
//        animation->setDuration(1000);
//        位移
//        animation->setStartValue(QPoint(0, 0));
//        animation->setEndValue(QPoint(250, 250));
//        animation->setStartValue(QRect(0, 0, 200, 200));   // 设置初始状态的矩形区域（大小为 200x200，位置在左上角）
//        animation->setEndValue(QRect(0, 0, 100, 100));     // 设置结束状态的矩形区域（大小为 100x100，位置在左上角）
//        animation->setEasingCurve(QEasingCurve::InOutQuad);// 设置缓和曲线，这里使用了 QEasingCurve::InOutQuad
//        animation->start();
}


MainWindow::~MainWindow()
{
    delete m_timeLabel;
    delete m_resetButton;
    delete m_timer;
    delete ui;
}
void MainWindow::Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//抗锯齿
    painter.setPen(QPen(QColor(0, 0, 0)));//棋盘色

    //背景图
    QPixmap pixmap("C:\\Users\\1\\Desktop\\16pic_3055470_b.jpg");
    pixmap = pixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(rect(), pixmap);

    //画棋盘
    for (int i = 0; i < board; i++)
    {
        painter.drawLine(QPoint(20, 20 + i * 40), QPoint((board-1)*40+20, 20 + i * 40));
        painter.drawLine(QPoint(20 + i * 40, 20), QPoint(20 + i * 40, (board-1)*40+20));
    }

    for (int i = 0; i < board; i++)
    {
        for (int j = 0; j < board; j++)
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

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {

        if(m_player != n_player)
        {
            QMessageBox::information(this, "Warning!", "现在不该你下棋！");

        }

        else{
        int x = event->position().x() / 40;
        int y = event->position().y() / 40;
        if (m_board[x][y] == 0)
        {
            m_board[x][y] = m_player;
            m_player = 3 - m_player;

            int ret = checkWin(x, y);
            if (ret == 1)
            {
                m_player = 1;
                n_player = 2;
                resettime();
                QMessageBox::information(this, "Game Over", "黑子胜!");
                memset(m_board, 0, sizeof(m_board));

            }
            else if (ret == 2)
            {
                m_player = 1;
                n_player = 2;
                resettime();
                QMessageBox::information(this, "Game Over", "白子胜!");
                memset(m_board, 0, sizeof(m_board));

            }

            update();
        }
        }

    }
    //totalTime=15;
}
int MainWindow::checkWin(int x, int y)
{
    //不围棋
    is_dead = 1;
    dfs(x, y); init();//判断自杀
    if(is_dead) return 3 - m_board[x][y];//死了

    for(int i = 0; i < 4; i++){
        int nx = x + dx[i], ny = y + dy[i];
        if(nx < 0 || nx >= board || ny < 0 || ny >= board || st[nx][ny]) continue;
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

void MainWindow::dfs(int x, int y)
{
    for(int i = 0; i < 4; i++){
        int nx = x + dx[i], ny = y + dy[i];
        if(nx < 0 || nx >= board || ny < 0 || ny >= board || st[nx][ny]) continue;//四周是边界/标记过的同色子
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

void MainWindow::init(){
    memset(st, 0, sizeof(st));//归零st
}



void MainWindow::updateRemainingTime()
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
        if(m_player==1)
        {
            QMessageBox::information(this, "Time's up", "黑子执子时间到，白子胜！");
            memset(m_board, 0, sizeof(m_board));
            m_player = 1;
            n_player = 2;
            resettime();
        }
        else if(m_player==2)
        {
            QMessageBox::information(this, "Time's up", "白子执子时间到，黑子胜！");
            memset(m_board, 0, sizeof(m_board));
            m_player = 1;
            n_player = 2;
            resettime();
        }
    }
}

void MainWindow::resetTime()
{
    // 停止定时器并重置倒计时
    if(m_player==n_player)
    {
        QMessageBox::information(this, "Warning!", "请你先下棋！");
    }
    else{
    n_player = 3 - n_player;
    m_timer->stop();
    m_remainingTime = 15;
    m_timeLabel->setText("00:00:15");
    // 启动定时器，每秒更新一次倒计时
    if(m_player==1) m_timetext->setText("黑子执子剩余时间");
    else m_timetext->setText("白子执子剩余时间");
    m_timer->start(1000);
    }
}

void MainWindow::resettime()
{
    n_player = 3 - n_player;
    m_timer->stop();
    m_remainingTime = 15;
    m_timeLabel->setText("00:00:00");
    m_timetext->setText("黑子第一手不计时");
}



