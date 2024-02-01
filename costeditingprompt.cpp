#include "costeditingprompt.h"
#include "ui_costeditingprompt.h"
#include "httpclient.h"
#include "format.h"
#include "tcmainwindow.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>
#include <QMessageBox>
CostEditingPrompt::CostEditingPrompt(const QString& userName, const QString& applyId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CostEditingPrompt)
{
    setFixedSize(400, 300);
    setWindowFlags(Qt::Window);
    ui->setupUi(this);
    this->applyId = applyId;
    this->userName = userName;
    initiate();
}

CostEditingPrompt::~CostEditingPrompt()
{
    delete ui;
}

void CostEditingPrompt::initiate()
{
    ui->costTable->setColumnCount(5);
    QHeaderView *header = ui->costTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);


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

void CostEditingPrompt::showCostInfo(QJsonObject &responseInfo)
{
    QJsonObject data = responseInfo["data"].toObject();

    ui->costTable->setRowCount(data["total"].toInt());

    QJsonArray costDatas = data["records"].toArray();
    int row = -1;
    for (const QJsonValue &value : costDatas) {
        QJsonObject dataObject = value.toObject();

        row ++;
        QTableWidgetItem *operationName	 = new QTableWidgetItem(dataObject["operationname"].toString());
        QTableWidgetItem *mKey = new QTableWidgetItem(QString::number(dataObject["mkey"].toInt()));
        QTableWidgetItem *tugNumber	 = new QTableWidgetItem(QString::number(dataObject["tugnumber"].toInt()));
        QTableWidgetItem *defaultCost = new QTableWidgetItem(QString::number(dataObject["cost"].toInt()));
//        QLineEdit *actualCost = new QLineEdit();
        QTableWidgetItem *actualCost = new QTableWidgetItem();

        ui->costTable->setItem(row, 0, operationName);
        ui->costTable->setItem(row, 1, mKey);
        ui->costTable->setItem(row, 2, tugNumber);
        ui->costTable->setItem(row, 3, defaultCost);
//        ui->costTable->setCellWidget(row, 4, actualCost);
        ui->costTable->setItem(row, 4, actualCost);
    }
}

void CostEditingPrompt::closeEvent(QCloseEvent *event)
{
    // 执行关闭窗口时的操作，调用父窗口的成员函数
    if (parentWidget()) {
        static_cast<TCMainWindow*>(parentWidget())->cashierRequest();
    }
    QWidget::closeEvent(event);
}

void CostEditingPrompt::on_confirm_clicked()
{
    //装配要发送的数据
    QJsonObject sendInfo;
    sendInfo["applyid"] = applyId;


    //添加aa数组
    //WARNING 以下这种方式有问题 会造成程序崩溃 但是后来怎么又好了 真奇怪
    QJsonArray aaArray;
    for (int row = 0; row < ui->costTable->rowCount(); ++row) {
        // 获取对应行的数据
        QString mKey = ui->costTable->item(row, 1)->text();
        QString cost = ui->costTable->item(row, 4)->text();
        if (cost.isEmpty()){
            cost = ui->costTable->item(row, 3)->text();
        }
        QJsonObject obj;
        obj[mKey] = cost.toInt();
        aaArray.append(obj);
    }
    sendInfo["aa"] = aaArray;

    sendInfo["audit_clerk"] = userName;

    HttpClient("http://" + netWorkIpAddress + ":8095/audit_tug").success([=](const QString &response) {

                                                    QJsonObject responseInfo = Format::QStringToJson(response);
//                                                    showPorts(responseInfo);
                                                    if (responseInfo["code"].toInt() == 200) {
                                                        // 创建信息框
                                                        // 这种信息框可以阻塞主窗口 并且不会导致其关闭

                                                        QMessageBox::about(this, "提示", "修改申请成功");

                                                    } else {
                                                        // 创建信息框
                                                        QMessageBox::about(this, "提示", "修改申请失败");
                                                    }
                                                    //TODO 在此之前应当涉及一个数据的更新使得主窗口重新发送信息请求
                                                    //

                                                    //释放窗口资源
                                                    this->close();
                                                 }).json(Format::JsonToQString(sendInfo)).post();

}

void CostEditingPrompt::on_cancel_clicked()
{
    //释放窗口资源
    close();
}


