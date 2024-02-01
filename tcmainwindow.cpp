#include "tcmainwindow.h"
#include "ui_tcmainwindow.h"
#include "httpclient.h"
#include "format.h"
#include "costeditingprompt.h"
#include "ordercheckingprompt.h"
#include "login.h"

#include <QJsonObject>
#include <QJsonArray>


TCMainWindow::TCMainWindow(const QString& userName, const int &position, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TCMainWindow)
{

    this->userName = userName;
    ui->setupUi(this);

    setFixedSize(982, 473);

    timer = new QTimer();
    //0.75秒钟执行一次查询
    timer->start(750);

    //就是这个决定这个窗口展示哪一面 计费员还是复核员
    ui->userType->setCurrentIndex(position);
    if(position == 0){
        initCashierTable();
        connect(timer, &QTimer::timeout, [=](){
            cashierRequest();
        });
    }
    else{
        initAuditorTable();
        connect(timer, &QTimer::timeout, [=](){
            auditorRequest();
        });
    }


}

TCMainWindow::~TCMainWindow()
{
    foreach(QWidget *widget, QApplication::topLevelWidgets())
    {
        if (widget != this)
        {
            delete widget;
        }
    }
    delete ui;
}

void TCMainWindow::initCashierTable()
{
    //初始化欢迎语
    ui->welcomeSign0->setText("欢迎 计费员" + userName);
    ui->cashierTable->resizeColumnsToContents();

    //初始化表格

    QHeaderView *header = ui->cashierTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    //设置行选择和单选
    ui->cashierTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cashierTable->setSelectionMode(QAbstractItemView::SingleSelection);
    //设置表格尺寸
    ui->cashierTable->setRowCount(pageSize);
    ui->cashierTable->setColumnCount(6);
    cashierRequest();
    //上一页的按键是绝对按不了的
    ui->lastPage0->setEnabled(false);
    //设置表格不能编辑
    ui->cashierTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //这里设置双击事件
    //即弹出窗口使计费员可以更改费用
    connect(ui->cashierTable, &QTableWidget::itemDoubleClicked, [&](QTableWidgetItem *item){
        if(item) {
            int row = item->row();
            QString status = ui->cashierTable->item(row, 5)->text();
            if (status == "需要计费"){
                //检查整行的所有单元格 只要有内容
                bool rowHasContent = false;
                for (int col = 0; col < ui->cashierTable->columnCount(); ++col) {
                    if (ui->cashierTable->item(row, col) && !ui->cashierTable->item(row, col)->text().isEmpty()){
                        rowHasContent = true;
                        break;
                    }
                }

                if (rowHasContent) {
                    CostEditingPrompt *prompt = new CostEditingPrompt(userName, ui->cashierTable->item(row, 2)->text(), this);
                    prompt->show();
                }
}
        }
    });
}

inline void TCMainWindow::cashierRequest()
{
    QJsonObject sendInfo;
    sendInfo["pageSize"] = pageSize;
    sendInfo["pageNum"] = currentPage;
    HttpClient("http://" + netWorkIpAddress + ":8095/apply_list").success([=](const QString &response) {
                                                         QJsonObject responseInfo = Format::QStringToJson(response);
                                                         showBillsInfo(responseInfo);
                                                         ui->totalPage0->setText(QString("总页数：%1").arg(totalPage));
                                                         ui->currentPage0->setText(QString("当前页数：%1").arg(currentPage));
                                                     }).json(Format::JsonToQString(sendInfo)).post();
}

inline void TCMainWindow::showBillsInfo(QJsonObject &billObj)
{
    QJsonObject data = billObj["data"].toObject();
    totalPage = data["pages"].toInt();

    //这里控制一开始是否能翻页 虽然和函数的主题不相关
    if (totalPage <= 1 || currentPage == totalPage)
        ui->nextPage0->setEnabled(false);
    else
        ui->nextPage0->setEnabled(true);



    QJsonArray orderDatas = data["records"].toArray();
    int row = -1;

    for (const QJsonValue &value : orderDatas) {
        QJsonObject dataObject = value.toObject();
        row ++;
        QTableWidgetItem *chineseName = new QTableWidgetItem(dataObject["chinesename"].toString());
        QTableWidgetItem *englishName = new QTableWidgetItem(dataObject["englishname"].toString());
        QTableWidgetItem *applyId = new QTableWidgetItem(dataObject["tugapplyid"].toString());
        QTableWidgetItem *applyDate	 = new QTableWidgetItem(dataObject["applydate"].toString());
        QTableWidgetItem *saCompany = new QTableWidgetItem(dataObject["sacompany"].toString());

        QString status_str = Format::getOrderStatus(dataObject["auditstate"].toString(), dataObject["checkstate"].toString(), dataObject["isrespagree"].toString());
        QTableWidgetItem *status = new QTableWidgetItem(status_str);

        if (status_str == "需要计费") {
            status->setBackground(Qt::red);
        }

        ui->cashierTable->setItem(row, 0, chineseName);
        ui->cashierTable->setItem(row, 1, englishName);
        ui->cashierTable->setItem(row, 2, applyId);
        ui->cashierTable->setItem(row, 3, applyDate);
        ui->cashierTable->setItem(row, 4, saCompany);
        ui->cashierTable->setItem(row, 5, status);
    }

}

void TCMainWindow::initAuditorTable()
{  
    ui->welcomeSign1->setText("欢迎 复核员" + userName);


    QHeaderView *header = ui->auditorTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    //设置行选择和单选
    ui->auditorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->auditorTable->setSelectionMode(QAbstractItemView::SingleSelection);

    //设置表格尺寸
    ui->auditorTable->setRowCount(pageSize);
    ui->auditorTable->setColumnCount(7);
    auditorRequest();

    //上一页的按键是绝对按不了的
    ui->lastPage1->setEnabled(false);

    //设置表格不能编辑
    ui->auditorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //调整列宽度适应内容
    ui->auditorTable->resizeColumnsToContents();

    //这里设置双击事件
    //即弹出窗口使计费员可以更改费用
    connect(ui->auditorTable, &QTableWidget::itemDoubleClicked, [&](QTableWidgetItem *item){
        if(item) {

            int row = item->row();
            QString status = ui->auditorTable->item(row, 6)->text();
            if (status == "需要复核" || status == "船代已同意"){
                //TODO 这里是否说明复核员的prompt窗口在初始化时根据这个status也作出反应

                //检查整行的所有单元格 只要有内容
                bool rowHasContent = false;
                for (int col = 0; col < ui->auditorTable->columnCount(); ++col) {
                    if (ui->auditorTable->item(row, col) && !ui->auditorTable->item(row, col)->text().isEmpty()){
                        rowHasContent = true;
                        break;
                    }
                }

                if (rowHasContent) {
                    //WARNING 这里有过问题 程序找不到被销毁的子窗口指针而崩溃
                    // 问题在于在login中初始化tcmainwindow时将login指定为了tcmainwindow的父窗口 可能和qt的继承树关系有关 但是不明白的一点是 将tcmainwindow的父窗口设为空指针后就没有问题 但是这个prompt窗口和tcmainwindow的创建又有什么关系？
                    OrderCheckingPrompt *prompt = new OrderCheckingPrompt(userName, ui->auditorTable->item(row, 2)->text(), ui->auditorTable->item(row, 6)->text(), this);
                    prompt->show();
                }
            }
        }
    });
}

void TCMainWindow::auditorRequest()
{
    QJsonObject sendInfo;
    sendInfo["pageSize"] = pageSize;
    sendInfo["pageNum"] = currentPage;
    HttpClient("http://" + netWorkIpAddress + ":8095/apply_list").success([=](const QString &response) {
                                                         QJsonObject responseInfo = Format::QStringToJson(response);
                                                         showChecksInfo(responseInfo);
                                                         ui->totalPage1->setText(QString("总页数：%1").arg(totalPage));
                                                         ui->currentPage1->setText(QString("当前页数：%1").arg(currentPage));
                                                     }).json(Format::JsonToQString(sendInfo)).post();
}

void TCMainWindow::showChecksInfo(QJsonObject &checkObj)
{
    QJsonObject data = checkObj["data"].toObject();
    totalPage = data["pages"].toInt();

    //这里控制一开始是否能翻页 虽然和函数的主题不相关
    //但是这里有问题，在于每次向下翻页时，click事件触发的按钮可用性判断会被这里的粗略判断给覆盖。把if做得更细致一些就好了。
//    if(totalPage <= 1)
//        ui->nextPage1->setEnabled(false);
//    else
//        ui->nextPage1->setEnabled(true);

    if (totalPage <= 1 || currentPage == totalPage)
        ui->nextPage1->setEnabled(false);
    else
        ui->nextPage1->setEnabled(true);



    QJsonArray checkDatas = data["records"].toArray();
    int row = -1;
    //
    for (const QJsonValue &value : checkDatas) {
        QJsonObject dataObject = value.toObject();
        row ++;
        QTableWidgetItem *chineseName = new QTableWidgetItem(dataObject["chinesename"].toString());
        QTableWidgetItem *englishName = new QTableWidgetItem(dataObject["englishname"].toString());
        QTableWidgetItem *applyId = new QTableWidgetItem(dataObject["tugapplyid"].toString());
        QTableWidgetItem *applicant = new QTableWidgetItem(dataObject["applicant"].toString());
        QTableWidgetItem *saCompany = new QTableWidgetItem(dataObject["sacompany"].toString());
        QTableWidgetItem *auditClerk = new QTableWidgetItem(dataObject["auditclerk"].toString());

        QString status_str = Format::getOrderStatus(dataObject["auditstate"].toString(), dataObject["checkstate"].toString(), dataObject["isrespagree"].toString());
        QTableWidgetItem *status = new QTableWidgetItem(status_str);

        //TODO 在这里添加背景色
        if (status_str == "船代已同意") {
            // 设置背景颜色为黄色
            status->setBackground(Qt::yellow);
        } else if (status_str == "需要复核") {
            // 设置背景颜色为红色
            status->setBackground(Qt::red);
        }
        ui->auditorTable->setItem(row, 0, chineseName);
        ui->auditorTable->setItem(row, 1, englishName);
        ui->auditorTable->setItem(row, 2, applyId);
        ui->auditorTable->setItem(row, 3, applicant);
        ui->auditorTable->setItem(row, 4, saCompany);
        ui->auditorTable->setItem(row, 5, auditClerk);
        ui->auditorTable->setItem(row, 6, status);
    }
}

void TCMainWindow::on_lastPage0_clicked()
{
    currentPage --;
    ui->nextPage0->setEnabled(true);

    ui->cashierTable->clearContents();


    cashierRequest();

    if (currentPage == 1)
        ui->lastPage0->setEnabled(false);
}

void TCMainWindow::on_nextPage0_clicked()
{
    currentPage ++;
    ui->lastPage0->setEnabled(true);

    ui->cashierTable->clearContents();
    cashierRequest();

    if (currentPage == totalPage)
        ui->nextPage0->setEnabled(false);
}

void TCMainWindow::on_lastPage1_clicked()
{
    currentPage --;
    ui->nextPage1->setEnabled(true);

    ui->auditorTable->clearContents();
    auditorRequest();

    if (currentPage == 1)
        ui->lastPage1->setEnabled(false);
}

void TCMainWindow::on_nextPage1_clicked()
{
    currentPage ++;
    ui->lastPage1->setEnabled(true);

    ui->auditorTable->clearContents();
    auditorRequest();

    if (currentPage == totalPage)
        ui->nextPage1->setEnabled(false);

}

void TCMainWindow::on_logOut0_clicked()
{
    Login* login = new Login();
    login->show();
    close();
}

void TCMainWindow::on_logOut1_clicked()
{
    Login* login = new Login();
    login->show();
    close();
}
