#include "AlgoWidget.h"
#include <QtConcurrent>

extern QString _saveDir;

AlgoWidget::AlgoWidget(QWidget* parent):QWidget(parent)
{
    this->setParent(parent);
    QFont font;
    font.setPointSize(15);
    this->setFont(font);
    this->setWindowTitle("Quec 设置窗口");
    this->setWindowIcon(newIcon(":/uimain/icon/main_config.png", 30, 30));
    this->setWindowFlags(Qt::Tool);
    this->move(200, 200);
    this->resize(1280, 720);

    algoSetting = new AlgoSetting(this);
    algoConfig = new AlgoConfig(this);
    algoCameras = new AlgoCameras(this);

    this->init_UI();
}

void AlgoWidget::init_UI()
{
    QWidget* rightW = new QWidget(this);
    rightW->setStyleSheet("background-color:rgb(30, 41, 54)");
    newHBoxLayout(rightW, {algoSetting,  algoConfig, algoCameras});
    this->selectShowWidget(true, false, false);

    QString qss = "QToolButton{border: 1px; border-radius:5px; background-color: rgb(128,128,128);font-size: 18px; color: white; text-align;} \
            QToolButton:hover{background-color:rgb(15,117,155);} \
            QToolButton:checked{background-color:rgb(21,166,221);}";
    button_AlgoSetting = newToolButton(this, "算法参数设置", "算法参数设置窗口", 170, 100, "", true, false, Qt::ToolButtonTextBesideIcon, qss);  // 50,180,240
    button_AlgoSetting->setIconSize(QSize(30, 30));
    button_AlgoSetting->setIcon(newIcon(":/uimain/icon/main_config.png", 30, 30));

    button_AlgoConfig = newToolButton(this, "ROI设置", "轴承各个面ROI设置窗口", 170, 100, "", true, false, Qt::ToolButtonTextBesideIcon, qss);
    button_AlgoConfig->setIconSize(QSize(30, 30));
    button_AlgoConfig->setIcon(newIcon(":/uimain/icon/main_about.png", 30, 30));

    button_AlgoCameras = newToolButton(this, "相机配置", "相机配置与存图设置窗口", 170, 100, "", true, false, Qt::ToolButtonTextBesideIcon, qss);
    button_AlgoCameras->setIconSize(QSize(30, 30));
    button_AlgoCameras->setIcon(newIcon(":/uimain/icon/u20.png", 30, 30));

    connect(button_AlgoSetting, &QToolButton::clicked, this, &AlgoWidget::on_button_AlgoSetting_clicked);
    connect(button_AlgoConfig, &QToolButton::clicked, this, &AlgoWidget::on_button_AlgoConfig_clicked);
    connect(button_AlgoCameras, &QToolButton::clicked, this, &AlgoWidget::on_button_AlgoCameras_clicked);

    QWidget *itemBottom = new QWidget(this);
    itemBottom->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QWidget* leftW = new QWidget(this);
    newVBoxLayout(leftW, {button_AlgoSetting, button_AlgoConfig, button_AlgoCameras, itemBottom}, 0, 10, {2,10,2,10}, Qt::AlignTop | Qt::AlignHCenter);


    newHBoxLayout(this, {leftW, rightW});
}

void AlgoWidget::on_button_AlgoSetting_clicked()
{
    if (button_AlgoSetting->isChecked()){
        button_AlgoConfig->setChecked(false);
        button_AlgoCameras->setChecked(false);

        this->selectShowWidget(true, false, false);
    }
    else{
        button_AlgoSetting->setChecked(true);
        this->on_button_AlgoSetting_clicked();
    }
}

void AlgoWidget::on_button_AlgoConfig_clicked()
{
    if (button_AlgoConfig->isChecked()){
        button_AlgoSetting->setChecked(false);
        button_AlgoCameras->setChecked(false);

        this->selectShowWidget(false, true, false);
    }
    else{
        button_AlgoConfig->setChecked(true);
        this->on_button_AlgoConfig_clicked();
    }
}

void AlgoWidget::on_button_AlgoCameras_clicked()
{
    if (button_AlgoCameras->isChecked()){
        button_AlgoSetting->setChecked(false);
        button_AlgoConfig->setChecked(false);

        algoCameras->updateShow(_saveDir);
        this->selectShowWidget(false, false, true);
    }
    else{
        button_AlgoCameras->setChecked(true);
        this->on_button_AlgoCameras_clicked();
    }
}

void AlgoWidget::selectShowWidget(bool isShow1, bool isShow2, bool isShow3)
{
    if (isShow1)
        algoSetting->show();
    else
        algoSetting->hide();

    if (isShow2)
        algoConfig->show();
    else
        algoConfig->hide();

    if (isShow3)
        algoCameras->show();
    else
        algoCameras->hide();

}
