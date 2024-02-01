#include "login.h"
#include "ui_login.h"
#include "samainwindow.h"
#include "tcmainwindow.h"
#include "httpclient.h"
#include "format.h"
#include "networksetting.h"

#include <QButtonGroup>
#include <QJsonObject>
#include <QJsonDocument>
#include<QUrlQuery>
#include <QMessageBox>
Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{

    setFixedSize(422, 192);
    ui->setupUi(this);
    //使用QButtonGroup来控制两个radiobutton只能选其一
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->userType0);
    buttonGroup->addButton(ui->userType1);
    // 连接槽函数，确保在同一时间只有一个按钮被选中
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onRadioButtonClicked(QAbstractButton*)));

}

Login::~Login()
{
    delete ui;
}

void Login::on_login_clicked() {
    userName = ui->userName_line->text();
    passWord = ui->passWord_line->text();

    QJsonObject sendInfo;
    sendInfo["username"] = userName;
    sendInfo["password"] = passWord;
    sendInfo["usertype"] = userType;

    HttpClient("http://" + netWorkIpAddress + ":8095/login").success([=](const QString &response) {
                                                    QJsonObject responseInfo = Format::QStringToJson(response);
                                                    loginCheck(responseInfo);
                                             }).json(Format::JsonToQString(sendInfo)).post();
}

void Login::on_netWorkSetting_clicked()
{
    NetWorkSetting* prompt = new NetWorkSetting();
    prompt->show();
}

void Login::onRadioButtonClicked(QAbstractButton *button) {
    if (button->text() == "拖轮公司") {
        // 在这里可以执行与 RadioButton 1 相关的操作，给出值 0
        userType = 1;
    } else if (button->text() == "船代") {
        // 在这里可以执行与 RadioButton 2 相关的操作，给出值 1
        userType = 0;
    }
}

void Login::loginCheck(QJsonObject &responseObj){

    int code = responseObj["code"].toInt();

    if (code == 400) {
        QMessageBox::about(this, "提示", "账号或密码错误！");
        return;
    }
    QJsonValue data = responseObj["data"];
    QJsonObject dataObj = data.toObject();
    QString userName = dataObj["username"].toString();
    QString userType = dataObj["usertype"].toString();
    this->userName = userName;
    close();
    if(userType == "0"){
    SAMainWindow *sa = new SAMainWindow(userName);
    sa->show();
    } else {
    int position = data["position"].toInt();
    TCMainWindow *tc = new TCMainWindow(userName, position);
    tc->show();
    }

}

