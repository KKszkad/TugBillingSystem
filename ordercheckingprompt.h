#ifndef ORDERCHECKINGPROMPT_H
#define ORDERCHECKINGPROMPT_H

#include "InvoicePart.h"
#include "tcmainwindow.h"

#include <QWidget>
#include <QCloseEvent>

// 复核员双击弹出的小窗口

namespace Ui {
class OrderCheckingPrompt;
}

class OrderCheckingPrompt : public QWidget
{
    Q_OBJECT

public:
    explicit OrderCheckingPrompt(const QString& userName, const QString& applyId, const QString& status, QWidget *parent = nullptr);
    ~OrderCheckingPrompt();

private:
    Ui::OrderCheckingPrompt *ui;

    QString applyId;
    QString userName;
    QString status;
    
    InvoicePart invoice;
    // 用于判断复核员是否是在审核计费员的计费申请
    // 1：审核计费申请
    // 0：查看船代已经同意的订单 为其打印发票

    void initiate();
    void showCostInfo(QJsonObject& responseInfo);
    void produceExcelFile(QJsonObject& responseInfo);

    void closeEvent(QCloseEvent *event) override;//NOTE 这个关闭事件可以触发

public slots:
    void on_confirm_clicked();
    void on_deny_clicked();
    void on_getCheck_clicked();
    void on_cancel_clicked();
};

#endif // ORDERCHECKINGPROMPT_H
