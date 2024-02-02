#include "ordercheckingprompt.h"
#include "ui_ordercheckingprompt.h"
#include "xlsxdocument.h"

//OrderCheckingPrompt::OrderCheckingPrompt(const QString& userName, const QString& applyId, QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::OrderCheckingPrompt)
//{
//    ui->setupUi(this);
//    this->applyId = applyId;
//    this->userName = userName;

//}

//OrderCheckingPrompt::~OrderCheckingPrompt()
//{
//    delete ui;
//}

#include "httpclient.h"
#include "format.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>

OrderCheckingPrompt::OrderCheckingPrompt(const QString& userName, const QString& applyId, const QString& status, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderCheckingPrompt)
{
    setWindowFlags(Qt::Window);
    setFixedSize(450, 327);
    ui->setupUi(this);
    this->applyId = applyId;
    this->userName = userName;
    this->status = status;
    initiate();
}

OrderCheckingPrompt::~OrderCheckingPrompt()
{
    delete ui;
}

void OrderCheckingPrompt::initiate()
{
    ui->costTable->setColumnCount(4);
    QHeaderView *header = ui->costTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    ui->costTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QJsonObject sendInfo;
    sendInfo["pageSize"] = 99;
    sendInfo["pageNum"] = 1;

    QJsonObject conditionObject;
    conditionObject["applyid"] = applyId;
    sendInfo["condition"] = conditionObject;

    if(status == "AC Approval Required") {
        ui->getCheck->setEnabled(false);
    } else {
        ui->confirm->setEnabled(false);
        ui->deny->setEnabled(false);
    }

    HttpClient("http://" + netWorkIpAddress + ":8095/apply_content_list").success([=](const QString &response) {
                                                              QJsonObject responseInfo = Format::QStringToJson(response);
                                                              showCostInfo(responseInfo);
                                                          }).json(Format::JsonToQString(sendInfo)).post();
}

void OrderCheckingPrompt::showCostInfo(QJsonObject &responseInfo)
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
        QTableWidgetItem *cost = new QTableWidgetItem(QString::number(dataObject["cost"].toInt()));

        //NOTE 直接在这里计算发票的不定部分好了
        invoice.works.append(dataObject["operationname"].toString());
        invoice.tugNumbers.append(dataObject["tugnumber"].toInt());
        invoice.totalCost += dataObject["cost"].toInt();



        ui->costTable->setItem(row, 0, operationName);
        ui->costTable->setItem(row, 1, mKey);
        ui->costTable->setItem(row, 2, tugNumber);
        ui->costTable->setItem(row, 3, cost);
    }
}

void OrderCheckingPrompt::produceExcelFile(QJsonObject &responseInfo)
{
    //WARNING 最新的进度在这里
    //需要在responseInfo中提取公有信息
    QJsonArray records = responseInfo["data"].toArray();
    QJsonObject sampleData = records[0].toObject();


    //然后在默认路径打开excel文件
    //将数据导入其中
    //再使用QFileDialog将其保存到指定路径
    //这里使用了资源文件sources.qrc管理其他文件 记得加上前缀:/excel
    QXlsx::Document excel(":/excel/InvoiceTemplate.xlsx");
//    QXlsx::Document excel(QDir::currentPath() + "InvoiceTemplate.xlsx");
    // WARNING 如果使用上面的语句的话 当前路径 "E:/Programs/Qt/build-TugBillingSystem-Desktop_Qt_6_6_0_MSVC2019_64bit-DebugInvoiceTemplate.xlsx" 并不是我所想的那样的项目路径

    if (excel.load()) {
        qDebug() << "file is loaded";
        excel.write(2, 2, sampleData["agentname"].toString());
        excel.write(2, 5, sampleData["applydate"].toString());

        //        申请单位:	2,2

        //            申请日期:	2,5

        //            执行时间： 4,3
        excel.write(4, 3, sampleData["operationtime"].toString());
        //            申请人：	5,2
        excel.write(5,2, sampleData["applicant"].toString());
        //            计费员：	5,5
        excel.write(5, 5, sampleData["auditclerk"].toString());
        //            复核员：	5,7
        excel.write(5, 7, sampleData["checker"].toString());
        //            拖轮申请ID：	7,3
        excel.write(7, 3, sampleData["tugapplyid"].toString());
        //            船名：	8,2
        excel.write(8, 2, sampleData["chinesename"].toString());
        //            长度：	9,4
        excel.write(9, 4, sampleData["shiplength"].toInt());
        //            宽度：	9,5
        excel.write(9, 5, sampleData["shipwidth"].toInt());
        //            船重：	9,6
        excel.write(9, 6, sampleData["totaltonnum"].toInt());
        //            类型：	9,7
        excel.write(9, 7, sampleData["shiptype"].toString());
        //            作业地点：	11,2
        excel.write(11, 2, sampleData["operationloc"].toString());

        //            作业内容：	11,4
        excel.write(11, 4, invoice.works.join(", "));

        // 构建以逗号分隔的字符串
        QString result;
        for (int value : invoice.tugNumbers) {
            if (!result.isEmpty()) {
                result += ", ";
            }
            result += QString::number(value);
        }

        //            对应拖轮数：	11,7
        excel.write(11, 7, result);
        //            合计价格：	13,3
        excel.write(13, 3, invoice.totalCost);
        QString templatePath = QFileDialog::getSaveFileName(nullptr, "Select Excel Template", "", "Excel Files (*.xlsx *.xls)");
        excel.saveAs(templatePath);
    } else {
        qDebug() << "加载excel文件失败";
    }

    //刷新支票部分的数据
    invoice.works.clear();
    invoice.tugNumbers.clear();
    invoice.totalCost=0;
    close();
}

void OrderCheckingPrompt::closeEvent(QCloseEvent *event) {
    // 执行关闭窗口时的操作，调用父窗口的成员函数
    if (parentWidget()) {
        static_cast<TCMainWindow*>(parentWidget())->auditorRequest();
    }
    QWidget::closeEvent(event);
}

void OrderCheckingPrompt::on_confirm_clicked()
{


    //装配要发送的数据
    QJsonObject sendInfo;
    sendInfo["applyid"] = applyId;
    sendInfo["conf"] = 1;
    sendInfo["checker"] = userName;
    sendInfo["confirmRemark"] = "没问题";

    HttpClient("http://" + netWorkIpAddress + ":8095/comfirm_tug").success([=](const QString &response) {
                                             QJsonObject responseInfo = Format::QStringToJson(response);
                                             if (responseInfo["code"].toInt() == 200) {
                                                 // 创建信息框
                                                 // 这种信息框可以阻塞主窗口 并且不会导致其关闭

                                                 QMessageBox::about(this, "Tip", "Edited expense approved!");

                                             } else {
                                                 // 创建信息框
                                                 QMessageBox::about(this, "提示", "审核失败");
                                             }
                                             //TODO 在此之前应当涉及一个数据的更新使得主窗口重新发送信息请求
                                             //

                                             //释放窗口资源
                                             close();
                                         }).json(Format::JsonToQString(sendInfo)).post();

}

void OrderCheckingPrompt::on_deny_clicked()
{
                                     //装配要发送的数据
                                     QJsonObject sendInfo;
                                     sendInfo["applyid"] = applyId;
                                     sendInfo["conf"] = 0;
                                     sendInfo["checker"] = userName;
                                     sendInfo["confirmRemark"] = "没问题";

                                     HttpClient("http://" + netWorkIpAddress + ":8095/comfirm_tug").success([=](const QString &response) {
                                                                                        QJsonObject responseInfo = Format::QStringToJson(response);
                                                                                            if (responseInfo["code"].toInt() == 200) {
                                                                                            // 创建信息框
                                                                                            // 这种信息框可以阻塞主窗口 并且不会导致其关闭

                                                                                            QMessageBox::about(this, "Tip", "Edited expense denied!");

                                                                                        } else {
                                                                                            // 创建信息框
                                                                                            QMessageBox::about(this, "提示", "审核失败");
                                                                                        }
                                                                                        //TODO 在此之前应当涉及一个数据的更新使得主窗口重新发送信息请求
                                                                                        //

                                                                                        //释放窗口资源
                                                                                        close();
                                                                                    }).json(Format::JsonToQString(sendInfo)).post();

}

void OrderCheckingPrompt::on_getCheck_clicked()
{
     //TODO  生成支票
                                     //装配sendInfo
                                     QJsonObject sendInfo;
                                     sendInfo["tugapplyid"] = applyId;
                                     HttpClient("http://" + netWorkIpAddress + ":8095/get_piaoju").success([=](const QString &response) {
                                                                                        QJsonObject responseInfo = Format::QStringToJson(response);
                                                                                        produceExcelFile(responseInfo);
                                                                                        //释放窗口资源
                                                                                        close();
                                                                                    }).json(Format::JsonToQString(sendInfo)).post();


}



void OrderCheckingPrompt::on_cancel_clicked()
{
    //释放窗口资源
    close();
}


