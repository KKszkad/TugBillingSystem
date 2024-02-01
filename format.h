#ifndef FORMAT_H
#define FORMAT_H

//程序通用的工具类 以命名空间的方式使用

#include <QJsonObject>

#pragma once

// 服务器主机地址  全局变量
extern QString netWorkIpAddress;

namespace Format{
// string转为json的函数
QJsonObject QStringToJson(QString jsonString);
// json转为strign的函数
QString JsonToQString(QJsonObject jsonObject);
// 根据相关的三个角色的状态得出订单的状态
QString getOrderStatus(QString auditState, QString checkState, QString isrespagree);
}


#endif // FORMAT_H
