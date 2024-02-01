#pragma once
#ifndef TCMAINWINDOW_H
#define TCMAINWINDOW_H


#include <QMainWindow>
#include <QTimer>

//拖轮公司人员窗口
//这个窗口是叠加的 设置index可以确定是谁的   0计费员    1复核员

namespace Ui {
class TCMainWindow;
}

class TCMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TCMainWindow(const QString& userName, const int& position, QWidget *parent = nullptr);
    ~TCMainWindow();

    // 计费员的数据展示部分
    void initCashierTable();
    void cashierRequest();
    void showBillsInfo(QJsonObject& billObj);

    // 复核员的数据展示部分
    void initAuditorTable();
    void auditorRequest();
    void showChecksInfo(QJsonObject& checkObj);

private:
    Ui::TCMainWindow *ui;

    QString userName;
    int pageSize = 10;
    int currentPage = 1;
    int totalPage;

    QTimer* timer;


public slots:
    // 计费员部分的翻页按键
    void on_lastPage0_clicked();
    void on_nextPage0_clicked();

    // 复核员部分的翻页按键
    void on_lastPage1_clicked();
    void on_nextPage1_clicked();

    //退出登录
    void on_logOut0_clicked();
    void on_logOut1_clicked();
};




#endif // TCMAINWINDOW_H
