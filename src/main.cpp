#include "qsqldatabase.h"
#include "qsqlquery.h"
#include <unistd.h>
//#include <cpuid.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <QProcess>
#include <QMap>
#include <QDebug>
#include <thread>
#include <QtConcurrent>
#include <QApplication>
#include <filesystem>

#include "AlgoSetting.h"
#include "AlgoSetting.h"
#include "launch.h"
#include "verify.h"

using namespace std;

QString _cpp2Dir = "/workspace/cpp2"; //QDir::currentPath().replace("/bin", ""); //
QString _jsonsDir = _cpp2Dir + "/quec_libs/jsons"; //_cpp2Dir + "/bin/jsons"; //
QString _saveDir = _cpp2Dir + "/results";
QMap<QString, QString> _settingJsons;
bool _isShowAllLabel = true;
bool get_mainJson()
{
    QString path = _jsonsDir + "/setting.json";

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QByteArray all = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(all);
    if (doc.isNull()){ // || doc.isEmpty()){
        QMessageBox::critical(nullptr, "加载错误！", "默认参数加载失败\n请检测默认设置文件的路径：" + path);
        return false;
    }
    QJsonObject obj = doc.object();  // get root

    try {
        _settingJsons["locate_map"] = obj["locate_map"].toString();
        _settingJsons["main_exec_dir"] = obj["main_exec_dir"].toString();
        _settingJsons["model_default"] = obj["model_default"].toString();
        _settingJsons["model_jsonDir"] = _jsonsDir + "/" + _settingJsons["model_default"];
        _isShowAllLabel = obj["show_allLabels"].toInt();
        _saveTriggerMode = obj["trigger"].toString();
        _saveDays = obj["save_days"].toInt();
        _mapDir = _settingJsons["locate_map"];

        _settingJsons["save_imgsNums"] = QString::number(obj["save_imgsNums"].toInt());
        _saveImgsNums = obj["save_imgsNums"].toInt();

        qDebug() << _settingJsons;
        return true;
    }  catch (QException &e) {
        QMessageBox* msBox = newMessageBox(NULL, "错误",
                                          "加载配置文件失败，\n请检查:" + path + "\n是否包含是否包含以下字段: \n"
                                          + "locate_map, main_exec_dir, model_default, \nsave_days, saveImgsNums, trigger, show_allLabels");
        msBox->exec();

        delete msBox;
        msBox = NULL;

        return false;
    }

}

void create_saveDir_first()
{
    _settingJsons["save_main_dir"] = _saveDir;

    _settingJsons["save_model_dir"] = _saveDir + "/" + _settingJsons["model_default"];
    _settingJsons["save_date_dir"] = _settingJsons["save_model_dir"] + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd");

    _settingJsons["save_cam1_dir"] = _settingJsons["save_date_dir"] + "/相机一";
    _settingJsons["save_cam1_dir_OK"] = _settingJsons["save_cam1_dir"] + "/OK";
    _settingJsons["save_cam1_dir_NG"] = _settingJsons["save_cam1_dir"] + "/NG";

    _settingJsons["save_cam2_dir"] = _settingJsons["save_date_dir"] + "/相机二";
    _settingJsons["save_cam2_dir_OK"] = _settingJsons["save_cam2_dir"] + "/OK";
    _settingJsons["save_cam2_dir_NG"] = _settingJsons["save_cam2_dir"] + "/NG";

    _settingJsons["save_cam3_dir"] = _settingJsons["save_date_dir"] + "/相机三";
    _settingJsons["save_cam3_dir_OK"] = _settingJsons["save_cam3_dir"] + "/OK";
    _settingJsons["save_cam3_dir_NG"] = _settingJsons["save_cam3_dir"] + "/NG";

    _settingJsons["save_cam4_dir"] = _settingJsons["save_date_dir"] + "/相机四";
    _settingJsons["save_cam4_dir_OK"] = _settingJsons["save_cam4_dir"] + "/OK";
    _settingJsons["save_cam4_dir_NG"] = _settingJsons["save_cam4_dir"] + "/NG";

    _settingJsons["save_cam5_dir"] = _settingJsons["save_date_dir"] + "/相机五";
    _settingJsons["save_cam5_dir_OK"] = _settingJsons["save_cam5_dir"] + "/OK";
    _settingJsons["save_cam5_dir_NG"] = _settingJsons["save_cam5_dir"] + "/NG";

    _settingJsons["save_merge_dir"] = _settingJsons["save_date_dir"] + "/略图";
    _settingJsons["save_merge_NG_dir"] = _settingJsons["save_date_dir"] + "/略图-NG";

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

    QProcess process;
    for (QString path: dirs)
    {
        QDir dirSave(path);
        if(!dirSave.exists()){
            dirSave.mkdir(path);
        }
    }

    QString logDir = _cpp2Dir + "/log";
    QDir dirLog(logDir);
    if(!dirLog.exists()){
        dirLog.mkdir(logDir);
    }

    process.start("chmod 777 " + _settingJsons["save_date_dir"] + " " + logDir);
    if (process.waitForFinished(3000)){
        qDebug() << ">>>> 赋予权限成功: " << _settingJsons["save_date_dir"];
    }
    else{
        qDebug() << ">>>> 赋予权限失败: " << _settingJsons["save_date_dir"];
    }

}

void clearDir_10days(int delete_days)
{
    if (delete_days <= 0){
        qDebug() << ">>>> 不删除!!!";
        return;
    }
    QDate dateCur = QDate::currentDate();

    QList<QString> days_path;
    QString cmd_str = "/bin/rm -r";
    for(int i=0; i<delete_days; i++)
    {
        QDate last_day = dateCur.addDays(-5 - i);

        QString last_day_path = _settingJsons["save_model_dir"] + "/" + last_day.toString("yyyy-MM-dd") + "*";
        cmd_str += " " + last_day_path;

        days_path.append(_settingJsons["model_default"] + "/" + last_day.toString("yyyy-MM-dd") + "*");
    }
//    cmd_str += " " + _settingJsons["save_model_dir"] + "/" + QDate::currentDate().toString("yyyy-MM-dd");

    QProcess process;
    process.start(cmd_str);
    if (process.waitForFinished(3000)){
        qDebug() << ">>>> 已删除" << delete_days << "天前数据:\n    " << cmd_str;
    }
    else{
        qDebug() << ">>>> 自动删除失败！！！";
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool isLoaded = get_mainJson();
    if (!isLoaded)  return -1;

    // 验签过程
    int nflag = 0;
    verify(nflag, (_cpp2Dir + "/quec_libs").toStdString());
    if (nflag == 1){
        printf("验签失败!!! \n");
        return -1;
    }

    // 创建文件夹
    QtConcurrent::run(create_saveDir_first);

    a.setWindowIcon(QIcon(":/main/icon/m1.png"));
    //加载样式表
    QFile file(":/qss/blacksoft.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
    dbConn = QSqlDatabase::addDatabase("QSQLITE");
    qDebug() << "query database dir: " << qApp->applicationDirPath();
    dbConn.setDatabaseName(qApp->applicationDirPath() + "/TA.db");
    if (dbConn.open()) {
        printf("Success: 连接数据库成功!\n");
    } else {
        printf("Error: 连接数据库失败!\n");
    }

    query = QSqlQuery(dbConn);
    QString opeStr = "create table if not exists workpieces(SN varchar(100) not null primary key, cam1 varchar(100), cam2 varchar(100), "
             "cam3 varchar(100), cam4 varchar(100), cam5 varchar(100), result varchar(100), timeCur datetime, location varchar(100), "
             "numOK int, numNG int, numTotal int, numTotalAll int);";
    if (query.exec(opeStr)){
        printf("Success: 创建数据库表成功!\n");
    }
    else{
        printf("Error: 创建数据库表失败!\n");
    }

//    QString opeStr2 = "create table numNGTypes if not exists (SN int not null primary key, noJunYun int, pengShang int, zangWu int, maoCao int, wuGai int, aoKeng int, huaHen int);";
//    query.exec(opeStr2);

    MyMainWindow w;
    w.resize(1260, 900);
    w.setWindowTitle(" Quec轴承AI质检");
    w.setWindowIcon(QIcon(":/uimain/icon/m1.png"));
    QRect desktop = QApplication::desktop()->availableGeometry();
    w.move(desktop.width()/2 - w.width()/2, desktop.height()/2 - w.height()/2);
    w.show();

    MyWidget* widget = new MyWidget(&w);

    // 1.
    LaunchWidget* launchWidget = new LaunchWidget(&w, widget);
    newVBoxLayout(widget, {launchWidget});
    w.setCentralWidget(widget);

//    // 2.
//    frmMain* mainW = new frmMain(&w);
//    newVBoxLayout(widget, {mainW, mainW->_useWidget});
//    w.setCentralWidget(widget);

    // 自动清除文件夹中n天以前的文件
    if (_saveDays > 0){
        qDebug() << ">>>> 当前自动删除： " << _saveDays  << "天前";
        std::thread mythread(clearDir_10days, _saveDays);
        mythread.join();
    }


    return a.exec();
}


