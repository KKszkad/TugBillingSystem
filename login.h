#ifndef LOGIN_H
#define LOGIN_H
#pragma once
#include <QWidget>
#include <QAbstractButton>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

signals:
    void loginSuccess();
    void loginFail();



public:
    Login(QWidget *parent = nullptr);
    ~Login();

private:
    Ui::Login *ui;

    // 登录请求需要的信息
    QString userName;
    QString passWord;
    int userType;

    // 登录检查
    void loginCheck(QJsonObject &responseObj);

private slots:
    // 选择用户类型 就是那个小圆点的勾选框
    void onRadioButtonClicked(QAbstractButton *button);
    // 按下登录键
    void on_login_clicked();
    // *新加的 改服务器ip用
    void on_netWorkSetting_clicked();
};
#endif // LOGIN_H
