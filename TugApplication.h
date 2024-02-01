#ifndef TUGAPPLICATION_H
#define TUGAPPLICATION_H

#include <QString>
#include <QDateTime>
#include <QVector>

// 船代申请需要的数据类型

class WorkDetail {
public:
    int mKey;
    int tugBoatCount;
    WorkDetail(int mKey, int tugBoatCount) {
        this->mKey = mKey;
        this->tugBoatCount = tugBoatCount;
    }

    QString toQString() {
        return "mKey:" + QString::number(mKey) + "\n" + "tugBoatCount:" + QString::number(tugBoatCount);
    }

    bool operator == (const WorkDetail & another) const {
        return mKey == another.mKey && tugBoatCount == another.tugBoatCount;
    }


};

class TugApplication{
public:
    QString zhName;
    QDateTime stTime;
    QDateTime edTime;
//    QVector<int> operation_mkey;
    QString areaName;
    int busState;
//    QVector<int> operation_num;
    QVector<WorkDetail> workDetails;
    QString applicant;
    QString shipTel;
};



#endif // TUGAPPLICATION_H
