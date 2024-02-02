#include <QJsonDocument>

#include "format.h"

QString netWorkIpAddress = "106.54.213.38";

namespace Format {
    QJsonObject QStringToJson(QString jsonString)
    {

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data()); //jsonString.toLocal8bit这个方法有毒 不能用
        if(jsonDocument.isNull())
        {
            qDebug()<< "String NULL"<< jsonString.toLocal8Bit().data();
        }
        QJsonObject jsonObject = jsonDocument.object();
        return jsonObject;
    }


    QString JsonToQString(QJsonObject jsonObject)
    {
        QJsonDocument document;
        document.setObject(jsonObject);
        QByteArray simpbyte_array = document.toJson(QJsonDocument::Compact);
        QString simpjson_str(simpbyte_array);

        return simpjson_str;
    }

    QString getOrderStatus(QString auditState, QString checkState, QString isrespagree)
    {
//        if (auditState == "00" && checkState == "00" && isrespagree == "0")
//            return "未计费";
//        if (auditState == "01" && checkState == "00")
//            return "未复核";
//        if (auditState == "01" && checkState == "10")
//            return "复核不通过";
//        if (auditState == "01" && checkState == "01"){
//            if (isrespagree == "0")
//                return "船代未同意";
//            else if (isrespagree == "01")
//                return "船代同意";
//            else if (isrespagree == "10")
//                return "船代不同意";
//        }
        if (auditState == "00")
            return "BC Editing Required";
        if (checkState == "00")
            return "AC Approval Required";
        if (isrespagree == "0")
            return "SA Approval Required";
        else
            return "SA Approved";
    }

}
