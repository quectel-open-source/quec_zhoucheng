#include "AlgoSetting.h"
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
#include <QtConcurrent>

#include<cstring>

using namespace std;


extern QString _jsonsDir;
extern QMap<QString, QString> _settingJsons;
QMap<QString, QString> _modelTypes_dict;

/////////////////////////////////
AlgoSetting::AlgoSetting(QWidget* parent):QWidget(parent)
{
    this->setParent(parent);
    _pre_counts = {2, 14, 6};
    // 显示
    _isLoaded = false;
    _isMoving = false;
    _isViewShow = false;
    _viewWidget = new MyWidget(parent);
    _viewWidget->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _viewWidget->setStyleSheet("background-color:rgb(30,41,54);");
    this->init_view();

    this->init_char();
    this->init_UI();

    this->setFixedSize(460, 900);
    this->move(200, 120);
    this->setWindowTitle("算法设置窗口");
    this->setWindowFlags(Qt::Tool);
    this->setMouseTracking(true);
    this->setStyleSheet("background-color:rgb(30,41,54);");
    //
    widget_add = new MyWidget(parent);
    widget_add->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint | Qt::Tool);
    widget_add->setWindowTitle("增加型号");
    widget_add->setStyleSheet("background-color:rgb(30,41,54);");
    this->init_UI_add();

    // load jsons dir
    QDir dir(_jsonsDir);
    if (dir.exists()){
        QStringList dirNames = dir.entryList(QDir::Dirs);
        dirNames.removeOne(".");
        dirNames.removeOne("..");
        printf("[%s] load_count=%d\n", __func__, dirNames.count());
        foreach(QString dirname, dirNames){
            _modelTypes_dict[dirname] = _jsonsDir + "/" + dirname;
            comboBox_zhouCheng->addItem(dirname);
            qDebug() << "  " << dirname;
        }

        qDebug() << _modelTypes_dict;
        if (_modelTypes_dict.keys().contains(_settingJsons["model_default"])){
            comboBox_zhouCheng->setCurrentText(_settingJsons["model_default"]);
        }

        printf("\n");
    }
    else{
        printf("[%s] jsonDir is not exist: %s!!!\n", __func__, _jsonsDir.toLatin1().data());
    }

    // read default current 4 jsons
//    QtConcurrent::run(this, &AlgoSetting::JsonSelectAndShow, _settingJsons["model_default"]);


}

AlgoSetting::~AlgoSetting() {}

void AlgoSetting::closeEvent(QCloseEvent* event)
{
    if (_viewWidget){
        _viewWidget->close();
    }
}
void AlgoSetting::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton){
        _posMousePress = event->globalPos();
        _isMoving = true;
    }
}
void AlgoSetting::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton){
        _isMoving = false;
    }
}
void AlgoSetting::mouseMoveEvent(QMouseEvent* event)
{
    if (_isMoving){
        _viewWidget->move(_posShow + event->globalPos() - _posMousePress);
//        _viewWidget->show();
    }
}

void AlgoSetting::getAlgo_loadedState()
{
    _isLoaded = true;
}


void AlgoSetting::init_UI()
{
    int w = 90;
    int ww = 70;
    int h = 30;
    int www = 120;

    QLabel* label_zhouCheng = newLabel(this, "轴承型号:", w, h, "border: 0px; font-size: 17px;");
    MyWidget* w_no0 = new MyWidget(this);
    newVBoxLayout(w_no0, {label_zhouCheng}, 0, 10, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    comboBox_zhouCheng = newComboBox(this, {}, "请输入轴承型号", www, h);
    MyWidget* w_no1 = new MyWidget(this);
    newVBoxLayout(w_no1, {comboBox_zhouCheng}, 0, 10, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    QToolButton* button_zhouCheng = newToolButton(this, "增加型号", "", w, h, "", false, false, Qt::ToolButtonTextOnly);
    MyWidget* w_btn = new MyWidget(this);
    newVBoxLayout(w_btn, {button_zhouCheng}, 0, 10, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    MyWidget* wHead = new MyWidget(this);
    newHBoxLayout(wHead, {w_no1, w_btn}, 0, 10, {0,0,0,0}, Qt::AlignLeft | Qt::AlignTop);
    connect(comboBox_zhouCheng, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AlgoSetting::on_comboBox_zhouCheng_changed);
    connect(button_zhouCheng, &QToolButton::clicked, this, &AlgoSetting::on_toolButton_zhouCheng_clicked);

    ///////////////////////////////////
    this->init_UI_pre();
    MyWidget* widget_pre = new MyWidget(this);
    newHBoxLayout(widget_pre, _widgets_pre, 0, 0, {0, 0, 0, 10});
    this->initState_widget_pre(true, false, false, false);

    QLabel* label_detectLocate = newLabel(this, "检测位置:", w, h, "border: 0px; font-size: 17px;");
    QString qss_cb = "QCheckBox {border: 0px; font-size: 17px;} QCheckBox::indicator{width: 16px; height: 16px;}";
    checkBox_neiDaoJiao = newCheckBox(this, "内倒角面", ww + 25, h, false, qss_cb);
    checkBox_duanMian = newCheckBox(this, "端面", ww-15, h, true, qss_cb);
    checkBox_waiHuan = newCheckBox(this, "外环面", ww+5, h, false, qss_cb);
    checkBox_neiHuan = newCheckBox(this, "内环面", ww+5, h, false, qss_cb);

    MyWidget* wLocate = new MyWidget(this);
    newHBoxLayout(wLocate, {checkBox_neiHuan, checkBox_duanMian, checkBox_waiHuan}, 0, /*8*/20, {0, 0, 0, 0}, Qt::AlignLeft | Qt::AlignVCenter);
    connect(checkBox_neiDaoJiao, &QCheckBox::clicked, this, [=](){on_checkBox_clicked(checkBox_neiDaoJiao);});
    connect(checkBox_duanMian, &QCheckBox::clicked, this, [=](){on_checkBox_clicked(checkBox_duanMian);});
    connect(checkBox_neiHuan, &QCheckBox::clicked, this, [=](){on_checkBox_clicked(checkBox_neiHuan);});
    connect(checkBox_waiHuan, &QCheckBox::clicked, this, [=](){on_checkBox_clicked(checkBox_waiHuan);});
    checkBox_neiDaoJiao->hide();

    QWidget* wLine = newWidgetLine(this, 0, 2, "border: 1px solid rgb(200, 200, 200);");
    QWidget* wLine2 = newWidgetLine(this, 0, 2, "border: 1px solid rgb(200, 200, 200);");
    wLine2->setMinimumWidth(200);

    checkBox_setROI = newRadioButton(this, "是否设置ROI区域", www+50, h, true, "QRadioButton {border: 0px; font-size: 17px;}  QRadioButton::indicator{width: 20px; height: 20px;}");
    connect(checkBox_setROI, &QRadioButton::clicked, this, &AlgoSetting::on_checkBox_setROI_clicked);
    _wROI = new MyWidget(this);
    newHBoxLayout(_wROI, {checkBox_setROI}, 0, 0, {0, 0, 0, 0}, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* label_defectType = newLabel(this, "缺陷类别:", w, h, "border: 0px; font-size: 17px;");
    // 后处理checkBox
    wLabels_DuanMian = new MyWidget(this);
    wLabels_NeiHuan = new MyWidget(this);
    wLabels_neiDaoJiao= new MyWidget(this);
    wLabels_WaiHuanMian = new MyWidget(this);
    MyWidget* wPost = new MyWidget(this);
    newVBoxLayout(wPost, {wLabels_DuanMian, wLabels_NeiHuan, wLabels_neiDaoJiao, wLabels_WaiHuanMian}, 0, 0, {0,0,0,0}, Qt::AlignLeft | Qt::AlignTop);

    QLabel* label0 = newLabel(this, "置信度阈值:", w, h);
    lineEdit_score = newLineEdit(this, "", www, h);
    connect(lineEdit_score, &QLineEdit::returnPressed, this, [=](){update_Post_lineEdit(lineEdit_score->text(), false);});

    QLabel* label1 = newLabel(this, "缺陷面积阈值:", w, h);
    lineEdit_area = newLineEdit(this, "", www, h);
    connect(lineEdit_area, &QLineEdit::returnPressed, this, [=](){update_Post_lineEdit(lineEdit_area->text());});
    QLabel* label2 = newLabel(this, "缺陷个数阈值:", w, h);
    lineEdit_number = newLineEdit(this, "", www, h);
    connect(lineEdit_number, &QLineEdit::returnPressed, this, [=](){update_Post_lineEdit(lineEdit_number->text());});
    QLabel* label3 = newLabel(this, "缺陷长度阈值:", w, h);
    lineEdit_length = newLineEdit(this, "", www, h);
    connect(lineEdit_length, &QLineEdit::returnPressed, this, [=](){update_Post_lineEdit(lineEdit_length->text());});
    QLabel* label4 = newLabel(this, "平均灰度阈值:", w, h);
    lineEdit_gray = newLineEdit(this, "", www, h);
    connect(lineEdit_gray, &QLineEdit::returnPressed, this, [=](){update_Post_lineEdit(lineEdit_gray->text());});
    QLabel* label5 = newLabel(this, "最小面积阈值:", w, h);
    lineEdit_area_min = newLineEdit(this, "", www, h);
    connect(lineEdit_area_min, &QLineEdit::returnPressed, this, [=](){update_Post_lineEdit(lineEdit_area_min->text());});
    // 12345
    wParam12345 = new MyWidget(this);
    QGridLayout* layout1234 = new QGridLayout(wParam12345);
    layout1234->addWidget(label0, 0, 0, 1, 1);
    layout1234->addWidget(lineEdit_score, 0, 1, 1, 3);
    layout1234->addWidget(label1, 1, 0, 1, 1);
    layout1234->addWidget(lineEdit_area, 1, 1, 1, 3);
    layout1234->addWidget(label2, 2, 0, 1, 1);
    layout1234->addWidget(lineEdit_number, 2, 1, 1, 3);
    layout1234->addWidget(label3, 3, 0, 1, 1);
    layout1234->addWidget(lineEdit_length, 3, 1, 1, 3);
    layout1234->addWidget(label4, 4, 0, 1, 1);
    layout1234->addWidget(lineEdit_gray, 4, 1, 1, 3);
    layout1234->addWidget(label5, 5, 0, 1, 1);
    layout1234->addWidget(lineEdit_area_min, 5, 1, 1, 3);

    layout1234->addWidget(_widget_char, 1, 6, 4, 1);
    layout1234->setSpacing(10);
    layout1234->setContentsMargins(10, 0, 10, 10);
    layout1234->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    buttonYes = newToolButton(this, " 保存 参数 ", "", 180, 60, "", false, false, Qt::ToolButtonTextBesideIcon);
    buttonYes->setStyleSheet("QToolButton{border: 0px; background-color: rgb(3,185,5);font-size: 18px; border-radius:5px;} \
         QToolButton:hover{background-color:rgb(2,140,2);}");
    buttonYes->setIconSize(QSize(20, 20));
    buttonYes->setIcon(newIcon(":/uimain/icon/u21.png", 30, 30));


    button_openShow = newToolButton(this, " 打开单张显示 ", "", 180, 60, "", false, false, Qt::ToolButtonTextBesideIcon);
    button_openShow->setStyleSheet("QToolButton{border: 0px; background-color: rgb(21,166,221);font-size: 18px; border-radius:5px;} \
         QToolButton:hover{background-color:rgb(15,117,155);}");
    button_openShow->setIconSize(QSize(20, 20));
    button_openShow->setIcon(newIcon(":/uimain/icon/u22.png", 30, 30));

    connect(buttonYes, &QToolButton::clicked, this, &AlgoSetting::on_toolButton_clicked);
    connect(button_openShow, &QToolButton::clicked, this, &AlgoSetting::on_toolbutton_openShow_clicked);
    MyWidget* wButtons = new MyWidget(this);
    newHBoxLayout(wButtons, {buttonYes, button_openShow}, 0, 40, {10, 0, 10, 0}, Qt::AlignCenter);
    wButtons->setMinimumHeight(40);
    wButtons->setStyleSheet("border: 0px; border-left: 1px solid rgb(43, 65, 89); border-right: 1px solid rgb(43, 65, 89)");


    // zong 1
    MyWidget* wParams = new MyWidget(this);
    wParams->setStyleSheet("border: 1px solid rgb(43, 65, 89); border-bottom: 0px; border-radius: 2px;");
    QGridLayout* layout = new QGridLayout(wParams);
    layout->addWidget(w_no0, 0, 0, 1, 1);
    layout->addWidget(wHead, 0, 1, 1, 3);
    layout->addWidget(wLine, 1, 0, 1, 4);
    layout->addWidget(label_detectLocate, 2, 0, 1, 1);
    layout->addWidget(wLocate, 2, 1, 1, 3);

    layout->addWidget(_wROI, 3, 0, 1, 4);
    layout->addWidget(widget_pre, 4, 0, 1, 4);

    layout->addWidget(wLine2, 5, 0, 1, 4);
    layout->addWidget(label_defectType, 6, 0, 2, 1);
    layout->addWidget(wPost, 6, 1, 2, 3);
    layout->addWidget(wParam12345, 8, 0, 1, 4);

    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    MyWidget* wSpace = new MyWidget(this);
    wSpace->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // 总 1
    wLeftTotal = new MyWidget(this);
    newVBoxLayout(wLeftTotal, {wParams, wSpace, wButtons}, 0, 0, {0, 0, 0, 10});
    wLeftTotal->setStyleSheet("border: 1px solid rgb(43, 65, 89); border-radius: 2px;");
    newHBoxLayout(this, {wLeftTotal}, 0, 5, {5,5,5,5});
}

void AlgoSetting::init_view()
{
    _view = new MyGraphicsView(this);
    connect(_view, &MyGraphicsView::emit_updateText, this, &AlgoSetting::view_textEdit_updated);
    connect(_view, &MyGraphicsView::emit_imageWH, this, &AlgoSetting::view_imageInfo_receivedd);

    // 总 2-1
    int www = 120;

    int hhh = 40;
    QToolButton* button_open = newToolButton(_viewWidget, " 打开图像 ", "", www, hhh, "", false, false, Qt::ToolButtonTextBesideIcon, "QToolButton{font-size: 16px;}");
    connect(button_open, &QToolButton::clicked, this, &AlgoSetting::on_toolButton_open_clicked);
    MyWidget* wB = new MyWidget(_viewWidget);
    newHBoxLayout(wB, {button_open});

    QToolButton* button_setROI = newToolButton(_viewWidget, "配置ROI", "", www, hhh, "", false, false, Qt::ToolButtonTextBesideIcon, "QToolButton{font-size: 16px;}");
    connect(button_setROI, &QToolButton::clicked, this, &AlgoSetting::on_buttonSetROI_clicked);
    _textEdit = newTextEdit(_viewWidget, -1, hhh);
    _textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _textEdit->setStyleSheet("border:1px solid rgb(120, 120, 120); color: white; font-size: 15px;");
    _roiWidget = new MyWidget(_viewWidget);
    newHBoxLayout(_roiWidget, {_textEdit, button_setROI}, 0, 12);

    QWidget* temp1 = new QWidget(_viewWidget);
    temp1->setFixedWidth(200);

    MyWidget* wRight1 = new MyWidget(_viewWidget);
    newHBoxLayout(wRight1, {wB, temp1, _roiWidget}, 0, 10, {0, 0, 0, 5}, Qt::AlignLeft | Qt::AlignVCenter);

    MyWidget* w = new MyWidget(_viewWidget);
    newVBoxLayout(w, {wRight1, _view});
    w->setStyleSheet("border: 1px solid rgb(43, 65, 89); border-radius: 2px;");

    newVBoxLayout(_viewWidget, {w}, 0, 0, {5,5,5,5});

}

void AlgoSetting::init_UI_pre()
{
    int w = 120;
    int h = 30;
    int w100 = 80;
    int rows;

    // 内环面
    QList<QWidget*> labels_NeiHuan_pre;
    for (int i=0; i< _pre_counts[0]; i++)
    {
        QLabel* lab1 = newLabel(this, "", w, h, "font-size:13px;");
        QLineEdit* liEdit1 = newLineEdit(this, "", w100, h);
        connect(liEdit1, &QLineEdit::returnPressed, this, [=](){update_Pre_lineEdit(i, liEdit1->text());});


        QWidget* w = new QWidget(this);
        newHBoxLayout(w, {lab1, liEdit1}, 0, 10);
        labels_NeiHuan_pre.append(w);

        _label_NeiHuan_pre.append(lab1);
        _lineEdits_NeiHuan_pre.append(liEdit1);
    }
    QWidget* w2 = new QWidget(this);
    QGridLayout* layout2 = new QGridLayout(w2);

    rows = _pre_counts[0] / 2;
    if(_pre_counts[0] % 2 == 1){
        rows += 1;
    }
    for (int i=0; i< rows; i++)
    {
        for (int j=0; j < 2; j++)
        {
            if (j + i*2 >= _pre_counts[0])
                break;
            layout2->addWidget(labels_NeiHuan_pre[j + i * 2], i, j, 1, 1);
        }
    }
    layout2->setHorizontalSpacing(10);
    layout2->setVerticalSpacing(10);
    layout2->setContentsMargins(10, 0, 10, 0);

    _widgets_pre.append(w2);

    // 端面
    QList<QWidget*> labels_DuanMian_pre;
    for (int i=0; i< _pre_counts[1]; i++)
    {
        QLabel* lab = newLabel(this, "", w, h, "font-size:13px;");
        QLineEdit* liEdit = newLineEdit(this, "", w100, h,{0,0});
        connect(liEdit, &QLineEdit::returnPressed, this, [=](){update_Pre_lineEdit(i, liEdit->text());});

        QWidget* w = new QWidget(this);
        newHBoxLayout(w, {lab, liEdit}, 0, 6);
        labels_DuanMian_pre.append(w);

        _label_DuanMian_pre.append(lab);
        _lineEdits_DuanMian_pre.append(liEdit);
    }

    QWidget* w1 = new QWidget(this);
    QGridLayout* layout1 = new QGridLayout(w1);

    rows = _pre_counts[1] / 2;
    if(_pre_counts[1 ]% 2 == 1){
        rows += 1;
    }
    for (int i=0; i< rows; i++)
    {
        for (int j=0; j < 2; j++)
        {
            if (i*2+j >= _pre_counts[1])
                break;
            layout1->addWidget(labels_DuanMian_pre[j + i * 2], i, j, 1, 1);
        }
    }
    layout1->setHorizontalSpacing(10);
    layout1->setVerticalSpacing(10);
    layout1->setContentsMargins(10, 0, 10, 0);

    _widgets_pre.append(w1);


    // 外环面
    QList<QWidget*> labels_WaiHuanMian_pre;
    for (int i=0; i< _pre_counts[2]; i++)
    {
        QLabel* lab3 = newLabel(this, "", w, h, "font-size:13px;");
        QLineEdit* liEdit3 = newLineEdit(this, "", w100, h);
        connect(liEdit3, &QLineEdit::returnPressed, this, [=](){update_Pre_lineEdit(i, liEdit3->text());});

        QWidget* w = new QWidget(this);
        newHBoxLayout(w, {lab3, liEdit3}, 0, 10);
        labels_WaiHuanMian_pre.append(w);

        _label_WaiHuanMian_pre.append(lab3);
        _lineEdits_WaiHuanMian_pre.append(liEdit3);
    }
    QWidget* w4 = new QWidget(this);
    QGridLayout* layout4 = new QGridLayout(w4);

    rows = _pre_counts[2] / 2;
    if(_pre_counts[2] % 2 == 1){
        rows += 1;
    }
    for (int i=0; i< rows; i++)
    {
        for (int j=0; j < 2; j++)
        {
            if (j + i*2 >= _pre_counts[2])
                break;
            layout4->addWidget(labels_WaiHuanMian_pre[j + i * 2], i, j, 1, 1);
        }
    }
    layout4->setHorizontalSpacing(10);
    layout4->setVerticalSpacing(10);
    layout4->setContentsMargins(10, 0, 10, 0);

    _widgets_pre.append(w4);

}

void AlgoSetting::init_UI_add()
{
    QRect desktop = QApplication::desktop()->availableGeometry();
    widget_add->move(desktop.width()/2 - widget_add->width()/2, desktop.height()/2 - widget_add->height()/2);

    QLabel* label = newLabel(this, "新型号名称: ", -1, 30, "border: 0px; font-size: 17px;");
    lineEdit_zchengName = newLineEdit(this, "", 120, 30);
    MyWidget* w1 = new MyWidget;
    newHBoxLayout(w1, {label, lineEdit_zchengName}, 0, 10, {10, 10, 10, 10}, Qt::AlignLeft | Qt::AlignVCenter);

    buttonYesName = newToolButton(this, "确认设置", "", 120, 50, "", false, false, Qt::ToolButtonTextOnly);
    connect(buttonYesName, &QToolButton::clicked, this, &AlgoSetting::on_buttonYesName_clicked);
    buttonYesName->setStyleSheet("QToolButton{border: 0px; background-color: rgb(3,185,5);font-size: 18px; border-radius:5px;} \
         QToolButton:hover{background-color:rgb(2,140,2);}");

    MyWidget* w2 = new MyWidget(this);
    newHBoxLayout(w2, {buttonYesName}, 0, 10, {10, 10, 10, 10}, Qt::AlignRight | Qt::AlignVCenter);

    newVBoxLayout(widget_add, {w1, w2}, 0, 10, {10, 10, 10, 10}, Qt::AlignLeft | Qt::AlignVCenter);
}

void AlgoSetting::init_char()
{
    QLabel* label_1 = newLabel(this, "请选择字符:", -1, -1, "border: 0px; font-size: 17px;");
    MyWidget* w_1 = new MyWidget(this);
    newVBoxLayout(w_1, {label_1}, 0, 0, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* label_2 = newLabel(this, "型 号:", -1, -1, "border: 0px; font-size: 14px;");
    comboBox_char_type = newComboBox(this, {}, "不设置型号", 90, 30);
    comboBox_char_type->setCurrentIndex(0);
//    connect(comboBox_char_type, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AlgoSetting::on_comboBox_char_type_changed);
    MyWidget* w_2 = new MyWidget(this);
    newHBoxLayout(w_2, {label_2, comboBox_char_type}, 0, 10, {10,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* label_3 = newLabel(this, "类 别:", -1, -1, "border: 0px; font-size: 14px;");
    comboBox_char_diff = newComboBox(this, {}, "不设置类别", 90, 30);
    comboBox_char_diff->setCurrentIndex(0);
//    connect(comboBox_char_diff, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AlgoSetting::on_comboBox_char_diff_changed);
    MyWidget* w_3 = new MyWidget(this);
    newHBoxLayout(w_3, {label_3, comboBox_char_diff}, 0, 10, {10,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);

    button_charYes = newToolButton(this, " 确  定 ", "", 100, 40, "", false, false, Qt::ToolButtonTextBesideIcon);
    button_charYes->setStyleSheet("QToolButton{border: 0px; background-color: rgb(3,185,5);font-size: 16px; border-radius:5px;} \
         QToolButton:hover{background-color:rgb(2,140,2);}");
    connect(button_charYes, &QToolButton::clicked, this, &AlgoSetting::on_button_charYes_clicked);
    MyWidget* w_4 = new MyWidget(this);
    newHBoxLayout(w_4, {button_charYes}, 0, 10, {10,0,0,0}, Qt::AlignCenter);

    _widget_char = new MyWidget(this);
    _widget_char->setStyleSheet("border: 1px solid rgb(119,183,244);");
    newVBoxLayout(_widget_char, {w_1, w_2, w_3, w_4}, 0, 10, {12,8,12,8}, Qt::AlignCenter);

}
void AlgoSetting::on_button_charYes_clicked()
{
    QString text1 = comboBox_char_type->currentText();
    QString text2 = comboBox_char_diff->currentText();
//    if (text1.isEmpty() || text1.isNull() || text1.contains("不")){
//        QMessageBox* mgBox1 = newMessageBox(this, "错误", "请输入正确的字符型号!");
//        mgBox1->exec();
//        return;
//    }

    QMessageBox* mgBox = newMessageBox(this, "提示", "确定选择该字符？\n  "+text1 + "  "+text2, QMessageBox::Yes | QMessageBox::No);
    if (mgBox->exec() != QMessageBox::Yes){
        return;
    }

    emit emit_sendChars(text1, text2);
}
void AlgoSetting::getCharSetting(QList<QString> keys)
{
    qDebug() << "load Chars: " << keys;
    for(int i=1; i<comboBox_char_type->count(); i++){
        comboBox_char_type->removeItem(i);
    }
    for(int i=1; i<comboBox_char_diff->count(); i++){
        comboBox_char_diff->removeItem(i);
    }

    foreach(QString key, keys)
    {
        QChar first = key.at(0);
        if (first.isDigit()){
            comboBox_char_type->addItem(key);
        }
        else{
            comboBox_char_diff->addItem(key);
        }
    }
    if (comboBox_char_type->count() > 0)
        comboBox_char_type->setCurrentIndex(1);
//    if (comboBox_char_diff->count() > 0)
//        comboBox_char_diff->setCurrentIndex(1);

    if (_dParams[1].chars.count() > 0){
        comboBox_char_type->setCurrentText(_dParams[1].chars[0]);
        comboBox_char_diff->setCurrentText(_dParams[1].chars[1]);

        emit emit_sendChars(comboBox_char_type->currentText(), comboBox_char_diff->currentText());
    }
}


void AlgoSetting::initState_checkBox(int i)
{
    for (int j=0; j< _checkBoxList.count() - 4; j++)
    {
        if (j != i){
            _checkBoxList[j]->setChecked(false);
        }
        else{
            _checkBoxList[j]->setChecked(true);
        }
    }
    printf("[%s] init checkBox completed!\n", __func__);
}

void AlgoSetting::initState_widget_pre(bool show_NeiHuan, bool show_DuanMian, bool show_WaiHuanMian, bool show_neiDaoJiao)
{
//    if (show_neiDaoJiao)
//        _widgets_pre[0]->show();
//    else
//        _widgets_pre[0]->hide();

    if (show_DuanMian)
        _widgets_pre[1]->show();
    else
        _widgets_pre[1]->hide();


    if (show_WaiHuanMian)
        _widgets_pre[2]->show();
    else
        _widgets_pre[2]->hide();

    if (show_NeiHuan)
        _widgets_pre[0]->show();
    else
        _widgets_pre[0]->hide();
}

/////////
void AlgoSetting::on_checkBox_clicked(QCheckBox* cb)
{
    QString text = cb->text().trimmed();
    qDebug() << "<<< click checkBox=" << text;

    if(text.compare("内倒角面") == 0){
//        checkBox_neiDaoJiao->setChecked(true);
//        checkBox_duanMian->setChecked(false);
//        checkBox_waiHuan->setChecked(false);
//        checkBox_neiHuan->setChecked(false);

//        checkBox_setROI->setChecked(false);
//        _wROI->show();
//        _widget_char->hide();

//        this->initState_widget_pre(true, false, false, false);
//        this->update_cBox_defectShow(_dParams[0].labels_CN, true, false, false, false);
//        this->on_checkBoxDefect_clicked(1, cBox_neiDaoJiao_list[0]);


//        updateLineEdit(0, _dParams[0].labels_params[_dParams[0].labels[0]]);
    }
    else if (text.compare("端面") == 0){
        checkBox_neiHuan->setChecked(false);
        checkBox_duanMian->setChecked(true);
        checkBox_waiHuan->setChecked(false);
        checkBox_neiDaoJiao->setChecked(false);

        checkBox_setROI->setChecked(false);
        _wROI->hide();
        _widget_char->show();

        this->initState_widget_pre(false, true, false, false);
        this->update_cBox_defectShow(_dParams[1].labels_CN, false, true, false, false);
        this->on_checkBoxDefect_clicked(2, cBox_DuanMian_list[0]);


        updateLineEdit(0, _dParams[1].labels_params[_dParams[1].labels[0]]);
    }
    else  if(text.compare("外环面") == 0){
        checkBox_neiHuan->setChecked(false);
        checkBox_duanMian->setChecked(false);
        checkBox_waiHuan->setChecked(true);
        checkBox_neiDaoJiao->setChecked(false);

        checkBox_setROI->setChecked(false);
        _wROI->show();
        _widget_char->hide();

        this->initState_widget_pre(false, false, true, false);
        this->update_cBox_defectShow(_dParams[2].labels_CN, false, false, true, false);
        this->on_checkBoxDefect_clicked(3, cBox_WaiHuanMian_list[0]);

        updateLineEdit(0, _dParams[2].labels_params[_dParams[2].labels[0]]);
    }
    else  if(text.compare("内环面") == 0) {
        checkBox_neiHuan->setChecked(true);
        checkBox_duanMian->setChecked(false);
        checkBox_waiHuan->setChecked(false);
        checkBox_neiDaoJiao->setChecked(false);

        checkBox_setROI->setChecked(false);
        _wROI->show();
        _widget_char->hide();

        this->initState_widget_pre(true, false, false, false);
        this->update_cBox_defectShow(_dParams[0].labels_CN, true, false, false, false);
        this->on_checkBoxDefect_clicked(1, cBox_NeiHuan_list[0]);


        updateLineEdit(0, _dParams[0].labels_params[_dParams[0].labels[0]]);
    }

//    if (this->isActiveWindow())
//        on_checkBox_setROI_clicked();
}


//////
void AlgoSetting::update_Pre_lineEdit(int idx, QString text)
{
    if (!IsValidText(text)){
        QMessageBox* mgBox = newMessageBox(this, "错误！", "请输入正确的参数 <整数> ！！！");
        mgBox->exec();
        return;
    }

    if (this->checkBox_duanMian->isChecked()){

        _dParams[1].pre_th[idx] = text.toInt();
        emit emit_update_preParams(2, _dParams[1].pre_th);
    }
    else if (this->checkBox_neiHuan->isChecked()){

        _dParams[0].pre_th[idx] = text.toInt();
        emit emit_update_preParams(1, _dParams[0].pre_th);
    }
    else if (this->checkBox_waiHuan->isChecked()){

        _dParams[2].pre_th[idx] = text.toInt();
        emit emit_update_preParams(3, _dParams[2].pre_th);
    }
    else if (this->checkBox_neiDaoJiao->isChecked()){

        _dParams[4].pre_th[idx] = text.toInt();
        emit emit_update_preParams(4, _dParams[4].pre_th);
    }
}

///////////////////
void AlgoSetting::update_Post_lineEdit(QString text, bool isInt)
{
    if (isInt){
        if (!IsValidText(text)){
            QMessageBox* mgBox = newMessageBox(this, "错误！", "请输入正确的参数 <整数> ！！！");
            mgBox->exec();
            return;
        }
    }
    else{
        if (!IsValidText(text, "float")){
            QMessageBox* mgBox = newMessageBox(this, "错误！", "请输入正确的参数 <小数 <1 > ！！！");
            mgBox->exec();
            return;
        }
        else{
            if (text.toFloat() > 1.0){
                QMessageBox* mgBox = newMessageBox(this, "错误！", "请输入正确的参数 <小数 <1 > ！！！");
                mgBox->exec();
                return;
            }
        }
    }

    if (this->checkBox_duanMian->isChecked()){
        for (int i = 0; i< _dParams[1].labels.count(); i++)
        {
            if (cBox_DuanMian_list[i]->isChecked()){
                QList<int> params = {
                    int(lineEdit_score->text().toFloat() * 100),
                    lineEdit_area->text().toInt(), lineEdit_number->text().toInt(), lineEdit_length->text().toInt(),
                    lineEdit_gray->text().toInt(), lineEdit_area_min->text().toInt()
                };
                _dParams[1].labels_params[_dParams[1].labels[i]] = params;
            }
        }
        emit emit_update_postParams(2, _dParams[1].labels, _dParams[1].labels_params);
    }

    else if (this->checkBox_neiHuan->isChecked()){
        for (int i = 0; i< _dParams[0].labels.count(); i++)
        {
            if (cBox_NeiHuan_list[i]->isChecked()){
                QList<int> params = {
                    int(lineEdit_score->text().toFloat() * 100),
                    lineEdit_area->text().toInt(), lineEdit_number->text().toInt(), lineEdit_length->text().toInt(),
                    lineEdit_gray->text().toInt(), lineEdit_area_min->text().toInt()
                };
                _dParams[0].labels_params[_dParams[0].labels[i]] = params;
            }
        }
        emit emit_update_postParams(1, _dParams[0].labels, _dParams[0].labels_params);
    }

    else{
        for (int i = 0; i< _dParams[2].labels.count(); i++)
        {
            if (cBox_WaiHuanMian_list[i]->isChecked()){
                QList<int> params = {
                    int(lineEdit_score->text().toFloat() * 100),
                    lineEdit_area->text().toInt(), lineEdit_number->text().toInt(), lineEdit_length->text().toInt(),
                    lineEdit_gray->text().toInt(), lineEdit_area_min->text().toInt()
                };
                _dParams[2].labels_params[_dParams[2].labels[i]] = params;
            }
        }
        emit emit_update_postParams(3, _dParams[2].labels, _dParams[2].labels_params);
    }
}

///////
void AlgoSetting::updateLineEdit(int idx, QList<int> params)
{
    try {
        if (idx == 0){
            lineEdit_score->setText(QString::number(params[0] * 0.01));
            lineEdit_area->setText(QString::number(params[1]));
            lineEdit_number->setText(QString::number(params[2]));
            lineEdit_length->setText(QString::number(params[3]));
            lineEdit_gray->setText(QString::number(params[4]));
            lineEdit_area_min->setText(QString::number(params[5]));
        }
        else if (idx == 1){
            lineEdit_thOut->setText(QString::number(params[0]));
            lineEdit_thIn->setText(QString::number(params[1]));
        }
        printf("[%s] update lineEdit completed!\n", __func__);
    }  catch (exception &e) {
        printf("[%s] update lineEdit completed!\n", __func__);
        QMessageBox* mgBox = newMessageBox(this, "错误", "配置文件后处理参数错误\n请检查是否有遗漏！");
        mgBox->exec();
    }
    
}


/////////
void AlgoSetting::update_cBox_defectShow(QList<QString> labels, bool show1, bool show2, bool show3, bool show4)
{
//    qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaa     " << labels;
    int ww = 90;
    int hh = 30;
    int spacing = 10;
    QString qss_cb = "QCheckBox {border: 0px; font-size: 17px;} QCheckBox::indicator{width: 16px; height: 16px;}";
    if (show2){
        if (cBox_DuanMian_list.count() == 0){
            QList<QWidget*> widgets;
            for (int i = 0; i< labels.count(); i++)
            {
                QCheckBox* checkBox_1 = newCheckBox(this, labels[i], ww, hh, false, qss_cb);
                connect(checkBox_1, &QCheckBox::clicked, this, [=](){on_checkBoxDefect_clicked(2, checkBox_1);});
                cBox_DuanMian_list.append(checkBox_1);
                widgets.append(checkBox_1);
            }
            update_checkBoxesLayOut(wLabels_DuanMian, widgets, spacing, {10,0,10,0});
        }

        wLabels_DuanMian->show();
    }
    else
        wLabels_DuanMian->hide();

    if (show1){
        if (cBox_NeiHuan_list.count() == 0){
            QList<QWidget*> widgets_2;
            for (int i = 0; i< labels.count(); i++)
            {
                QCheckBox* checkBox_2 = newCheckBox(this, labels[i], ww, hh, false, qss_cb);
                connect(checkBox_2, &QCheckBox::clicked, this, [=](){on_checkBoxDefect_clicked(1, checkBox_2);});
                cBox_NeiHuan_list.append(checkBox_2);
                widgets_2.append(checkBox_2);
            }

            update_checkBoxesLayOut(wLabels_NeiHuan, widgets_2, spacing, {10,0,10,0});
        }

        wLabels_NeiHuan->show();
    }
    else
        wLabels_NeiHuan->hide();

//    if (show4){
//        if (cBox_neiDaoJiao_list.count() == 0){
//            QList<QWidget*> widgets_3;
//            for (int i = 0; i< labels.count(); i++)
//            {
//                QCheckBox* checkBox_3 = newCheckBox(this, labels[i], ww, hh, false, qss_cb);
//                connect(checkBox_3, &QCheckBox::clicked, this, [=](){on_checkBoxDefect_clicked(3, checkBox_3);});
//                cBox_neiDaoJiao_list.append(checkBox_3);
//                widgets_3.append(checkBox_3);
//            }

//            update_checkBoxesLayOut(wLabels_neiDaoJiao, widgets_3, spacing, {10,0,10,0});
//        }

//        wLabels_neiDaoJiao->show();
//    }
//    else
//        wLabels_neiDaoJiao->hide();

    if (show3){
        if (cBox_WaiHuanMian_list.count() == 0){
            QList<QWidget*> widgets_4;
            for (int i = 0; i< labels.count(); i++)
            {
                QCheckBox* checkBox_4 = newCheckBox(this, labels[i], ww, hh, false, qss_cb);
                connect(checkBox_4, &QCheckBox::clicked, this, [=](){on_checkBoxDefect_clicked(3, checkBox_4);});
                cBox_WaiHuanMian_list.append(checkBox_4);
                widgets_4.append(checkBox_4);
            }

            update_checkBoxesLayOut(wLabels_WaiHuanMian, widgets_4, spacing, {10,0,10,0});
        }

        wLabels_WaiHuanMian->show();
    }
    else
        wLabels_WaiHuanMian->hide();

}



/////////
void AlgoSetting::on_checkBoxDefect_clicked(int camid, QCheckBox* cb)
{
    QString text = cb->text();
    qDebug() << "<<<<<< click checkBox: " << text;
    QList<QCheckBox*> wList;
    switch (camid)
    {
    case 4:
        wList = cBox_neiDaoJiao_list;
        break;
    case 2:
        wList = cBox_DuanMian_list;
        break;
    case 3:
        wList = cBox_WaiHuanMian_list;
        break;
    case 1:
        wList = cBox_NeiHuan_list;
        break;

    }

//    qDebug() << "2222222222222222222    " << _dParams[i-1].labels;
    for(int idx=0; idx< wList.count(); idx++)
    {
        QCheckBox* cBox = wList[idx];
        if (cBox->text() == text){
            cBox->setChecked(true);
            this->updateLineEdit(0, _dParams[camid-1].labels_params[_dParams[camid-1].labels[idx]]);
        }
        else{
            cBox->setChecked(false);
        }
    }

}


/////////////////// 确认按钮
void AlgoSetting::on_toolButton_clicked()
{
    QMessageBox* mgBox = newMessageBox(this, "提示", "是否保存当前参数设置?", QMessageBox::Yes | QMessageBox::No);
    if (mgBox->exec() != QMessageBox::Yes){
        return;
    }

//    if (this->checkBox_neiDaoJiao->isChecked()){
//        JsonIOWrite(0, _jsonPath[0]);
//    }
    if (this->checkBox_duanMian->isChecked()){
        JsonIOWrite(1, _jsonPath[1]);
    }
    else if (this->checkBox_waiHuan->isChecked()){
        JsonIOWrite(2, _jsonPath[2]);
    }
    else{
        JsonIOWrite(0, _jsonPath[0]);
    }

}


////////
int AlgoSetting::JsonIORead(int i, QString path, bool isFirstLoad)
{
    if (!QFile(path).exists()){
        printf("[%s] error read json_path: %s is not exist!\n", __func__, path.toLatin1().data());
        return -1;
    }
    printf("[%s] start read json_path: %s!\n", __func__, path.toLatin1().data());
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray all = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(all);
    if (doc.isNull() || doc.isEmpty()){
        printf("[%s] fail to load json_path: %s !\n", __func__, path.toLatin1().data());
        return -1;
    }

    QJsonObject obj = doc.object();  // get root
    if (isFirstLoad)
        _objs.append(obj);
    else
        _objs[i] = obj;
    QJsonObject objAll = obj.value("inference").toArray()[0].toObject();

    QJsonObject Seg_v3_model = objAll.value("Seg.v3_model").toObject();

    QJsonArray Seg_v3_model_pre = Seg_v3_model["pre"].toArray();
    QJsonArray Seg_v3_model_pre_CN = Seg_v3_model["pre_CN"].toArray();
    QJsonArray Seg_v3_model_pre_roi = Seg_v3_model["pre_roi"].toArray();
    QJsonObject Seg_v3_model_pre_params = Seg_v3_model["pre_params"].toObject();
    QJsonArray Seg_v3_model_label = Seg_v3_model["label"].toArray();
    QJsonArray Seg_v3_model_label_CN = Seg_v3_model["label_CN"].toArray();
    QJsonArray Seg_v3_model_label_all = Seg_v3_model["label_all"].toArray();
    QJsonObject Seg_v3_model_label_params = Seg_v3_model["label_params"].toObject();

    if (!Seg_v3_model["chars"].isNull()){
        _dParams[i].chars.append(Seg_v3_model["chars"].toArray()[0].toString());
        _dParams[i].chars.append(Seg_v3_model["chars"].toArray()[1].toString());
    }

    _dParams[i].pre.clear();
    _dParams[i].pre_th.clear();
    _dParams[i].image_roi.clear();
    for (QVariant pre_roi: Seg_v3_model_pre_roi.toVariantList())
    {
        QString pre1 = pre_roi.toString();
        _dParams[i].image_roi.append(pre1.toInt());
    }
    for (QVariant pre_label_CN: Seg_v3_model_pre_CN.toVariantList())
    {
        QString pre_CN = pre_label_CN.toString();
        _dParams[i].pre_CN.append(pre_CN);
    }
    for (QVariant pre_label: Seg_v3_model_pre.toVariantList())
    {
        QString pre = pre_label.toString();
        _dParams[i].pre.append(pre);
        _dParams[i].pre_th.append(Seg_v3_model_pre_params[pre].toInt());
    }
    switch (i)
    {
//    case 0:
//        for(int idx=0; idx < _lineEdits_NeiDaoJiao_pre.count(); idx++)
//        {
//            _lineEdits_NeiDaoJiao_pre[idx]->setText(QString::number(_dParams[0].pre_th[idx]));
//        }
//        break;
    case 1:
        for(int idx=0; idx < _lineEdits_DuanMian_pre.count(); idx++)
        {
            _label_DuanMian_pre[idx]->setText(_dParams[i].pre_CN[idx]);
            _lineEdits_DuanMian_pre[idx]->setText(QString::number(_dParams[i].pre_th[idx]));
        }
        break;
    case 2:
        for(int idx=0; idx < _lineEdits_WaiHuanMian_pre.count(); idx++)
        {
            _label_WaiHuanMian_pre[idx]->setText(_dParams[i].pre_CN[idx]);
            _lineEdits_WaiHuanMian_pre[idx]->setText(QString::number(_dParams[i].pre_th[idx]));
        }
        break;
    case 0:
        for(int idx=0; idx < _lineEdits_NeiHuan_pre.count(); idx++)
        {
            _label_NeiHuan_pre[idx]->setText(_dParams[i].pre_CN[idx]);
            _lineEdits_NeiHuan_pre[idx]->setText(QString::number(_dParams[i].pre_th[idx]));
        }
        break;
    }

    _dParams[i].labels.clear();
    _dParams[i].labels_CN.clear();
    _dParams[i].labels_all.clear();
    for(int m=0; m < Seg_v3_model_label_all.count(); m++)
    {
        _dParams[i].labels_all.append(Seg_v3_model_label_all[m].toString());
    }
    for (int id=0; id<Seg_v3_model_label.count(); id++)
    {
        QString la = Seg_v3_model_label[id].toString();
        _dParams[i].labels.append(la);
        _dParams[i].labels_CN.append(Seg_v3_model_label_CN[id].toString());

        _dParams[i].labels_params[la].clear();
        for (int n=0; n< _dParams[i].labels_all.count(); n++)
        {
            if (n == 0)
                _dParams[i].labels_params[la].append(int(Seg_v3_model_label_params[la].toObject()[_dParams[i].labels_all[n]].toDouble() * 100));
            else
                _dParams[i].labels_params[la].append(Seg_v3_model_label_params[la].toObject()[_dParams[i].labels_all[n]].toInt());
        }
    }

    printf("[%s] end success read json_path: %s!\n", __func__, path.toLatin1().data());

    return 0;
}

int AlgoSetting::JsonIOWrite(int id, QString jsonPath)
{
    printf("[%s]  start write json_path\n", __func__);

    QJsonArray inferArray = _objs[id].value("inference").toArray();
    QJsonObject objAll = inferArray[0].toObject();
    QJsonObject Seg_v3_model = objAll.value("Seg.v3_model").toObject();

    QJsonArray Seg_v3_model_pre_roi       = Seg_v3_model["pre_roi"].toArray();
    QJsonObject Seg_v3_model_pre_params   = Seg_v3_model["pre_params"].toObject();
    QJsonObject Seg_v3_model_label_params = Seg_v3_model["label_params"].toObject();
    if (!Seg_v3_model["chars"].isNull()){
        QJsonArray Seg_v3_model_char   = Seg_v3_model["chars"].toArray();
        Seg_v3_model_char[0]  = comboBox_char_type->currentText();
        Seg_v3_model_char[1]  = comboBox_char_diff->currentText();
        Seg_v3_model["chars"] = Seg_v3_model_char;
    }

    QList<QLineEdit*> lineEdits;
    QList<QCheckBox*> checkBoxes;
    switch (id)
    {
//    case 0:
//        lineEdits = _lineEdits_NeiDaoJiao_pre;
//        checkBoxes = cBox_neiDaoJiao_list;
//        break;
    case 1:
        lineEdits = _lineEdits_DuanMian_pre;
        checkBoxes = cBox_DuanMian_list;
        break;
    case 2:
        lineEdits = _lineEdits_WaiHuanMian_pre;
        checkBoxes = cBox_WaiHuanMian_list;
        break;
    case 0:
        lineEdits = _lineEdits_NeiHuan_pre;
        checkBoxes = cBox_NeiHuan_list;
        break;
    }

    for(int i=0; i<_dParams[id].image_roi.count(); i++)
    {
        Seg_v3_model_pre_roi[i] = _dParams[id].image_roi[i];
    }
    for(int i=0; i< _dParams[id].pre.count(); i++)
    {
        Seg_v3_model_pre_params[_dParams[id].pre[i]] = lineEdits[i]->text().toInt();
    }
    for(int i=0; i< _dParams[id].labels.count(); i++)
    {
        QJsonObject param;
        if (checkBoxes[i]->isChecked()){
            param["score"] = lineEdit_score->text().toFloat();
            param["area"] = lineEdit_area->text().toInt();
            param["number"] = lineEdit_number->text().toInt();
            param["length"] = lineEdit_length->text().toInt();
            param["gray"] = lineEdit_gray->text().toInt();
            param["area_min"] = lineEdit_area_min->text().toInt();
        }
        else{
            QList<int> post_params = _dParams[id].labels_params[_dParams[id].labels[i]];
            param["score"] = post_params[0] * 0.01;
            param["area"] = post_params[1];
            param["number"] = post_params[2];
            param["length"] = post_params[3];
            param["gray"] = post_params[4];
            param["area_min"] = post_params[5];
        }

        Seg_v3_model_label_params[_dParams[id].labels[i]] = param;
    }

    Seg_v3_model["pre_roi"]      = Seg_v3_model_pre_roi;
    Seg_v3_model["pre_params"]   = Seg_v3_model_pre_params;
    Seg_v3_model["label_params"] = Seg_v3_model_label_params;
    objAll["Seg.v3_model"] = Seg_v3_model;
    inferArray[0] = objAll;
    _objs[id]["inference"] = inferArray;

    QJsonDocument doc(_objs[id]);
    QByteArray json = doc.toJson();

    QFile file(jsonPath);
    file.open(QFile::WriteOnly);
    file.write(json);
    file.close();
    printf("[%s]  end write json_path: success!  %s\n", __func__, jsonPath.toLatin1().data());

    lineEdits.clear();
    checkBoxes.clear();
    return 0;
}


///////////////////////// 选择哪种型号的json，并显示
void AlgoSetting::JsonSelectAndShow(const QString dirname)
{
    QString dir = _modelTypes_dict[dirname];
    qDebug() << dirname;

    _jsonPath.clear();
    _jsonPath << dir + "/model_1.json" << dir + "/model_2.json" << dir + "/model_3.json";

    // read to show
    JsonIORead(0, _jsonPath[0], true);
    JsonIORead(1, _jsonPath[1], true);
    JsonIORead(2, _jsonPath[2], true);
//    JsonIORead(3, _jsonPath[3], true);

    emit emit_update_preParams(1, _dParams[0].pre_th);
    emit emit_update_preParams(2, _dParams[1].pre_th);
    emit emit_update_preParams(3, _dParams[2].pre_th);
//    emit emit_update_preParams(4, _dParams[3].pre_th);

    emit emit_update_postParams(1, _dParams[0].labels, _dParams[0].labels_params);
    emit emit_update_postParams(2, _dParams[1].labels, _dParams[1].labels_params);
    emit emit_update_postParams(3, _dParams[2].labels, _dParams[2].labels_params);
//    emit emit_update_postParams(4, _dParams[3].labels, _dParams[3].labels_params);

    on_checkBox_clicked(checkBox_duanMian);
    qDebug() << "加载模型设置UI完毕！";

}

void AlgoSetting::on_comboBox_zhouCheng_changed(int idx)
{
    if (idx < 1 && _isLoaded){
        comboBox_zhouCheng->setCurrentIndex(1);
        return;
    }

    QString text = comboBox_zhouCheng->currentText();
    printf("[%s] comboBox: idx=%d, text=%s\n", __func__, idx, text.toLatin1().data());
    JsonSelectAndShow(text);

}

void AlgoSetting::on_toolButton_zhouCheng_clicked()
{
    lineEdit_zchengName->clear();

    widget_add->show();
}

/////
void AlgoSetting::on_toolbutton_openShow_clicked()
{
    if (!_isViewShow){
        button_openShow->setText(" 关闭单张显示 ");
        QPoint pt = this->mapToGlobal(QPoint(0, 0));
        _posShow = pt + QPoint(this->width(), 0);

        _viewWidget->setGeometry(_posShow.x(), _posShow.y(), 850, this->wLeftTotal->height()+10);
        _viewWidget->show();

    }
    else{
        button_openShow->setText(" 打开单张显示 ");
        _viewWidget->hide();
    }
    _isViewShow = !_isViewShow;
}


bool qCopyDirectory(const QDir& fromDir, const QDir& toDir, bool bCoverIfFileExists=true)
{
    QDir formDir_ = fromDir;
    QDir toDir_ = toDir;

    if(!toDir_.exists()) {
        if(!toDir_.mkdir(toDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = formDir_.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        //拷贝子目录
        if(fileInfo.isDir()){
            //递归调用拷贝
            if(!qCopyDirectory(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName()),true))
                return false;
        }
        //拷贝子文件
        else{
            if(bCoverIfFileExists && toDir_.exists(fileInfo.fileName())){
                toDir_.remove(fileInfo.fileName());
            }
            if(!QFile::copy(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName()))){
                return false;
            }
        }
    }
    return true;
}

void AlgoSetting::on_buttonYesName_clicked()
{
    QString dirname = lineEdit_zchengName->text();
    if (dirname.isEmpty() || dirname.isNull() || dirname == ""){
        QMessageBox* mgBox = newMessageBox(this, "错误", QString("请输入正确的型号名称!!! \n<新型号: %1>").arg(dirname));
        mgBox->exec();
        return;
    }
    comboBox_zhouCheng->addItem(dirname);
    comboBox_zhouCheng->setCurrentIndex(comboBox_zhouCheng->count() - 1);

    printf("[%s]  ----> add ZhouCheng Type: %s\n", __func__, dirname.toLatin1().data());

    // refresh
    _modelTypes_dict[dirname] = _jsonsDir + QString("/%1").arg(dirname);

    bool isCopyYes = false;
    isCopyYes = qCopyDirectory(_modelTypes_dict[comboBox_zhouCheng->itemText(1)], _modelTypes_dict[dirname]);
    if (isCopyYes)
        printf("[%s]  ----> create new ZhouCheng succeaa: %s\n", __func__, dirname.toLatin1().data());
    else
        printf("[%s]  ----> create new ZhouCheng error: %s\n", __func__, dirname.toLatin1().data());

    widget_add->close();
    QMessageBox* mgBox = newMessageBox(this, "添加新型号", QString("添加新型号成功!!! \n<新型号: %1>").arg(dirname));
    mgBox->exec();
    JsonSelectAndShow(dirname);
}


/////
void AlgoSetting::on_toolButton_open_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.bmp *.jpeg)");
    if (imagePath.isEmpty()) {
        return;
    }

    _view->addItem(imagePath);
}

void AlgoSetting::on_checkBox_setROI_clicked()
{
    if (_view->_pixmapItem == NULL){
        QMessageBox* mgBox = newMessageBox(this, "提示", "请先打开图像");
        mgBox->exec();
        this->checkBox_setROI->setChecked(false);
        return;
    }

    _view->open_pixmap_crossLine(this->checkBox_setROI->isChecked());
    if (this->checkBox_setROI->isChecked()){
        int idx = 1;
        if (this->checkBox_neiHuan->isChecked()){
            idx = 2;
        }
        else if (this->checkBox_neiHuan->isChecked()){
            idx = 3;
        }
        QString text = QString("(%1, %2, %3, %4)")
                .arg(_dParams[idx].image_roi[0]).arg(_dParams[idx].image_roi[1]).arg(_dParams[idx].image_roi[2]).arg(_dParams[idx].image_roi[3]);
        _textEdit->setPlainText(text);
        _textEdit->setToolTip(text);

        _roiWidget->show();
    }
    else{
        _roiWidget->hide();
    }
}

////
void AlgoSetting::on_buttonSetROI_clicked()
{
    QString text = _textEdit->toPlainText();
    text = text.replace("(", "");
    text = text.replace(")", "");
    QStringList texts = text.split(", ");

//    if (this->checkBox_neiDaoJiao->isChecked()){
//        _dParams[1].image_roi = {texts[0].toInt(), texts[1].toInt(), texts[2].toInt(), texts[3].toInt()};
//        emit_update_preROI(2, _dParams[1].image_roi);
//    }
//    else if (this->checkBox_neiHuan->isChecked()){
//        _dParams[2].image_roi = {texts[0].toInt(), texts[1].toInt(), texts[2].toInt(), texts[3].toInt()};
//        emit_update_preROI(3, _dParams[2].image_roi);
//    }
//    else if (this->checkBox_waiHuan->isChecked()){
//        _dParams[3].image_roi = {texts[0].toInt(), texts[1].toInt(), texts[2].toInt(), texts[3].toInt()};
//        emit_update_preROI(7, _dParams[3].image_roi);
//    }
}

void AlgoSetting::view_textEdit_updated(QString text)
{
    _textEdit->clear();
    _textEdit->setText(text);
    _textEdit->setToolTip(text);
}

void AlgoSetting::view_imageInfo_receivedd(int w, int h)
{
    QString text = QString(" w: %1, h: %2").arg(w).arg(h);
    _viewLabel->setText(text);
    _viewLabel->setToolTip(text);
}



void AlgoSetting::init_UI_logic()
{
    int w = 100;
    int h = 30;

    QLabel* label = newLabel(this, "请输入管理员密码:", w, h);
    lineEdit_logic = newLineEdit(this, "", w, h);
    MyWidget* w1 = new MyWidget;
    newHBoxLayout(w1, {label, lineEdit_logic}, 0, 10);

    buttonYes = newToolButton(this, "确认设置", "", w*2, 80, "", false, false, Qt::ToolButtonTextOnly);
    connect(buttonYes, &QToolButton::clicked, this, &AlgoSetting::on_toolButton_logic_clicked);

    widget_logic = new MyWidget(this);
    newVBoxLayout(widget_logic, {w1, buttonYes}, 0, 20);
}

void AlgoSetting::on_toolButton_logic_clicked()
{

}



















