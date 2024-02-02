#include "orderconfirmingprompt.h"
#include "ui_orderconfirmingprompt.h"
#include "format.h"
#include "httpclient.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <SAMainWindow.h>

OrderConfirmingPrompt::OrderConfirmingPrompt(const QString &applyId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderConfirmingPrompt)
{
    setFixedSize(458, 390);
    //这个是为了让prompt窗口有轮廓
    setWindowFlags(Qt::Window);

    ui->setupUi(this);
    this->applyId = applyId;
    initiate();
}

OrderConfirmingPrompt::~OrderConfirmingPrompt()
{
    delete ui;
}

void OrderConfirmingPrompt::initiate()
{
    ui->costTable->setColumnCount(3);
    QHeaderView *header = ui->costTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    ui->costTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QJsonObject sendInfo;
    sendInfo["pageSize"] = 99;
    sendInfo["pageNum"] = 1;

    QJsonObject conditionObject;
    conditionObject["applyid"] = applyId;
    sendInfo["condition"] = conditionObject;

    HttpClient("http://" + netWorkIpAddress + ":8095/apply_content_list").success([=](const QString &response) {
                                                              QJsonObject responseInfo = Format::QStringToJson(response);
                                                              showCostInfo(responseInfo);
                                                          }).json(Format::JsonToQString(sendInfo)).post();
}

void OrderConfirmingPrompt::showCostInfo(QJsonObject &responseInfo)
{
    QJsonObject data = responseInfo["data"].toObject();


    ui->costTable->setRowCount(data["total"].toInt());

    QJsonArray costDatas = data["records"].toArray();
    int row = -1;
    for (const QJsonValue &value : costDatas) {
        QJsonObject dataObject = value.toObject();

        row ++;
        QTableWidgetItem *operationName	 = new QTableWidgetItem(dataObject["operationname"].toString());
        QTableWidgetItem *tugNumber	 = new QTableWidgetItem(QString::number(dataObject["tugnumber"].toInt()));
        QTableWidgetItem *cost = new QTableWidgetItem(QString::number(dataObject["cost"].toInt()));

        ui->costTable->setItem(row, 0, operationName);
        ui->costTable->setItem(row, 1, tugNumber);
        ui->costTable->setItem(row, 2, cost);
    }
}

void OrderConfirmingPrompt::closeEvent(QCloseEvent *event)
{
    // 调用父窗口的方法以刷新表格
    if (parentWidget()) {
        static_cast<SAMainWindow*>(parentWidget())->agencyRequest();
    }
    QWidget::closeEvent(event);
}

void OrderConfirmingPrompt::on_agree_clicked()
{
    QJsonObject sendInfo;
    sendInfo["tugapplyid"] = applyId;
    sendInfo["isagree"] = 1;
    sendInfo["resmsg"] = "没问题";

    HttpClient("http://" + netWorkIpAddress + ":8095/IsAgree").success([=](const QString &response) {
                                                              QJsonObject responseInfo = Format::QStringToJson(response);
                                                              if (responseInfo["code"].toInt() == 200) {
                                                                  // 创建信息框
                                                                  // 这种信息框可以阻塞主窗口 并且不会导致其关闭

                                                                  QMessageBox::about(this, "Tip", "Quotation approved!");

                                                              } else {
                                                                  // 创建信息框
                                                                  QMessageBox::about(this, "提示", "同意订单失败");
                                                              }
                                                              //TODO 在此之前应当涉及一个数据的更新使得主窗口重新发送信息请求
                                                              //
                                                              //释放窗口资源
                                                              close();
                                                          }).json(Format::JsonToQString(sendInfo)).post();
}

void OrderConfirmingPrompt::on_disagree_clicked()
{
    QJsonObject sendInfo;
    sendInfo["tugapplyid"] = applyId;
    sendInfo["isagree"] = 0;
    sendInfo["resmsg"] = "没问题";

    HttpClient("http://" + netWorkIpAddress + ":8095/IsAgree").success([=](const QString &response) {
                                                              QJsonObject responseInfo = Format::QStringToJson(response);
                                                              if (responseInfo["code"].toInt() == 200) {
                                                                  // 创建信息框
                                                                  // 这种信息框可以阻塞主窗口 并且不会导致其关闭
                                                                  QMessageBox::about(this, "Tip", "Deny the quotation");

                                                              } else {
                                                                  // 创建信息框
                                                                  QMessageBox::about(this, "提示", "退回订单失败");
                                                              }
                                                              //释放窗口资源
                                                              close();
                                                          }).json(Format::JsonToQString(sendInfo)).post();
}

void OrderConfirmingPrompt::on_cancel_clicked()
{
    close();
}
