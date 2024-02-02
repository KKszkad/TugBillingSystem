#include "samainwindow.h"
#include "ui_samainwindow.h"
#include "httpclient.h"
#include "format.h"
#include "orderconfirmingprompt.h"
#include "login.h"

#include <QJsonObject>
#include <QJsonDocument>
#include<QUrlQuery>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QMessageBox>
#include <QRegularExpression>

SAMainWindow::SAMainWindow(const QString& userName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SAMainWindow)
{
    this->userName = userName;
    setFixedSize(991, 521);
    ui->setupUi(this);
    ui->confirmShip->setEnabled(false);

    ui->workInfo->setSelectionMode(QAbstractItemView::NoSelection);

    QHeaderView *worksHeaderView = ui->workInfo->horizontalHeader();
    worksHeaderView->setSectionResizeMode(QHeaderView::Stretch);

    timer = new QTimer(this);

    //0.75秒钟执行一次查询
    timer->start(750);

    initOrdersTable();

    ui->zhName->setPlaceholderText("Enter the Chinese name of the ship");

}

SAMainWindow::~SAMainWindow()
{
    delete ui;
}


//查询船只信息并显示在shipInfo中
void SAMainWindow::on_findShip_clicked() {
    QString zhName = ui->zhName->text();
    QJsonObject sendInfo;
    sendInfo["zhName"] = zhName;
    HttpClient("http://" + netWorkIpAddress + ":8095/get_shipagent").success([=](const QString &response) {
                                                 QJsonObject responseInfo = Format::QStringToJson(response);
                                                 showShipInfo(responseInfo);
                                             }).json(Format::JsonToQString(sendInfo)).post();
}

//确认选择此船后进入下一步选择工作
void SAMainWindow::on_confirmShip_clicked() {
    ui->procedure->setCurrentIndex(0);
    ui->shipInfo->clear();
    ui->zhName->clear();
    HttpClient("http://" + netWorkIpAddress + ":8095/get_ope").success([=](const QString &response) {
                                                   QJsonObject responseInfo = Format::QStringToJson(response);
                                                   showWorkInfo(responseInfo);
                                               }).post();
}



void SAMainWindow::on_confirmWorks_clicked() {

    // 遍历 QTableWidget 中的每一行
    for (int row = 0; row < ui->workInfo->rowCount(); ++row) {
        // 获取相应行的数据
        QString mKey = ui->workInfo->item(row, 4)->text();
//        QString tugBoatCount = ui->workInfo->item(row, 5)->text();
        QString tugBoatCount = dynamic_cast<QLineEdit*>(ui->workInfo->cellWidget(row, 5))->text();
        bool confirmWorkChecked = ui->workInfo->item(row, 6)->checkState() == Qt::Checked;

        // 检查条件并输出
        if (!tugBoatCount.isEmpty() && confirmWorkChecked) {
            // 使用正则表达式检查是否为正整数
            QRegularExpression regExp("^[1-9]\\d*$");  // 匹配以1-9开头的一个或多个数字

            if (regExp.match(tugBoatCount).hasMatch()) {
                WorkDetail workDetail = WorkDetail(mKey.toInt(), tugBoatCount.toInt());
                application.workDetails.append(workDetail);
            } else {
                QMessageBox::about(this, "Warning", "the tugboatCount must be int!");
                return;
            }

        }
    }

    if (application.workDetails.isEmpty()){
        QMessageBox::about(this, "Warning", "Should choose at least one work!");
        return;
    }
    ui->procedure->setCurrentIndex(1);
    showPorts();

}

void SAMainWindow::on_submitApplication_clicked() {
    application.areaName = ui->selectPort->currentText();
    application.busState = ui->business->currentIndex();
    application.stTime = ui->startTime->dateTime();
    application.edTime = ui->endTime->dateTime();
    application.shipTel = ui->shipTel->text();

    QJsonObject sendInfo;
    int workCount = application.workDetails.size();

    QJsonArray operation_mkey;
    QJsonArray operation_num;
    for (int i = 0; i < workCount; i++) {
        operation_mkey.append(application.workDetails[i].mKey);
        operation_num.append(application.workDetails[i].tugBoatCount);
    }

    sendInfo["zhname"] = application.zhName;
    sendInfo["sttime"] = application.stTime.toString("yyyy-MM-dd hh:mm:ss");
    sendInfo["edtime"] = application.edTime.toString("yyyy-MM-dd hh:mm:ss");
    sendInfo["operation_mkey"] = operation_mkey;
    sendInfo["operation_num"] = operation_num;
    sendInfo["areaname"] = application.areaName;
    sendInfo["busstate"] = application.busState;
    sendInfo["applicant"] = userName;
    sendInfo["shiptel"] = application.shipTel;


    HttpClient("http://" + netWorkIpAddress + ":8095/apply_tug").success([=](const QString &response) {
                                                     QJsonObject responseInfo = Format::QStringToJson(response);
                                                     if (responseInfo["code"].toInt() == 200) {
                                                         // 创建信息框
                                                         // 这种信息框可以阻塞主窗口 并且不会导致其关闭
//                                                         messageBox.setMessage("Successfully submit the application!");
                                                         QMessageBox::about(this, "Tip", "Successfully submit the application!");

                                                     } else {
                                                         // 创建信息框
                                                         QMessageBox::about(this, "提示", "拖轮申请失败");
                                                     }
                                                     ui->procedure->setCurrentIndex(2);
                                                     // WARNNING 新改动 刷新数据
                                                     agencyRequest();
                                                                }).json(Format::JsonToQString(sendInfo)).post();
}

void SAMainWindow::on_lastSection0_clicked()
{
    ui->procedure->setCurrentIndex(2);
}

void SAMainWindow::on_lastSection1_clicked()
{
    ui->procedure->setCurrentIndex(0);
}

//将传回的船舶信息显示在shipInfo中 如没有此船则显示查无此船
void SAMainWindow::showShipInfo(QJsonObject &responseObject) {
    int code = responseObject["code"].toInt();
    if (code == 400) {
        ui->shipInfo->setText("No such ship.");
        ui->confirmShip->setEnabled(false);
        return;
    }
    ui->confirmShip->setEnabled(true);
    QJsonValue shipInfo = responseObject["data"];

    QStringList shipInfoStrings;
    shipInfoStrings.append(QString("中文名：%1").arg(shipInfo["zhname"].toString()));
    shipInfoStrings.append(QString("英文名：%1").arg(shipInfo["enname"].toString()));
    shipInfoStrings.append(QString("mmsi号：%1").arg(shipInfo["mmsi"].toString()));
    shipInfoStrings.append(QString("国籍：%1").arg(shipInfo["nationality"].toString()));
    shipInfoStrings.append(QString("船舶类型：%1").arg(shipInfo["shiptype"].toString()));
    shipInfoStrings.append(QString("船长：%1").arg(shipInfo["shipLength"].toInt()));
    shipInfoStrings.append(QString("船宽：%1").arg(shipInfo["shipwidth"].toInt()));
    shipInfoStrings.append(QString("总吨数：%1").arg(shipInfo["totaltonnum"].toInt()));
    shipInfoStrings.append(QString("净吨数：%1").arg(shipInfo["nettonnum"].toInt()));
    shipInfoStrings.append(QString("代理人：%1").arg(shipInfo["agentname"].toString()));

    ui->shipInfo->clear();
    ui->shipInfo->append(shipInfoStrings.join("\n"));

    //确定选中此船
    application.zhName = shipInfo["zhname"].toString();
}

void SAMainWindow::showWorkInfo(QJsonObject &workInfo){

    QJsonArray operationDatas = workInfo["data"].toArray();
    ui->workInfo->setRowCount(operationDatas.size());
    int row = -1;
    for (const QJsonValue &value : operationDatas) {
        QJsonObject dataObject = value.toObject();

        row ++;
        QTableWidgetItem *operationName = new QTableWidgetItem(dataObject["operationname"].toString());
        QTableWidgetItem *operationType = new QTableWidgetItem(dataObject["operationtype"].toString());
        QTableWidgetItem *chargeStandard = new QTableWidgetItem(dataObject["chargestandard"].toString());
        QTableWidgetItem *abbreviation = new QTableWidgetItem(dataObject["abbr"].toString());
        QTableWidgetItem *mKey = new QTableWidgetItem(QString::number(dataObject["mkey"].toInt()));
        QLineEdit *tugBoatCount = new QLineEdit();
        QTableWidgetItem *confirmWork = new QTableWidgetItem();

        operationName->setFlags(operationName->flags() & ~Qt::ItemIsEditable);
        operationType->setFlags(operationType->flags() & ~Qt::ItemIsEditable);
        chargeStandard->setFlags(chargeStandard->flags() & ~Qt::ItemIsEditable);
        abbreviation->setFlags(abbreviation->flags() & ~Qt::ItemIsEditable);
        mKey->setFlags(mKey->flags() & ~Qt::ItemIsEditable);
        confirmWork->setFlags(confirmWork->flags() & ~Qt::ItemIsEditable);

        confirmWork->setCheckState(Qt::Unchecked);
        ui->workInfo->setItem(row, 0, operationName);
        ui->workInfo->setItem(row, 1, operationType);
        ui->workInfo->setItem(row, 2, chargeStandard);
        ui->workInfo->setItem(row, 3, abbreviation);
        ui->workInfo->setItem(row, 4, mKey);
        ui->workInfo->setCellWidget(row, 5, tugBoatCount);
        ui->workInfo->setItem(row, 6, confirmWork);
    }
}

void SAMainWindow::showPorts()
{
    ui->selectPort->addItem("霞山港区");
    ui->selectPort->addItem("宝满港区");
    ui->selectPort->addItem("调顺港区");
    ui->selectPort->addItem("电厂码头");
    ui->selectPort->addItem("4804厂码头");
    ui->selectPort->addItem("部队码头");
    ui->selectPort->addItem("宝钢码头");
    ui->selectPort->addItem("中科炼化码头");
    ui->selectPort->addItem("南油码头");
    ui->selectPort->addItem("武理码头");
    ui->selectPort->addItem("宝盛码头");
    ui->selectPort->addItem("海滨船厂");
    ui->selectPort->addItem("湛江航道");
}

//void SAMainWindow::showPorts(QJsonObject &ports) {
//    QJsonArray portDatas = ports["data"].toArray();
//    for (const QJsonValue &portValue : portDatas) {
//        QJsonObject portObj = portValue.toObject();
//        ui->selectPort->addItem(portObj["areaname"].toString());
//    }
//}

void SAMainWindow::initOrdersTable()
{
    QHeaderView *ordersHeaderView = ui->ordersTable->horizontalHeader();
    ordersHeaderView->setSectionResizeMode(QHeaderView::Stretch);


    //设置行选择和单选
    ui->ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);

    //设置表格尺寸
    ui->ordersTable->setRowCount(pageSize);
    ui->ordersTable->setColumnCount(6);

    ui->lastPage->setEnabled(false);

    //设置表格不能编辑
    ui->ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    agencyRequest();


    connect(timer, &QTimer::timeout, [=]{
        agencyRequest();
    });

    //这里设置双击事件
    connect(ui->ordersTable, &QTableWidget::itemDoubleClicked, [&](QTableWidgetItem *item){
        if(item) {
            int row = item->row();
            QString status = ui->ordersTable->item(row, 5)->text();
            if (status == "SA Approval Required"){
                //检查整行的所有单元格 只要有内容
                bool rowHasContent = false;
                for (int col = 0; col < ui->ordersTable->columnCount(); ++col) {
                    if (ui->ordersTable->item(row, col) && !ui->ordersTable->item(row, col)->text().isEmpty()){
                        rowHasContent = true;
                        break;
                    }
                }
                if (rowHasContent) {
                    OrderConfirmingPrompt *prompt = new OrderConfirmingPrompt(ui->ordersTable->item(row, 0)->text(), this);
                    prompt->show();
                }
            }
        }
    });
}

void SAMainWindow::agencyRequest()
{
    QJsonObject sendInfo;
    sendInfo["pageSize"] = pageSize;
    sendInfo["pageNum"] = currentPage;

    QJsonObject conditionObject;
    conditionObject["applicant"] = userName;

    sendInfo["condition"] = conditionObject;

    HttpClient("http://" + netWorkIpAddress + ":8095/applicant_apply_list").success([=](const QString &response) {
                                                         QJsonObject responseInfo = Format::QStringToJson(response);
                                                         showOrdersTable(responseInfo);
                                                         ui->totalPage->setText(QString("Total:%1").arg(totalPage));
                                                         ui->currentPage->setText(QString("Current Page:%1").arg(currentPage));
                                                     }).json(Format::JsonToQString(sendInfo)).post();
}

void SAMainWindow::showOrdersTable(QJsonObject &orderObj)
{
    QJsonObject data = orderObj["data"].toObject();
    totalPage = data["pages"].toInt();

    //这里控制一开始是否能翻页 虽然和函数的主题不相关
    if (totalPage <= 1 || currentPage == totalPage)
        ui->nextPage->setEnabled(false);
    else
        ui->nextPage->setEnabled(true);


    QJsonArray checkDatas = data["records"].toArray();
    int row = -1;
    //TODO 这里不需要过滤 但需要设置一下权限 当订单还在船代公司那边审核时船代将无法点击订单
    //弹出prompt窗口后 而同意和不同意选项则不能选择

    for (const QJsonValue &value : checkDatas) {
        QJsonObject dataObject = value.toObject();

        row ++;
        QTableWidgetItem *applyId = new QTableWidgetItem(dataObject["tugapplyid"].toString());
        QTableWidgetItem *chineseName  = new QTableWidgetItem(dataObject["chinesename"].toString());
        QTableWidgetItem *applyDate  = new QTableWidgetItem(dataObject["applydate"].toString());
        QTableWidgetItem *auditor  = new QTableWidgetItem(dataObject["auditclerk"].toString());
        QTableWidgetItem *checker  = new QTableWidgetItem(dataObject["checker"].toString());

        QString status_str = Format::getOrderStatus(dataObject["auditstate"].toString(), dataObject["checkstate"].toString(), dataObject["isrespagree"].toString());
        QTableWidgetItem *status = new QTableWidgetItem(status_str);


        //TODO 在这里添加背景色
        if (status_str == "SA Approval Required") {
            // 设置背景颜色为红色
            status->setBackground(Qt::red);
        }

        ui->ordersTable->setItem(row, 0, applyId);
        ui->ordersTable->setItem(row, 1, chineseName);
        ui->ordersTable->setItem(row, 2, applyDate);
        ui->ordersTable->setItem(row, 3, auditor);
        ui->ordersTable->setItem(row, 4, checker);
        ui->ordersTable->setItem(row, 5, status);
    }
}

void SAMainWindow::on_lastPage_clicked()
{
    currentPage --;
    ui->nextPage->setEnabled(true);
    ui->ordersTable->clearContents();
    agencyRequest();

    if (currentPage == 1)
        ui->lastPage->setEnabled(false);
}

void SAMainWindow::on_nextPage_clicked()
{
    currentPage ++;
    ui->lastPage->setEnabled(true);
    ui->ordersTable->clearContents();
    agencyRequest();

    if (currentPage == totalPage)
        ui->nextPage->setEnabled(false);
}

void SAMainWindow::on_logOut_clicked()
{
    Login* login = new Login();
    login->show();
    close();
}


