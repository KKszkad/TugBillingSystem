#ifndef COSTEDITINGPROMPT_H
#define COSTEDITINGPROMPT_H

#include <QWidget>

// 计费员弹出的小窗口 可以看到所有的工作 并且编辑费用

namespace Ui {
class CostEditingPrompt;
}

class CostEditingPrompt : public QWidget
{
    Q_OBJECT

public:
    explicit CostEditingPrompt(const QString& userName, const QString& applyId, QWidget *parent = nullptr);
    ~CostEditingPrompt();

private:
    Ui::CostEditingPrompt *ui;

    //下面两条是修改费用请求必须的参数 要从父窗口继承过来
    QString applyId;
    QString userName;

    //初始化
    void initiate();
    //展示工作内容 以及修改前的费用
    void showCostInfo(QJsonObject& responseInfo);
    //关闭窗口后的事件 主要是让主窗口能刷新表格
    void closeEvent(QCloseEvent *event) override;

public slots:
    // 按钮点击事件 看英文名就知道做什么
    void on_confirm_clicked();
    void on_cancel_clicked();
};

#endif // COSTEDITINGPROMPT_H
