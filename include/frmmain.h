#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QDialog>
#include <QWidget>
#include <QMap>
#include <QElapsedTimer>
#include "videopanel.h"
#include "AlgoWidget.h"

class QLabel;
class QToolButton;
class QStackedWidget;
class QString;

namespace Ui {
class frmMain;
}

class frmMain : public QDialog
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

    QLabel* labeltotal2;
    QLabel* labelng2;
    int checktotall;
    int checkng;
    int checkok;
    QList<int> _ngTypes;
    QTimer _timer;

    SystemUsedWidget* _useWidget;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
//    void changeEvent(QEvent* event); // override;

    void closeEvent(QCloseEvent* event) override;

private:
    Ui::frmMain *ui;

    VideoPanel* _videoPanel;
//    ngChart* _ngChart;
//    LineChart* _lineChart;

//    AlgoWidget* _algoWidget;
    AlgoSetting* _algoSetting;
//    AlgoConfig*  _algoConfig;

    AlgoCameras* _algoCameras;
    QPushButton* pushButton_algoCamera;

    bool _isMoving;
    QPoint _posMousePress;
    QPoint _posShow;

    ClearDataWidget* _clearWidget;
    bool _isCameraOffline;

private:
    void initForm();
    void init_UI();
    void buttonClick();

    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

    void init_algoWidget();

signals:
    void emit_loadCompleted();

    void emit_isAlgoLoaded();

    void emit_isShowSysteminfo(bool isShow);

private slots:
    void on_btnMenu_setting_clicked();
    void on_btnMenu_config_clicked();
    void on_btnMenu_refresh_clicked();
    void on_pushButton_algoCamera_clicked();
    void on_pushButton_line_clicked();

    void on_btnstart_clicked();
    void on_btnsql_clicked();
    void get_clearAndCreate(int ret_nums, int ret_imgs);

    void getui(int ok, int ng, int total, QList<int> ngTypes);
    void getCameraState(bool isOffline);
    void getInfer_initState(int initState);

    void on_btnMenu_refresh_pressed();
    void timer_timeout();
    void LineChartWidget_closed();


};

#endif // UIDEMO01_H
