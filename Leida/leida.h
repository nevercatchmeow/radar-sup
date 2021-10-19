#ifndef LEIDA_H
#define LEIDA_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QProcess>

#include "recvdata.h"
#include "qcustomplot.h"

namespace Ui {
class Leida;
}

class Leida : public QMainWindow
{
    Q_OBJECT

public:
    explicit Leida(QWidget *parent = nullptr);
    ~Leida();

protected:

private:
    Ui::Leida *ui;
    bool isRunning = false;
    bool isNotRecved = true;
    QTcpSocket *tcpSocket = nullptr;
    RecvDataCtl *recvThread = nullptr;
    QTimer *timer = nullptr;
    QProcess *procScan = nullptr;
    QStandardItemModel *model = nullptr;

    int lastCount = 0;
private:
    // 关闭事件过滤器
    void closeEvent(QCloseEvent *event);
    // 初始化
    void initUI();
    // 初始化QCustomPlot
    void initPlot(QCustomPlot *qCustomPlot);
    // 初始化TableView
    void initTableView();
    // 初始化扫描雷达
    void initScanRadar();
    // 初始化槽函数
    void initConnect();
};

#endif // LEIDA_H
