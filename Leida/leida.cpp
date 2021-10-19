#include "leida.h"
#include "ui_leida.h"

#include <QMessageBox>
#include <QScreen>
#include <QFont>

#include <QTcpSocket>
#include <QHostAddress>

#include <Windows.h>
#include <WinUser.h>

Leida::Leida(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Leida),
    recvThread(new RecvDataCtl),
    timer(new QTimer),
    procScan(new QProcess),
    model(new QStandardItemModel())
{
    ui->setupUi(this);

    initUI();
    initConnect();
    // initScanRadar();
    timer->start(75);
    qInfo() << "[init] start timer: 75ms per cycle.";
}

Leida::~Leida()
{
    delete ui;
}

// 关闭事件过滤器
void Leida::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button = QMessageBox::question(this, QStringLiteral("退出"), QStringLiteral("确认退出程序？"),QMessageBox::Yes|QMessageBox::No);
    if(QMessageBox::No == button)
    {
        event->ignore();
    }
    else if(QMessageBox::Yes==button)
    {
        qInfo() << "user closed app.";
        qInfo() << "APP STOP...";
        event->accept();
    }
}

void Leida::initUI()
{
    // 自适应分辨率
    QRect rect = QGuiApplication::primaryScreen()->geometry();
    this->setFixedSize(rect.width(), rect.height());
    // 全屏显示
    this->showFullScreen();
    // 标题
    this->setWindowTitle(QStringLiteral("雷达监测v3.0"));

    // 设置UI
    // 限制Group大小
    ui->groupSet->setMaximumSize(ui->centralWidget->width() / 2, ui->centralWidget->width() / 2);
    ui->groupData->setMaximumSize(ui->centralWidget->width() / 2, ui->centralWidget->width() / 2);

    // 设置默认IP
    ui->addrInput->settext("192.168.1.232");
    // ui->addrInput->settext("127.0.0.1");
    qInfo() << "[init] ip default value: 192.168.1.232";

    // 设置默认端口
    ui->portInput->setValidator(new QIntValidator(0, 65535, this)); // 限制仅数字及输入范围
    ui->portInput->setText("8889");
    qInfo() << "[init] port default value: 8889";

    // 横向范围
    ui->xInput->setRange(10, 100);
    ui->xInput->setValue(50);
    qInfo() << "[init] x range: [10, 100], default value: 50";

    // 纵向范围
    ui->yInput->setRange(5, 100);
    ui->yInput->setValue(50);
    qInfo() << "[init] y range: [5, 100], default value: 50";

    initPlot(ui->viewWidget);

    ui->viewWidget->xAxis->setRange((ui->xInput->text().toDouble() / 2.0) * (-1), (ui->xInput->text().toDouble() / 2.0));
    ui->viewWidget->yAxis->setRange(0,ui->yInput->text().toDouble());

    initTableView();
}

void Leida::initPlot(QCustomPlot *qCustomPlot)
{
    qCustomPlot->xAxis->setLabel(QStringLiteral("水平方向"));
    qCustomPlot->yAxis->setLabel(QStringLiteral("垂直方向"));

    qCustomPlot->xAxis->setRange(ui->xInput->value() * (-1), ui->xInput->value());
    qCustomPlot->yAxis->setRange(0, ui->yInput->value());

    qCustomPlot->axisRect()->setBackground(QBrush(Qt::white));
    qCustomPlot->xAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));
    qCustomPlot->yAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));

    qCustomPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::black));
    qCustomPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::black));

    qCustomPlot->xAxis->ticker()->setTickCount(14);
    qCustomPlot->yAxis->ticker()->setTickCount(20);

    ui->viewWidget->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability); // 可读性优于设置

    qCustomPlot->addGraph(qCustomPlot->yAxis, qCustomPlot->xAxis);
}

void Leida::initTableView()
{
    model->setColumnCount(7);
    model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("ID"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("X坐标（单位：m）"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Y坐标（单位：m）"));
    model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("X速度（单位：m/s）"));
    model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Y速度（单位：m/s）"));
    model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("RCS（单位：dBm2）"));
    model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("置信度（单位：%）"));

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    // 设置列宽
    int col1_width;
    int double_col1_width = ui->tableView->width() - (ui->tableView->width() / 7) * (7 - 1);
    if(double_col1_width % 2 == 0)
    {
        col1_width = double_col1_width / 2;
    }
    else
    {
        col1_width = double_col1_width / 2 + 1;
    }

    for(int i = 0; i < 7; i++)
    {
        if(i == 0)
        {
            ui->tableView->setColumnWidth(i, col1_width);
        }
        else
        {
            ui->tableView->setColumnWidth(i, (ui->tableView->width() - col1_width) / 6);
        }
        // 固定列宽：不可拖动。
        ui->tableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
    }
}

/*
void Leida::initScanRadar()
{
    QString scanExeStr = "LidarViewer.exe";
    procScan = new QProcess();
    procScan->start("tasklist");
    procScan->waitForFinished();                    //等待进程结束
    QString processLists = procScan->readAllStandardOutput();//获取执行命令后的输出内容
    if(processLists.contains(scanExeStr))
    {
        procScan->execute("TASKKILL /IM LidarViewer.exe /F");
    }
    procScan->start("E:\\Workspace\\TASKS\\Others\\210816_LeQingRadar\\PROJ\\Leida\\LiderViewer\\LidarViewer.exe",QStringList());

    // 设置扫描雷达软件窗口名和标题
    QString windowName = QStringLiteral("Qt5QWindowIcon");
    QString titleName = QStringLiteral("雷达客户端 v0.1.9");
    // 需要增加延时才能获取到句柄。
    Sleep(50);
    // 获取扫描雷达软件句柄
    HWND hwind = FindWindow((LPCWSTR)windowName.unicode(),(LPCWSTR)titleName.unicode());//查找窗口句柄
    if(!hwind)
    {
        QMessageBox::information(this,tr("Warning"),tr("can't get LidarViewer Window Handle!"));
    }
    else
    {
        qDebug() << "Get LidarViewer Window Handle!";

    }

    Sleep(500);
    WId wid = (WId)FindWindow(L"Qt5QWindowIcon", NULL);
    //WId wid = (WId)FindWindow(L"Rgui Workspace",NULL);
    QWindow *scan_window;
    scan_window = QWindow::fromWinId(wid);
    //scan_window->setFlags(scan_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint); //

    qDebug() << "i'm here.";

    QWidget *m_widget;
    m_widget = QWidget::createWindowContainer(scan_window, this->ui->scanWidget);
    m_widget->setMinimumSize(400,300);

    HWND hwnd = ::FindWindow(L"Qt5QWindowIcon", NULL);
    QWindow* native_wnd  = QWindow::fromWinId((WId)hwnd);
    ui->scanWidget = QWidget::createWindowContainer(native_wnd);

    QWidget* native_wdgt = QWidget::createWindowContainer(native_wnd);
    this->ui->centralWidget->addSubWindow(native_wdgt);

    native_wdgt->setMinimumSize(600,400);

    QString cmd="C:/Windows/system32/calc.exe";
    STARTUPINFO si={sizeof(si)};
    PROCESS_INFORMATION pi;
    si.dwFlags=STARTF_USESHOWWINDOW;
    si.wShowWindow=true;

    bool bRet=CreateProcess(
        NULL,
        (LPWSTR)cmd.toStdWString().c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,&si,&pi);

    Sleep(50);

    WId wid = (WId)FindWindow(L"CalcFrame",NULL);
    //WId wid = (WId)FindWindow(L"Rgui Workspace",NULL);
    QWindow *m_window;
    m_window = QWindow::fromWinId(wid);
    //m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint); //

    QWidget *m_widget;
    m_widget = QWidget::createWindowContainer(m_window,this->ui->scanWidget);
    m_widget->setMinimumSize(400,300);
}
*/

void Leida::initConnect()
{
    // 开始/停止
    connect(ui->startBtn, &QPushButton::clicked, this, [=](){
        if(!this->isRunning)
        {
            // 获取IP
            QString ip, ip1, ip2, ip3, ip4;
            ui->addrInput->getIp(ip1, ip2, ip3, ip4);
            ip = ip1 + "." + ip2 + "." + ip3 + "." + ip4;

            // 获取PORT
            quint16 port;
            port = ui->portInput->text().toUShort();

            // 非法IP过滤
            if((ip1.toInt() > 255) || (ip2.toInt() > 255) || (ip3.toInt() > 255) || (ip4.toInt() > 255))
            {
                QMessageBox::critical(this, QStringLiteral("非法IP"), QStringLiteral("IP地址不合法，请重新输入！"), QMessageBox::Yes, QMessageBox::Yes);
                qWarning() << "start read failed: type of ip isn't correct.";
                return;
            }

            this->isNotRecved = true;

            // 建立连接（IPv4），获取套接字。
            if (tcpSocket != nullptr)
            {
                delete tcpSocket;
                tcpSocket = nullptr;
            }
            tcpSocket = new QTcpSocket();
            tcpSocket->connectToHost(ip, port, QAbstractSocket::ReadWrite, QAbstractSocket::IPv4Protocol);

            if(!tcpSocket->waitForConnected(500))
            {
                QMessageBox::critical(this, QStringLiteral("连接失败"), QStringLiteral("服务器不存在！"), QMessageBox::Yes, QMessageBox::Yes);
                qWarning() << "start read failed: tcp connection failed, " << ip << port;
                return;
            }

            // 启动子线程：读数据。
            qInfo() << "start read success: " << ip << port;
            emit this->recvThread->start(this->tcpSocket);

            // 更新界面
            ui->startBtn->setText(QStringLiteral("停止"));
            this->isRunning = true;

            ui->viewWidget->xAxis->setRange((ui->xInput->text().toDouble() / 2.0) * (-1), (ui->xInput->text().toDouble() / 2.0));
            ui->viewWidget->yAxis->setRange(0,ui->yInput->text().toDouble());

            // 禁用设置
            ui->addrInput->setEnabled(false);
            ui->portInput->setEnabled(false);
            ui->xInput->setEnabled(false);
            ui->yInput->setEnabled(false);
        }
        else
        {
            // 向子线程发送停止信号
            qInfo() << "stop read: user oprate.";
            emit this->recvThread->stop();
            // 更新按钮文字
            ui->startBtn->setText(QStringLiteral("开始"));
            // 更新运行标志
            this->isRunning = false;
            this->isNotRecved = false;
            // 刷新界面
            ui->viewWidget->clearGraphs();
            ui->viewWidget->replot();
            // 启用设置
            ui->addrInput->setEnabled(true);
            ui->portInput->setEnabled(true);
            ui->xInput->setEnabled(true);
            ui->yInput->setEnabled(true);
        }
    });

    // 定时器：刷新界面
    connect(timer, &QTimer::timeout, this, [=](){
        if(this->isNotRecved)
        {
            ui->viewWidget->clearGraphs();
            ui->viewWidget->replot();
        }
        else
        {
            this->isNotRecved = true;
        }
        if(!this->isRunning)
        {
            ui->viewWidget->clearGraphs();
            ui->viewWidget->replot();
        }
        // 使用写socket判断连接是否断开
        if((this->tcpSocket != nullptr) && (this->tcpSocket->write("") == -1) && this->isRunning)
        {
            qInfo() << "stop read: tcp has disconnected, clear interface data!";
            emit this->recvThread->stop();
            QMessageBox::critical(this, QStringLiteral("TCP连接断开"), QStringLiteral("TCP连接断开，请检查！"), QMessageBox::Yes, QMessageBox::Yes);
            this->isRunning = false;
            // 刷新界面
            ui->viewWidget->clearGraphs();
            ui->viewWidget->replot();
            ui->startBtn->setText(QStringLiteral("开始"));
            ui->addrInput->setEnabled(true);
            ui->portInput->setEnabled(true);
            ui->xInput->setEnabled(true);
            ui->yInput->setEnabled(true);
            this->tcpSocket = nullptr;
            return;
        }
    });

    // 永远不会触发，使用心跳检测代替。
    /*
    connect(this->tcpSocket, &QTcpSocket::disconnected, this, [=](){
        emit this->recvThread->stop();
        QMessageBox::critical(this, QStringLiteral("TCP连接断开"), QStringLiteral("TCP连接断开，请检查！"), QMessageBox::Yes, QMessageBox::Yes);
        this->isRunning = false;
    });
    */

    // 更新数据
    // 跨线程信号中携带数据需要注册
    qRegisterMetaType<QVector<UNIT_ELE>>("QVector<UNIT_ELE>");
    connect(this->recvThread, &RecvDataCtl::returnReady, this, [=](QVector<UNIT_ELE> vec){
        if(lastCount > 0)
        {
            model->removeRows(0, lastCount - 1);
        }
        UNIT_ELE_VIEW ele_view = {};
        QVector<double_t> posX = {};
        QVector<double_t> posY = {};
        for (int i = 0; i < vec.length(); i++)
        {
            UNIT_ELE ele = vec.at(i);
            qDebug() << "[interface] recv data: "  << ele.id << ele.x_pos << ele.y_pos << ele.x_speed << ele.y_speed << ele.rcs << ele.conf;
            ele_view = {};

            if(ele.id == 0)
            {
                continue;
            }

            ele_view.x_pos = QString::number((static_cast<double_t>(ele.x_pos)) / 100);
            ele_view.y_pos = QString::number((static_cast<double_t>(ele.y_pos)) / 100);
            ele_view.x_speed = QString::number((static_cast<double_t>(ele.x_speed)) / 100);
            ele_view.y_speed = QString::number((static_cast<double_t>(ele.y_speed)) / 100);
            ele_view.rcs = QString::number(ele.rcs);
            ele_view.conf = QString::number((static_cast<double_t>(ele.conf)) / 100);

            posX.append((static_cast<double_t>(ele.x_pos)) / 100);
            posY.append((static_cast<double_t>(ele.y_pos)) / 100);

            model->setItem(i, 0, new QStandardItem(ele_view.id));
            model->setItem(i, 1, new QStandardItem(ele_view.x_pos));
            model->setItem(i, 2, new QStandardItem(ele_view.y_pos));
            model->setItem(i, 3, new QStandardItem(ele_view.x_speed));
            model->setItem(i, 4, new QStandardItem(ele_view.y_speed));
            model->setItem(i, 5, new QStandardItem(ele_view.rcs));
            model->setItem(i, 6, new QStandardItem(ele_view.conf));

            int col1_width;
            int double_col1_width = ui->tableView->width() - (ui->tableView->width() / 7) * (7 - 1);
            if(double_col1_width % 2 == 0)
            {
                col1_width = double_col1_width / 2;
            }
            else
            {
                col1_width = double_col1_width / 2 + 1;
            }

            for(int i = 0; i < 7; i++)
            {
                if(i == 0)
                {
                    ui->tableView->setColumnWidth(i, col1_width);
                }
                else
                {
                    ui->tableView->setColumnWidth(i, (ui->tableView->width() - col1_width) / 6);
                }
                // 固定列宽：不可拖动。
                ui->tableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
            }

        }

        lastCount = vec.length();

        this->isNotRecved = false;
        ui->viewWidget->clearGraphs();
        QPen drawPen;
        drawPen.setColor(Qt::red);
        drawPen.setWidth(1);
        QCPGraph * curGraph = ui->viewWidget->addGraph();
        curGraph->setPen(drawPen);
        curGraph->setLineStyle(QCPGraph::lsNone);
        //curGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
        curGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 10));
        curGraph->setData(posX, posY);
        ui->viewWidget->replot();
    });
}
