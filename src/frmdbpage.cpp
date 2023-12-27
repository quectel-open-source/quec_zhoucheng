#pragma execution_character_set("utf-8")

#include "frmdbpage.h"
#include "ui_frmdbpage.h"
#include "dbpage.h"
#include "commonZack.h"

frmDbPage::frmDbPage(QWidget *parent) : QWidget(parent), ui(new Ui::frmDbPage)
{
    this->setParent(parent);
    _colNameList ={"SN","cam1", "cam2", "cam3", "cam4", "cam5","result","timeCur","location","numOK","numNG","numTotal","numTotalAll"};
    _colNameList_CN = {"SN号","相机一","相机二","相机三","相机四","相机五","相机结果汇总","时间","图片地址","当前OK数","当前NG数","当前检查总数","工件总数"};
    _searchNames = {"时间", "SN号", "相机一","相机二","相机三","相机四","相机五","相机结果汇总"};
    _searchNames_value = {"timeCur", "SN","cam1", "cam2", "cam3", "cam4", "cam5","result"};

    ui->setupUi(this);
    this->init_UI();
    this->initForm();
    this->setWindowFlags( Qt::WindowStaysOnTopHint | Qt::Tool);

    on_btnSelect_clicked();
    this->setGeometry(300,300, 1050,660);
}

frmDbPage::~frmDbPage()
{
    delete ui;
}

void frmDbPage::initForm()
{
    columnNames.clear();
    columnWidths.clear();

    // tableName = "new";
    tableName = "workpieces";
    countName = "SN";
    for (QString colName: _colNameList_CN)
    {
        columnNames.append(colName);
    }

    int colW = 42;
    columnWidths = {60,colW,colW,colW,colW,colW,80,180,180,60,60,80,80};

    //设置需要显示数据的表格和翻页的按钮
    dbPage = new DbPage(this);
    //设置所有列居中显示
    dbPage->setAllCenter(true);
    dbPage->setControl(ui->tableMain, ui->labPageTotal, ui->labPageCurrent, ui->labRecordsTotal, ui->labRecordsPerpage,
                       ui->labSelectTime, 0, ui->btnFirst, ui->btnPreVious, ui->btnNext, ui->btnLast, countName);
    ui->tableMain->horizontalHeader()->setStretchLastSection(true);
    ui->tableMain->verticalHeader()->setDefaultSectionSize(25);
}

void frmDbPage::init_UI()
{
    ui->lineEdit->hide();
    ui->btnSelect->setStyleSheet("QPushButton{border:2px solid rgb(119,183,244);}, QPushButton:hover{background-color:rgb(119,183,244);}");

    _comboBox = newComboBox(this, {"时间", "相机位1","相机位2","相机位3","相机位4","相机位5","相机结果汇总"}, "请选择搜索条件…", 120, 30, 0);
    connect(_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &frmDbPage::on_comboBox_changed);


    _lineEdit = newLineEdit(this, "", 0, 30, {0,0});

    _widgetEdit = new QWidget;
    _timeEdit_1 = newDateTimeEdit(this, "", 0, 30, {-365, 365}, true);
    _timeEdit_2 = newDateTimeEdit(this, "", 0, 30, {-365, 365}, true);
    QLabel* label = newLabel(this, " — ");
    newHBoxLayout(_widgetEdit, {_timeEdit_1, label, _timeEdit_2}, 8);

    _buttonSearch = newPushButton(this, "  查 找  ", "", 0, 0);
    connect(_buttonSearch, &QPushButton::clicked, this, &frmDbPage::on_buttonSearch_clicked);
    _buttonResume = newPushButton(this, "  恢 复  ", "", 0, 0);
    connect(_buttonResume, &QPushButton::clicked, this, &frmDbPage::on_btnSelect_clicked);

    newHBoxLayout(ui->widget_edit, {_comboBox, _lineEdit, _widgetEdit, _buttonSearch, _buttonResume}, 0, 8, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);
    _widgetEdit->hide();
}

void frmDbPage::on_comboBox_changed(int idx)
{
    if (idx < 1){
        return;
    }

    QString text = _comboBox->currentText();
    if (text == "时间"){
        _lineEdit->hide();
        _widgetEdit->show();
        QDateTime time = QDateTime::currentDateTime();
        _timeEdit_1->setDateTime(time);
        _timeEdit_2->setDateTime(time);
    }
    else{
        _widgetEdit->hide();
        _lineEdit->show();
    }
}

void frmDbPage::on_buttonSearch_clicked()
{
    int index = _comboBox->currentIndex();
    if (index <= 0) return;

    QString whereSql = "";
    if (index == 1){
        whereSql = QString("where %1 between %2 and %3").arg(_searchNames_value[index-1]).arg(_timeEdit_1->dateTime().toString()).arg(_timeEdit_2->dateTime().toString());
    }
    else{
        whereSql = "where " + _searchNames_value[index-1] + " like '%" + _searchNames[index-1] + "%'";
    }

    dbPage->setTableName(tableName);
    dbPage->setOrderSql(QString("%1 %2").arg(countName).arg("asc"));
    dbPage->setWhereSql(whereSql);
    dbPage->setSelectColumn("*");
    dbPage->setRecordsPerpage(20);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);

    dbPage->select();
    dbPage->first();
}

void frmDbPage::on_btnSelect_clicked()
{
    //绑定数据到表格
    QString sql = "where 1=1";
//    qDebug() << "start";
//    qDebug() << dateview->start;
//    qDebug() << "end";
//    qDebug() << dateview->end;

    //    query.exec(QString("create table new  as select   * from line1data  where date between '2022-11-17 20:19:42' and '2022-11-17 20:22:47'  "));
    //          query.exec(QString("select   * from new  where date between '2022-11-17 20:19:42' and '2022-11-17 20:22:47' "));
//    if(dateview->start.isEmpty() == true)
//    {
//        dbPage->setTableName(tableName);
//    }else{
//        query.exec(QString("create table newT  as select   * from line1data  where date between '%1' and '%2' ").arg(dateview->start).arg(dateview->end));
//        // dbPage->setTableName("newT");
        dbPage->setTableName(tableName);
//    }


    dbPage->setOrderSql(QString("%1 %2").arg(countName).arg("asc"));
    dbPage->setWhereSql(sql);
    dbPage->setRecordsPerpage(20);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);
    dbPage->select();
}
