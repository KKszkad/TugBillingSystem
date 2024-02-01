#ifndef NETWORKSETTING_H
#define NETWORKSETTING_H

#include <QWidget>

// 修改ip地址弹出的小窗口 比较简单

namespace Ui {
class NetWorkSetting;
}

class NetWorkSetting : public QWidget
{
    Q_OBJECT

public:
    explicit NetWorkSetting(QWidget *parent = nullptr);
    ~NetWorkSetting();

private:
    Ui::NetWorkSetting *ui;

public slots:
    void on_confirm_clicked();

};


#endif // NETWORKSETTING_H
