#include "AlgoCameras.h"
#include <fstream>
#include <iostream>
#include <json/config.h>
#include <json/json.h>
#include <json/reader.h>
#include <sys/time.h>
#include <unistd.h>
#include "ifaddrs.h"
#include <QApplication>
#include <QDesktopWidget>

#include<cstring>

using namespace std;


extern QString _jsonsDir;
extern QMap<QString, QString> _modelTypes_dict;

/////////////////////////////////
AlgoCameras::AlgoCameras(QWidget* parent):QWidget(parent)
{
    this->setParent(parent);
    this->setGeometry(200, 200, 680, 520);
    this->init_UI();
    this->setWindowTitle("相机配置窗口");
    this->setWindowFlags(Qt::Tool);
    this->setStyleSheet("background-color:rgb(30,41,54)");


}

AlgoCameras::~AlgoCameras() {}

void AlgoCameras::init_UI()
{
    int w90 = 90;
    int w150 = 400;
    int h = 30;
    int h80 = 60;

    QLabel* label_0 = newLabel(this, "存图目录:", w90, h, "border: 0px; font-size: 18px;");
    lineEdit_0 = newLineEdit(this, "", w150, h, {0,0});
    lineEdit_0->setReadOnly(true);
    MyWidget* w_0 = new MyWidget(this);
    newHBoxLayout(w_0, {label_0, lineEdit_0}, 0, 8, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* label_1 = newLabel(this, "文件格式:", w90, h, "border: 0px; font-size: 18px;");
    comboBox_1 = newComboBox(this, {"SN序列号_时间_图像序号_结果序号"}, "SN序列号_时间_图像序号_结果序号", w150, h, 0);
    MyWidget* w_1 = new MyWidget(this);
    newHBoxLayout(w_1, {label_1, comboBox_1}, 0, 8, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);
    connect(comboBox_1, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AlgoCameras::on_comboBox_1_changed);

    QLabel* label_2 = newLabel(this, "存图格式:", w90, h, "border: 0px; font-size: 18px;");
    comboBox_2 = newComboBox(this, {"jpg", "bmp", "png (不建议, 耗时久)"}, _saveSuffix, w150, h, 0);
    MyWidget* w_2 = new MyWidget(this);
    newHBoxLayout(w_2, {label_2, comboBox_2}, 0, 8, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);
    connect(comboBox_2, &QComboBox::currentTextChanged, this, &AlgoCameras::on_comboBox_2_changed);

    QLabel* label_3 = newLabel(this, "保存天数:", w90, h, "border: 0px; font-size: 18px;");
    lineEdit_3 = newLineEdit(this, QString::number(_saveDays), w150, h, {0,0});
    MyWidget* w_3 = new MyWidget(this);
    w_3->setToolTip("<=0，代表不自动清除；>0代表自动清除N天前的图像");
    newHBoxLayout(w_3, {label_3, lineEdit_3}, 0, 8, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* label_4 = newLabel(this, "保存选择:", w90, h, "border: 0px; font-size: 18px;");
    QString info = _saveImgsNums == 0 ? "保存全部图像" : (_saveImgsNums < 0 ? "不保存图像" : "保存部分图像");
    comboBox_4 = newComboBox(this, {info}, info, w150, h, 0);
    MyWidget* w_4 = new MyWidget(this);
    newHBoxLayout(w_4, {label_4, comboBox_4}, 0, 8, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);
    connect(comboBox_4, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AlgoCameras::on_comboBox_1_changed);


    MyWidget* wLeft = new MyWidget(this);
    newVBoxLayout(wLeft, {w_0, w_1, w_2, w_3, w_4}, 0, 10, {20,10,10,10}, Qt::AlignLeft | Qt::AlignVCenter);

//    button_remove = newPushButton(this, "清除当前图像", "", w150/2, h80, "",
//                                  "QPushButton{border: 0px; background-color: rgb(248,173,23);font-size: 18px; border-radius:5px;} "
//                                  "QPushButton:hover{background-color:rgb(213,146,6);}");
//    connect(button_remove, &QPushButton::clicked, this, &AlgoCameras::on_button_remove_clicked);

//    QWidget* wSpacer = new MyWidget(this);
//    wSpacer->setFixedHeight(10);
//    wSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

//    MyWidget* wTop = new MyWidget(this);
//    newHBoxLayout(wTop, {wLeft/*, button_remove, wSpacer*/}, 0, 20,{0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    QWidget* wLine = newWidgetLine(this, 0, 2, "border: 1px solid rgb(200, 200, 200);");
    wLine->setMinimumWidth(200);


    listWidget = new QListWidget(this);
    listWidget->setLineWidth(0);
    listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setSpacing(10);
    listWidget->setContentsMargins(0,0,0,0);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setStyleSheet("QListWidget{border: 0px;} "
                              "QListWidget::item{border: 0px;} "
                              "QListWidget::item:hover{border: 0px;} "
                              "QListWidget::item:selected{border: 0px;}");

    for (int i=1;i<6;i++)
    {
        init_camera(i, true);
    }
    // 总
    newVBoxLayout(this, {wLeft, wLine, listWidget}, 0, 8, {10,10,10,10}, Qt::AlignTop | Qt::AlignLeft);
}

void AlgoCameras::init_camera(int net, int state)
{
    QLabel* label_1 = newLabel(this, QString("相机%1:").arg(net), 60, 30, "font-weight: bold; font-size:16px;");
    QLabel* label_2 = newLabel(this, "IP: ", 30, 30, "font-size:16px;");
    QLabel* label_2_1;
    if (net == 2)
        label_2_1 = newLabel(this, "192.168.22.100", 150, 30, "font-size:16px;");
    else
        label_2_1 = newLabel(this, QString("192.168.%1.100").arg(net), 150, 30, "font-size:16px;");
    label_2_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel* label_3 = newLabel(this, "触发方式: ", 80, 30, "font-size:16px;");
    QLabel* label_3_1;
    if (net == 3)
        label_3_1 = newLabel(this, "Line1", 80, 30, "font-size:16px;");
    else
        label_3_1 = newLabel(this, _saveTriggerMode, 80, 30, "font-size:16px;");
    QLabel* label_4 = newLabel(this, "状态:", 50, 30, "font-size:16px;");
    QLabel* label_4_1 = newLabel(this, "", 22, 30, "font-size:16px;");
    label_4_1->setAlignment(Qt::AlignCenter);
    QLabel* label_5 = newLabel(this, "", 110, 30, "font-size:16px;");
    
    MyWidget* w = new MyWidget(this);
    newHBoxLayout(w, {label_1, label_2, label_2_1, label_3, label_3_1, label_4, label_4_1, label_5}, 0, 5, {0,0,0,0}, Qt::AlignLeft|Qt::AlignVCenter);

    _labels.append(label_1);
    _labels.append(label_2_1);
    _labels.append(label_3_1);
    _labels.append(label_4_1);
    _labels.append(label_5);

    QListWidgetItem* item = new QListWidgetItem(listWidget);
    item->setSizeHint(QSize(650, 30));
    listWidget->addItem(item);
    listWidget->setItemWidget(item, w);
}
void AlgoCameras::update_cameras(int net, QString ipStr, int state)
{
//    qDebug() << net << "   "  << ipStr << "  " << state;
    int idx = net - 1;
    int nums = 5;
    _labels[idx * nums + 0]->setText(QString("相机%1:").arg(net));
    _labels[idx * nums + 1]->setText(ipStr);
    if (state == 0){
        _labels[idx * nums + 3]->setPixmap(QPixmap(":/uimain/icon/a2.png").scaled(20,20));
        _labels[idx * nums + 4]->setText("相机连接正常");
    }
    else{
        _labels[idx * nums + 3]->setPixmap(QPixmap(":/uimain/icon/a3.png").scaled(20,20));
        _labels[idx * nums + 4]->setText("相机连接失败");

    }
    listWidget->update();
}

void AlgoCameras::updateShow(QString dir)
{
    this->lineEdit_0->setText(dir);
    this->lineEdit_0->setToolTip(dir);
}

void AlgoCameras::on_comboBox_1_changed(int idx)
{
    _saveFormatIdx = idx;
}

void AlgoCameras::on_comboBox_2_changed(QString text)
{
    if (text.contains("(")){
        QStringList texts = text.split("(");
        _saveSuffix = texts[0].remove(" ");
    }
    else{
        _saveSuffix = text;
    }
    qDebug() << _saveSuffix;
}


void AlgoCameras::on_button_remove_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(nullptr, "提示", "是否清除当前图像文件夹？\n  " + this->lineEdit_0->text(), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes){
        if (clearFilesFromDir(this->lineEdit_0->text())){
            QMessageBox::information(nullptr, "提示", "已清除当前图像文件夹： \n  " + this->lineEdit_0->text());
        }
    }
}















