#include "networksetting.h"
#include "ui_networksetting.h"
#include "format.h"

NetWorkSetting::NetWorkSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWorkSetting)
{
    ui->setupUi(this);
    QString status = "当前的服务器地址为：" + netWorkIpAddress;
    ui->netWorkStatus->setText(status);
    ui->netWorkStatus->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

NetWorkSetting::~NetWorkSetting()
{
    delete ui;
}

void NetWorkSetting::on_confirm_clicked()
{
    netWorkIpAddress = ui->ipAddress->text();
    qDebug() << netWorkIpAddress;
    close();
}
