#include "frmmain.h"
#include "ui_frmmain.h"

#include "frmdbpage.h"
#include "DaHengCamera/GxIAPI.h"
#include "DaHengCamera/DxImageProc.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "qdebug.h"
#include <unistd.h>

#include "frmmain.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include <QMetaType>

extern QMap<QString, QString> _settingJsons;

//QXlsx::Document xlsx;
frmMain::frmMain(QWidget *parent) : QDialog(parent), ui(new Ui::frmMain)
{
    _videoPanel = NULL;
    checkng=0;
    checkok=0;
    checktotall=0;
    _ngTypes << 0 << 0 << 0 << 0 << 0 << 0 << 0;
    _isCameraOffline = false;

    ui->setupUi(this);
    this->setMouseTracking(true);
    this->initForm();
    this->init_UI();
    this->setWindowFlags(Qt::FramelessWindowHint);

    _videoPanel = new VideoPanel(this);
    connect(_videoPanel, &VideoPanel::emit_sendui, this, &frmMain::getui);
    connect(_videoPanel, &VideoPanel::emit_inferState, this, &frmMain::getInfer_initState);
    connect(_videoPanel, &VideoPanel::emit_cameraOffline, this, &frmMain::getCameraState);
    connect(this, &frmMain::emit_isShowSysteminfo, _videoPanel, &VideoPanel::get_isShowSysteminfo);
    newHBoxLayout(ui->widgetShow, {_videoPanel});


    /////////////
//    _ngChart = NULL;
//    _lineChart = NULL;
    _algoCameras = NULL;
    _clearWidget = NULL;
    _videoPanel->loadHistoryResult();
    //    _algoWidget = NULL;
    //    _algoConfig = NULL;

    _useWidget = new SystemUsedWidget(this->parentWidget());
//    connect(this, &frmMain::emit_isShowSystemInfo, _videoPanel->_modbusTcpThread, &ModbusTcpThread::calc_systemInfo);
//    connect(_videoPanel->_modbusTcpThread, &ModbusTcpThread::emit_systemInfo, _useWidget, &SystemUsedWidget::updateShow);
    connect(_videoPanel, &VideoPanel::emit_systemInfo, _useWidget, &SystemUsedWidget::updateShow);

//    qRegisterMetaType<emit_update_postParams>("emit_update_postParams");
    _algoSetting = new AlgoSetting(this->parentWidget());
    connect(_algoSetting, &AlgoSetting::emit_update_postParams, _videoPanel, &VideoPanel::update_postParams);
    connect(_algoSetting, &AlgoSetting::emit_update_preParams, _videoPanel, &VideoPanel::update_preParams);
    connect(_algoSetting, &AlgoSetting::emit_update_preROI, _videoPanel, &VideoPanel::update_preROI);
    connect(this, &frmMain::emit_isAlgoLoaded, _algoSetting, &AlgoSetting::getAlgo_loadedState);

    connect(_videoPanel, &VideoPanel::emit_initChar, _algoSetting, &AlgoSetting::getCharSetting);
    connect(_algoSetting, &AlgoSetting::emit_sendChars, _videoPanel, &VideoPanel::update_chars);
    emit _videoPanel->emit_initChar(_videoPanel->_charsInit);
}

frmMain::~frmMain()
{
    delete ui;
}


bool frmMain::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (watched == ui->widgetTitle) {
            on_btnMenu_Max_clicked();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmMain::closeEvent(QCloseEvent* event)
{
    if (_videoPanel){
        _videoPanel->start_closed();
    }
}

//void frmMain::changeEvent(QEvent* event)
//{
////    qDebug() << "111111111111111111";
//    if (event->type() == QEvent::WindowStateChange)
//    {
////        qDebug() << "222222222222222222222";
//        QWindowStateChangeEvent* stateEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
//        if (stateEvent != Q_NULLPTR){
//            if (this->windowState() == Qt::WindowMaximized){
//                ui->label_2->setMaximumSize(400, 400);
//            }
//            else if (this->windowState() == Qt::WindowNoState){
//                ui->label_2->setMaximumSize(200, 200);
//            }
//        }
//    }
//}


///////////////
void frmMain::initForm()
{
    //ui->widgetMenu->setVisible(false);
    this->setWindowTitle(" 轴承AI质检"); // "Quectel轴承AI质检");
    this->setWindowIcon(QIcon(":/uimain/icon/m1.png"));
    this->setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    connect(this, &frmMain::showMaximized, this, &frmMain::on_btnMenu_Max_clicked);

    ui->widgetTitle->installEventFilter(this);
    ui->widgetTitle->setProperty("数据查询", "title");
    // ui->widgetTitle_2_2->installEventFilter(this);
    //  ui->widgetTitle_2_2>setProperty("form", "title");
    ui->widgetTop->setProperty("nav", "top");

    ui->labTitle1->setPixmap(QPixmap(":/uimain/icon/u8.png"));
    ui->labTitle1->setScaledContents(1);
    // ui->btnMenu_Min2->setPixmap(QPixmap(":/uimain/icon/u8.png"));
    ui->labTitle1->setScaledContents(1);


    // ui->btnstart->setStyleSheet(QStringLiteral("<html><head/><body><p><img src=\"/home/q/icon/u8.png\"/>startstart</p><p><br/></p></body></html>"));
    //ui->verticalLayout_7->set("background-color:rgb(28,189,127);");
    ui->widgetTitle->setStyleSheet("background-color:rgb(2,6,13);");

    // btnNg and btntotal start
    QFont ft, ft1;
    ft.setPointSize(18);
    ft1.setPointSize(28);
    ft1.setFamily("Noto Sans CJK KR");
    ft1.setBold(true);
    ui->label_3->setFont(ft1);

    QLabel* labelng1 = newLabel(this, "NG数量", 100);
    labelng1->setFont(ft);
    labelng1->setStyleSheet("color:white");

    labelng2 = newLabel(this, "0");
    labelng2->setMinimumWidth(50);
    labelng2->setFont(ft);
    labelng2->setStyleSheet("color:white");
    QHBoxLayout* ngLayout = new QHBoxLayout(ui->btnNgValue);
    ngLayout->addWidget(labelng1);
    ngLayout->addSpacing(80);
    ngLayout->addWidget(labelng2);
    ngLayout->setAlignment(Qt::AlignCenter);
    ui->btnNgValue->setStyleSheet("background-color:rgb(68,68,68); border:0px solid write;");

    QLabel* labeltotal1 = newLabel(this, "检查总数", 100);
    labeltotal1->setFont(ft);
    labeltotal1->setStyleSheet("color:white");

    labeltotal2 = newLabel(this, "0");
    labeltotal2->setMinimumWidth(50);
    labeltotal2->setFont(ft);
    labeltotal2->setStyleSheet("color:white");
    QHBoxLayout* totalLayout = new QHBoxLayout(ui->btnTotalvalue);
    totalLayout->addWidget(labeltotal1);
    totalLayout->addSpacing(80);
    totalLayout->addWidget(labeltotal2);
//    totalLayout->addStretch();
    totalLayout->setAlignment(Qt::AlignCenter);
    ui->btnTotalvalue->setStyleSheet("background-color:rgb(68,68,68); border:0px solid write;");


    ft.setPointSize(12);
    ui->label_4->setText("相机连接中…");
    ui->label_4->setFont(ft);
//    QPixmap xjp=QPixmap(":/uimain/icon/u20.png");//("icon/a2.png");
//    QSize lbsize = ui->label_xj->size();
//    xjp.scaled(lbsize,Qt::KeepAspectRatioByExpanding);
//    ui->label_xj->setPixmap(xjp);
//    ui->label_xj->setScaledContents(true);

//    ui->gridLayout
    ft.setPointSize(30);
    ft.setBold(true);
    ui->label_2->setFont(ft);
    ui->label_2->setStyleSheet("border-image:url(:/uimain/icon/h1.png); color:green");
    ui->label_2->setText("0%");

//    ui->stackedWidget->setStyleSheet("QLabel{font:60px;}");
//    ui->widgetShow->setStyleSheet("background-color:rgb(2,6,13);");
    //单独设置指示器大小
    int addWidth = 20;
    int addHeight = 10;
    int rbtnWidth = 15;
    int ckWidth = 13;
    int scrWidth = 12;
    int borderWidth = 3;

    QStringList qss;
    qss << QString("QComboBox::drop-down,QDateEdit::drop-down,QTimeEdit::drop-down,QDateTimeEdit::drop-down{width:%1px;}").arg(addWidth);
    qss << QString("QComboBox::down-arrow,QDateEdit[calendarPopup=\"true\"]::down-arrow,QTimeEdit[calendarPopup=\"true\"]::down-arrow,"
                   "QDateTimeEdit[calendarPopup=\"true\"]::down-arrow{width:%1px;height:%1px;right:2px;}").arg(addHeight);
    qss << QString("QRadioButton::indicator{width:%1px;height:%1px;}").arg(rbtnWidth);
    qss << QString("QCheckBox::indicator,QGroupBox::indicator,QTreeWidget::indicator,QListWidget::indicator{width:%1px;height:%1px;}").arg(ckWidth);
    qss << QString("QScrollBar:horizontal{min-height:%1px;border-radius:%2px;}QScrollBar::handle:horizontal{border-radius:%2px;}"
                   "QScrollBar:vertical{min-width:%1px;border-radius:%2px;}QScrollBar::handle:vertical{border-radius:%2px;}").arg(scrWidth).arg(scrWidth / 2);
    qss << QString("QWidget#widget_top>QToolButton:pressed,QWidget#widget_top>QToolButton:hover,"
                   "QWidget#widget_top>QToolButton:checked,QWidget#widget_top>QLabel:hover{"
                   "border-width:0px 0px %1px 0px;}").arg(borderWidth);
    qss << QString("QWidget#widgetleft>QPushButton:checked,QWidget#widgetleft>QToolButton:checked,"
                   "QWidget#widgetleft>QPushButton:pressed,QWidget#widgetleft>QToolButton:pressed{"
                   "border-width:0px 0px 0px %1px;}").arg(borderWidth);
    this->setStyleSheet(qss.join(""));

//    //设置顶部导航按钮
//    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
//    foreach (QToolButton *btn, tbtns) {
//        btn->setIconSize(icoSize);
//        btn->setMinimumWidth(icoWidth);
//        btn->setCheckable(true);
//        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
//    }

}

void frmMain::init_UI()
{
    QString qssButton = "QPushButton{border:0px solid rgb(36,36,36);} "
                        "QPushButton:hover{background-color:rgb(96,96,96);} "
                        "QPushButton:checked{background-color:rgb(28,189,127);}";
    pushButton_algoCamera = newPushButton(this, "", "相机配置", 30, 30, "", qssButton);
    pushButton_algoCamera->setIconSize(QSize(18, 18));
    pushButton_algoCamera->setToolTip("相机配置");
    pushButton_algoCamera->setIcon(newIcon(":/uimain/icon/u20.png", 28, 28));
    connect(pushButton_algoCamera, &QPushButton::clicked, this, &frmMain::on_pushButton_algoCamera_clicked);

    ui->widgetTitle_2_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->labelTip->setStyleSheet("color: white; font-size:16px;");
    _timer.setSingleShot(true);
    connect(&_timer, &QTimer::timeout, ui->labelTip, &QLabel::hide);

    ui->btnMenu_refresh->setIconSize(QSize(20, 20));
    ui->btnMenu_refresh->setIcon(newIcon(":/uimain/icon/main_help.png", 30, 30));
    ui->btnMenu_refresh->setToolTip("缺陷统计图");
    ui->btnMenu_refresh->setStyleSheet(qssButton);
    connect(ui->btnMenu_refresh, &QPushButton::clicked, this, &frmMain::on_btnMenu_refresh_pressed);

    ui->pushButton_line->setIconSize(QSize(20, 20));
    ui->pushButton_line->setIcon(newIcon(":/uimain/icon/main_data.png", 30, 30));
    ui->pushButton_line->setToolTip("合格率趋势图");
    ui->pushButton_line->setStyleSheet(qssButton);
    connect(ui->pushButton_line, &QPushButton::clicked, this, &frmMain::on_pushButton_line_clicked);
    ui->pushButton_line->setCheckable(true);
    ui->pushButton_line->setChecked(false);

    ui->btnMenu_setting->setIconSize(QSize(20, 20));
    ui->btnMenu_setting->setIcon(newIcon(":/uimain/icon/main_config.png", 30, 30));
    ui->btnMenu_setting->setToolTip("算法参数设置");
    ui->btnMenu_setting->setStyleSheet(qssButton);

    ui->pushButton_algoConfig->setFixedSize(35, 35);
    ui->pushButton_algoConfig->setIconSize(QSize(20, 20));
    ui->pushButton_algoConfig->setToolTip("系统信息");
    ui->pushButton_algoConfig->setIcon(newIcon(":/uimain/icon/main_about.png", 30, 30));
    ui->pushButton_algoConfig->setStyleSheet(qssButton);
    connect(ui->pushButton_algoConfig, &QPushButton::clicked, this, &frmMain::on_btnMenu_config_clicked);
    ui->pushButton_algoConfig->setCheckable(true);
    ui->pushButton_algoConfig->setChecked(false);

    newHBoxLayout(ui->label_xj, {pushButton_algoCamera});
    ui->label_xj->setFixedSize(30, 30);

    /////////////////////
    QLabel* label = newLabel(this, "清除数据", 75, 30, "color:white; font-size: 18px;");
    label->setScaledContents(true);

    QLabel* label2 = newLabel(this, "", 20, 20, "border:0px solid write;");
    QPixmap pix(":/uimain/icon/u17.png");
    pix.scaled(label2->size(), Qt::KeepAspectRatioByExpanding);
    label2->setPixmap(pix);
    label2->setScaledContents(true);

    QHBoxLayout* myLayout = new QHBoxLayout(ui->btnstart);
    myLayout->addSpacing(1);
    myLayout->addWidget(label2);
    myLayout->addSpacing(1);
    myLayout->addWidget(label);
    myLayout->addStretch();
    myLayout->setAlignment(Qt::AlignCenter);
//    ui->btnstart->setCheckable(true);
//    ui->btnstart->setChecked(false);
    ui->btnstart->setStyleSheet("QPushButton{border: 0px; background-color: rgb(28,189,127);font-size: 18px; border-radius:5px;} \
                                QPushButton:hover{background-color:rgb(21,140,96);}");

    QLabel* labelsql1 = newLabel(this, "数据查询", 75, 30, "color:white; font-size: 18px;");
    labelsql1->setScaledContents(true);

    QLabel* labelsql2 = newLabel(this, "", 20, 20);
    QPixmap pixsql(":/uimain/icon/u20.png");
    pixsql.scaled(labelsql2->size(),Qt::KeepAspectRatioByExpanding);
    labelsql2->setPixmap(pixsql);
    labelsql2->setScaledContents(true);

    QHBoxLayout* sqlLayout = new QHBoxLayout(ui->btnsql);
    sqlLayout->addSpacing(1);
    sqlLayout->addWidget(labelsql2);
    sqlLayout->addSpacing(1);
    sqlLayout->addWidget(labelsql1);
    sqlLayout->addStretch();
    sqlLayout->setAlignment(Qt::AlignCenter);
    ui->btnsql->setStyleSheet("QPushButton{border: 0px; background-color: rgb(248,173,23);font-size: 18px; border-radius:5px;} \
                              QPushButton:hover{background-color:rgb(213,146,6);}");

    ////////////////
//    MyWidget* wBottom = new MyWidget(this);
//    wBottom->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//    wBottom->setFixedSize(250, 40);
//    QLabel* lbel = newLabel(this, "123456", -1, 30);
//    newVBoxLayout(wBottom, {lbel}, 0, 0, {0,0,0,0});
//    ui->verticalLayout->addWidget(lbel);
}


//////
void frmMain::getui(int ok, int ng, int total, QList<int> ngTypes)
{
    checkok = ok;
    checkng = ng;
    checktotall = total;
    for(int i=0; i< ngTypes.count(); i++){
        _ngTypes[i] = ngTypes[i];
    }

    double tmpvalue = 0;
    if (checktotall != 0){
        tmpvalue=checkok *100.0 / (float)checktotall;
    }

    ui->label_2->setText(QString::number(tmpvalue, 'f', 1).append("%"));
    labeltotal2->setText(QString::number(checktotall));
    labelng2->setText(QString::number(checkng));

//    if (_lineChart){
//        _lineChart->addData(checkok, checktotall);
//    }
}

////
void frmMain::getCameraState(bool isOffline)
{
    _isCameraOffline = isOffline;
    if (isOffline){
        if (ui->label_4->text() != "相机掉线"){
            ui->label_4->setText("相机掉线");
            ui->label_4->setStyleSheet("border: 0px; color: red;");
            pushButton_algoCamera->setIcon(newIcon(":/uimain/icon/a3.png", 28, 28));
        }
    }
    else{
        if (ui->label_4->text() != "相机连接正常"){
            ui->label_4->setText("相机连接正常");
            ui->label_4->setStyleSheet("border: 0px; color: rgb(28,189,127);");
            pushButton_algoCamera->setIcon(newIcon(":/uimain/icon/u20.png", 28, 28));
        }
    }
}

void frmMain::getInfer_initState(int initState)
{
    this->init_algoWidget();

    if (initState < 0){
        ui->labelTip->setText("Quec算法：初始化中… 请稍候……");
    }
    else if (initState == 0){
        ui->labelTip->setText("Quec算法：初始化失败…");
    }
    else{
        ui->labelTip->setText("Quec算法：初始化完毕！！！");
        emit emit_isAlgoLoaded();

        if (_algoSetting){
            _algoSetting->JsonSelectAndShow(_settingJsons["model_default"]);
        }
//        if (!_lineChart){
//            _lineChart = new LineChart(this->parentWidget());
//            connect(_lineChart, &LineChart::emit_closed, this, &frmMain::LineChartWidget_closed);
//        }
        if (!_algoCameras){
            _algoCameras = new AlgoCameras(this->parentWidget());
            connect(_videoPanel, &VideoPanel::emit_updateCamera, _algoCameras, &AlgoCameras::update_cameras);
        }

        this->getCameraState(_isCameraOffline);
        emit emit_loadCompleted();

        _timer.start(2000);
    }
}


void frmMain::buttonClick()
{
//    QToolButton *b = (QToolButton *)sender();
//    QString name = b->text();

//    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
//    foreach (QToolButton *btn, tbtns) {
//        btn->setChecked(btn == b);
//    }

//    if (name == "主界面") {
//        ui->stackedWidget->addWidget(_videoPanel);
//        ui->stackedWidget->setCurrentWidget(_videoPanel);
//    } else if (name == "系统设置") {
//        ui->stackedWidget->setCurrentIndex(1);
//    } else if (name == "数据查询") {
//        frmDbPage *qw = new frmDbPage;
//        // Ui::QWidget ui;
//        ui->stackedWidget->addWidget(qw);
//        //ui->stackedWidget->insertWidget(w);
//        ui->stackedWidget->setCurrentWidget(qw);
//        // ui->stackedWidget->setCurrentIndex(2);
//    } else if (name == "调试帮助") {
//        ui->stackedWidget->setCurrentIndex(3);
//    } else if (name == "用户退出") {
//        exit(0);
//    }
}

void frmMain::on_btnMenu_Min_clicked()
{
    showMinimized();
}

void frmMain::on_btnMenu_Max_clicked()
{
    static bool max = false;

    if (max) {
        this->showMaximized();
//        ui->label_2->setFixedSize(400, 400);
    } else {
        this->showNormal();
//        ui->label_2->setFixedSize(200, 200);
    }

    this->setProperty("canMove", max);
    max = !max;
}

void frmMain::on_btnMenu_Close_clicked()
{
    close();
}

void frmMain::on_btnsql_clicked()
{
    frmDbPage *qw = new frmDbPage(this);
    qw->show();

}


void frmMain::on_btnstart_clicked()
{
    if (_clearWidget == NULL){
        _clearWidget = new ClearDataWidget(this);
        connect(_clearWidget, &ClearDataWidget::emit_clearAndCreate, this, &frmMain::get_clearAndCreate);
    }
    _clearWidget->init_state();
    _clearWidget->show();


}
void frmMain::get_clearAndCreate(int ret_nums, int ret_imgs)
{
//    QThread::sleep(10);
    QString text;
    if (ret_nums == 0){
        QString sqlStr_1 = "drop table workpieces";
        if (query.exec(sqlStr_1)){
            QString opeStr = "create table if not exists workpieces(SN varchar(100) not null primary key, cam1 varchar(100), cam2 varchar(100), "
                             "cam3 varchar(100), cam4 varchar(100), cam5 varchar(100), result varchar(100), timeCur datetime, location varchar(100), "
                             "numOK int, numNG int, numTotal int, numTotalAll int);";
            query.exec(opeStr);
        }

        getui(0, 0, 0, {});
        _videoPanel->init_nums();

        text = "清除计数成功！ 若非刚启动状态，请复位PLC！";
    }
    if (ret_imgs == 0){
        QProcess process;
        process.start("/bin/rm -r /workspace/cpp2/results/608z/" + QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        process.waitForFinished();

        QList<QString> dirs;
        dirs.append(_settingJsons["save_main_dir"]);
        dirs.append(_settingJsons["save_model_dir"]);
        dirs.append(_settingJsons["save_date_dir"]);

        dirs.append(_settingJsons["save_cam1_dir"]);
        dirs.append(_settingJsons["save_cam1_dir_OK"]);
        dirs.append(_settingJsons["save_cam1_dir_NG"]);

        dirs.append(_settingJsons["save_cam2_dir"]);
        dirs.append(_settingJsons["save_cam2_dir_OK"]);
        dirs.append(_settingJsons["save_cam2_dir_NG"]);

        dirs.append(_settingJsons["save_cam3_dir"]);
        dirs.append(_settingJsons["save_cam3_dir_OK"]);
        dirs.append(_settingJsons["save_cam3_dir_NG"]);

        dirs.append(_settingJsons["save_cam4_dir"]);
        dirs.append(_settingJsons["save_cam4_dir_OK"]);
        dirs.append(_settingJsons["save_cam4_dir_NG"]);

        dirs.append(_settingJsons["save_cam5_dir"]);
        dirs.append(_settingJsons["save_cam5_dir_OK"]);
        dirs.append(_settingJsons["save_cam5_dir_NG"]);

        dirs.append(_settingJsons["save_merge_dir"]);
        dirs.append(_settingJsons["save_merge_NG_dir"]);

        for (QString path: dirs)
        {
            QDir dirSave(path);
            if(!dirSave.exists()){
                dirSave.mkdir(path);
            }
        }

        process.start("chmod 777 " + _settingJsons["save_date_dir"]);
        if (process.waitForFinished(3000)){
            qDebug() << ">>>> 赋予权限成功: " << _settingJsons["save_date_dir"];
        }
        else{
            qDebug() << ">>>> 赋予权限失败: " << _settingJsons["save_date_dir"];
        }
        text += "\n清除存图成功！";
    }

    if(_clearWidget){
        _clearWidget->close();
    }

    QMessageBox* mgBox_2 = newMessageBox(this, "提示", text);
    mgBox_2->exec();
}



void frmMain::init_algoWidget()
{
//    if(!_algoWidget){
//        _algoWidget = new AlgoWidget(this);
//        connect(_videoPanel, &VideoPanel::emit_updateCamera, _algoWidget->algoCameras, &AlgoCameras::update_cameras);
//        connect(_algoWidget->algoSetting, &AlgoSetting::emit_update_postParams, _videoPanel, &VideoPanel::update_postParams);
//        connect(_algoWidget->algoSetting, &AlgoSetting::emit_update_preParams, _videoPanel, &VideoPanel::update_preParams);
//        _videoPanel->updateCameraState();
//    }
}

///////////////////  system resource
void frmMain::on_btnMenu_config_clicked()
{
//    if (!_algoConfig){
//        _algoConfig = new AlgoConfig(this);
//    }

//    _algoConfig->show();
    if (ui->pushButton_algoConfig->isChecked()){
        _posShow = this->mapToGlobal(this->rect().bottomLeft());
        _useWidget->setGeometry(_posShow.x(), _posShow.y(), this->width(), 36);

        _useWidget->show();

        emit emit_isShowSysteminfo(true);
    }
    else{
        _useWidget->close();
        emit emit_isShowSysteminfo(false);
    }

}


///////////////////
void frmMain::on_btnMenu_setting_clicked()
{
//    this->init_algoWidget();

//    _algoWidget->on_button_AlgoSetting_clicked();
//    _algoWidget->show();

    _algoSetting->setFixedSize(460, 920);
    _algoSetting->show();

}


///////////////////
void frmMain::on_pushButton_algoCamera_clicked()
{
//    this->init_algoWidget();

//    _algoWidget->on_button_AlgoCameras_clicked();
//    _algoWidget->show();
    if (!_algoCameras){
        _algoCameras = new AlgoCameras(this->parentWidget());
        connect(_videoPanel, &VideoPanel::emit_updateCamera, _algoCameras, &AlgoCameras::update_cameras);
    }
    _algoCameras->updateShow(_settingJsons["save_date_dir"]);
    _videoPanel->updateCameraState();

    _algoCameras->show();
}


//////////////////
void frmMain::on_pushButton_line_clicked()
{
    if (ui->pushButton_line->isChecked()){
//        if (!_lineChart){
//            _lineChart = new LineChart(this->parentWidget());
//            connect(_lineChart, &LineChart::emit_closed, this, &frmMain::LineChartWidget_closed);
//        }
////        _lineChart->addData(checkok, checktotall);

//        _lineChart->move(150, 250);
//        _lineChart->show();

//        _lineChart->clearView();

//        checkok = 0;
//        checktotall = 0;

//        QTimer* timer = new QTimer(this);
//        connect(timer, &QTimer::timeout, this, &frmMain::timer_timeout);
//        timer->start(2000);
    }
    else{
//        _lineChart->close();
    }
}
void frmMain::LineChartWidget_closed()
{
    ui->pushButton_line->setChecked(false);
}
void frmMain::timer_timeout(){
    int tag = QRandomGenerator::global()->bounded(2);
    if (tag == 1)
        checkok++;
    checktotall++;
//    _lineChart->addData(checkok, checktotall);
}

void frmMain::on_btnMenu_refresh_clicked()
{
//    checkng=0;
//    checkok=0;
//    checktotall=0;
//    labeltotal2->setText("0");
//    labelng2->setText("0");
//    ui->label_2->setText("0");

}

/////////////
void frmMain::on_btnMenu_refresh_pressed()
{
//    if (!_ngChart){
//        _ngChart = new ngChart(this);
//    }

//    for(int i=0; i<_ngTypes.count(); i++){
//        _ngTypes[i] = 0;//(i + 1) * 10;
//    }
//    _ngChart->addData(_ngTypes);

//    if (!_ngChart->isVisible()){
//        _ngChart->move(200, 150);
//        _ngChart->close();
//    }
//    _ngChart->show();

}
