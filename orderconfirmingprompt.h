#ifndef ORDERCONFIRMINGPROMPT_H
#define ORDERCONFIRMINGPROMPT_H

//船代双击记录弹出的小窗口

#include <QWidget>

namespace Ui {
class OrderConfirmingPrompt;
}

class OrderConfirmingPrompt : public QWidget
{
    Q_OBJECT

public:
    explicit OrderConfirmingPrompt(const QString& applyId, QWidget *parent = nullptr);
    ~OrderConfirmingPrompt();

private:
    Ui::OrderConfirmingPrompt *ui;

    //船代同意需要的id号 从父窗口那里获得
    QString applyId;

    void initiate();
    void showCostInfo(QJsonObject& responseInfo);
    void closeEvent(QCloseEvent *event) override;

public slots:
    void on_agree_clicked();
    void on_disagree_clicked();
    void on_cancel_clicked();
};

#endif // ORDERCONFIRMINGPROMPT_H
