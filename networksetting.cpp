#include "networksetting.h"
#include "ui_networksetting.h"
#include "format.h"

NetWorkSetting::NetWorkSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWorkSetting)
{
    ui->setupUi(this);
    ui->netWorkStatus->setText("默认远程服务器地址为:106.54.213.38\n在下方填入新地址");
    ui->netWorkStatus->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->ipAddress->setText(netWorkIpAddress);
    ui->ipAddress->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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
