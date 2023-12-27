#ifndef ALGOSETTING_H
#define ALGOSETTING_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include "commonZack.h"
#include "AlgoConfig.h"

////
typedef struct defectParam_DuanMian_pre{
    int max_side_circle_r = 45;//最大外环公差
    int min_side_circle_r = 15;//最小外环公差
    int max_in_circle_r = 45;//最大内环公差
    int min_in_circle_r = 15;//最小内环公差

    //thr
    int min_wid_neijing = 10; //内径宽度
    int min_light_neijing = 160;//内径明暗度
    int min_areaa_neijing = 2000;//内径面积

    int one_light = 100; //严重允许面积
    int two_light = 150;//轻微允许面积
    int thr_light = 170;//次级允许面积
    int one_area = 40;//严重黑块
    int two_area = 60;//轻微黑块
    int thr_area = 100;//次级黑块
    int expand_side = -5;//凹陷距离
}defectParam_DuanMian_pre;

typedef struct defectParam_NeiHuan_pre{
    cv::Rect image_roi = cv::Rect(200, 330, 350, 750);//裁图roi

    int distance_min = 30;//矩形最小边长
    int distance_max = 225;//矩形最大边长
    int roi_width = 80;//裁图宽度
    int roi_hight = 420;//裁图高度
}defectParam_NeiHuan_pre;

typedef struct defectParam_NeiDaoJiao_pre{
    cv::Rect image_roi = cv::Rect(80, 150, 1050, 800);//裁图roi

    int area_min = 1000;//椭圆定位最小面积
    int area_max = 15000;//椭圆定位最大面积
    int Cricle_r = 140;//裁剪半径
    int roi_width = 360;//裁剪尺寸宽度
    int roi_height = 280;//裁剪尺寸高度
}defectParam_NeiDaoJiao_pre;

typedef struct defectParam_WaiHuanMian_pre{
    cv::Rect image_roi = cv::Rect(0, 0, 1880, 850);

    int left_width = 12; //线扫导角宽度
    int right_width = 12; //线扫导角宽度
    int number = 3;//连续凹陷个数
    float width_min = 0.0;//凹陷程度
    int left_side = 74;//左边切图宽度64 74
    int right_side = 467;//右边切图宽度474 477
}defectParam_WaiHuanMian_pre;


//////
typedef struct DefectParam{
    QList<QString> chars;
    QList<QString> pre;
    QList<QString> pre_CN;
    QList<int> pre_th;
    QList<int> image_roi;
    QList<QString> labels;
    QList<QString> labels_CN;
    QList<QString> labels_all;
    QMap<QString, QList<int>> labels_params;
    bool isSaved;
}DefectParam;




class AlgoSetting: public QWidget
{
    Q_OBJECT

public:
    explicit AlgoSetting(QWidget *parent = 0);
    ~AlgoSetting();

    QComboBox* comboBox_zhouCheng;

    QComboBox* comboBox_char_type;
    QComboBox* comboBox_char_diff;
    MyWidget* _widget_char;
    QToolButton* button_charYes;

    QCheckBox* checkBox_duanMian;
    QCheckBox* checkBox_neiDaoJiao;
    QCheckBox* checkBox_neiHuan;
    QCheckBox* checkBox_waiHuan;

    MyWidget* _wROI;
    QRadioButton* checkBox_setROI;

    QList<QCheckBox*> checkBox_list;

    QLineEdit* lineEdit_score;
    QLineEdit* lineEdit_area;
    QLineEdit* lineEdit_area_min;
    QLineEdit* lineEdit_number;
    QLineEdit* lineEdit_length;
    QLineEdit* lineEdit_gray;
    QLineEdit* lineEdit_thOut;
    QLineEdit* lineEdit_thIn;

    QList<QCheckBox*> cBox_DuanMian_list;
    QList<QCheckBox*> cBox_NeiHuan_list;
    QList<QCheckBox*> cBox_neiDaoJiao_list;
    QList<QCheckBox*> cBox_WaiHuanMian_list;
    MyWidget* wLabels_DuanMian;
    MyWidget* wLabels_NeiHuan;
    MyWidget* wLabels_neiDaoJiao;
    MyWidget* wLabels_WaiHuanMian;

    MyWidget* wParam12345;

    QToolButton* buttonYes;

    MyGraphicsView* _view;

    QStringList _defectNameList;
    QStringList _defectNameList_CN;
    QList<QCheckBox*> _checkBoxList;

    QList<QJsonObject> _objs;
    DefectParam _dParams[4];
    QStringList _jsonPath;

    QString _defaultJsonDir;

    QList<QWidget *> _widgets_pre;
    QList<QLabel*> _label_NeiHuan_pre;
    QList<QLabel*> _label_DuanMian_pre;
    QList<QLabel*> _label_WaiHuanMian_pre;
    QList<QLineEdit*> _lineEdits_NeiHuan_pre;
    QList<QLineEdit*> _lineEdits_DuanMian_pre;
    QList<QLineEdit*> _lineEdits_WaiHuanMian_pre;
    QList<int> _pre_counts;

    QToolButton* button_openShow;
    MyWidget* wLeftTotal;

    MyWidget* _viewWidget;
    QTextEdit* _textEdit;
    MyWidget* _roiWidget;
    QLabel* _viewLabel;
    bool _isViewShow;
    bool _isMoving;
    QPoint _posMousePress;
    QPoint _posShow;

    void JsonSelectAndShow(const QString dirname);
protected:
    void closeEvent(QCloseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    void init_UI();
    void init_view();
    void init_UI_add();
    void init_char();

    void init_UI_pre();
    void initState_widget_pre(bool show_neiDaoJiao, bool show_DuanMian, bool show_WaiHuanMian, bool show_NeiHuan);

    void update_cBox_defectShow(QList<QString> labels, bool show1, bool show2, bool show3, bool show4);

    void initState_checkBox(int i);

    int JsonIORead(int idx, QString path, bool isFirstLoad=false);
    int JsonIOWrite(int id, QString jsonPath);

    void updateLineEdit(int idx, QList<int> params);
    void update_Pre_lineEdit(int idx, QString text);
    void update_Post_lineEdit(QString text, bool isInt=true);


signals:
    void emit_sendJson(int id);
    void emit_sendJsonType(QString dir);

    void emit_update_preParams(int camid, QList<int> pre_params);
    void emit_update_preROI(int camid, QList<int> pre_ROI);
    void emit_update_postParams(int camid, QList<QString> label_list, QMap<QString, QList<int>> labels_params);

    void emit_sendChars(QString chars1, QString chars2);

private slots:
    void on_checkBox_clicked(QCheckBox* checkBox);
    void on_checkBoxDefect_clicked(int camid, QCheckBox* checkBox);
    void on_toolButton_clicked();
    void on_comboBox_zhouCheng_changed(int idx);

    void on_button_charYes_clicked();
    void on_toolButton_zhouCheng_clicked();
    void on_toolbutton_openShow_clicked();

    void on_toolButton_open_clicked();
    void on_checkBox_setROI_clicked();

    void on_buttonSetROI_clicked();
    void view_textEdit_updated(QString text);
    void view_imageInfo_receivedd(int w, int h);


private:
    QString opeStr;
    bool _isLoaded;

    MyWidget* widget_add;
    QLineEdit* lineEdit_zchengName;
    QToolButton* buttonYesName;


    MyWidget* widget_logic;
    QLineEdit* lineEdit_logic;
    QToolButton* buttonYesLogic;

    void init_UI_logic();

private slots:
    void on_buttonYesName_clicked();
    void on_toolButton_logic_clicked();

public slots:
    void getAlgo_loadedState();
    void getCharSetting(QList<QString> keys);

};




#endif // ALGOSETTING_H
