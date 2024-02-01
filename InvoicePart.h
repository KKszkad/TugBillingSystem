#ifndef INVOICEPART_H
#define INVOICEPART_H

#include <QString>
#include <QVector>

//支票可变的部分 包括工作内容（多个）    每个工作需要的拖轮数（多个）  总费用

class InvoicePart
{
public:
    //这些其实不需要
//    QString applyUnit;
//    QString operationDate;
//    QString applicant;
//    QString cashier;
//    QString auditor;
//    QString tugApplyId;
//    QString chineseName;
//    int shipLength;
//    int shipWidth;
//    int shipWeight;
//    QString shipType;
//    QString port;

    //以下为分开的内容
    QVector<QString> works;
    QVector<int> tugNumbers;
    int totalCost = 0;
    InvoicePart() {}

};

#endif // INVOICEPART_H
