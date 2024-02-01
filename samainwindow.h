#pragma once
#ifndef SAMAINWINDOW_H
#define SAMAINWINDOW_H

#include "TugApplication.h"

#include <QMainWindow>
#include <QTimer>

//船代窗口

namespace Ui {
class SAMainWindow;
}

class SAMainWindow : public QMainWindow
{
    Q_OBJECT

public slots:

public:
    QString userName;

    explicit SAMainWindow(const QString& userName, QWidget *parent = nullptr);
    ~SAMainWindow();

    //船代申请时需要看到的数据
    void showShipInfo(QJsonObject &shipInfo);
    void showWorkInfo(QJsonObject &workInfo);
//    void showPorts(QJsonObject &ports);
    void showPorts();

    //船代的数据展示部分
    void initOrdersTable();
    void agencyRequest();
    void showOrdersTable(QJsonObject& orderObj);

private:
    Ui::SAMainWindow *ui;

    //分页参数
    int pageSize = 10;
    int totalPage;
    int currentPage = 1;

    //计时器
    QTimer* timer;

    //自定义的结构体 包含拖轮申请所需的全部数据
    TugApplication application;


public slots:
    // 拖轮申请部分按钮按下的触发事件
    void on_findShip_clicked();
    void on_confirmShip_clicked();
    void on_confirmWorks_clicked();
    void on_submitApplication_clicked();

    // 船代的数据显示部分按钮按下的触发事件
    void on_lastPage_clicked();
    void on_nextPage_clicked();
    void on_logOut_clicked();
};

#endif // SAMAINWINDOW_H
