/********************************************************************************
** Form generated from reading UI file 'frmmain.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMMAIN_H
#define UI_FRMMAIN_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmMain
{
public:
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_6;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labTitle1;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer_7;
    QWidget *widgetTop;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelTip;
    QSpacerItem *horizontalSpacer;
    QLabel *labelRSpacer;
    QWidget *widgetMenu;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QWidget *widgetShow;
    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle_2_2;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_xj;
    QLabel *label_4;
    QWidget *widgetTop1;
    QHBoxLayout *horizontalLayout_31;
    QSpacerItem *horizontalSpacer1;
    QWidget *widgetMenu1;
    QGridLayout *gridLayout_21;
    QPushButton *btnMenu_refresh;
    QPushButton *btnMenu_setting;
    QPushButton *pushButton_algoConfig;
    QPushButton *pushButton_line;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_3;
    QWidget *widgetTitle_2_3;
    QHBoxLayout *horizontalLayout_22;
    QWidget *widgetMenu_2;
    QGridLayout *gridLayout_22;
    QLabel *btnTotalvalue;
    QSpacerItem *verticalSpacer1;
    QLabel *btnNgValue;
    QWidget *widgetTitle_2_21;
    QHBoxLayout *horizontalLayout_23;
    QHBoxLayout *horizontalLayout_23_bottom;
    QWidget *widgetTop2;
    QHBoxLayout *horizontalLayout_32;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btnstart;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widgetMenu2;
    QGridLayout *gridLayout_23;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnsql;

    void setupUi(QWidget *frmMain)
    {
        if (frmMain->objectName().isEmpty())
            frmMain->setObjectName(QString::fromUtf8("frmMain"));
        frmMain->resize(1013, 514);
        horizontalLayout_4 = new QHBoxLayout(frmMain);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 3, 0);
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        widgetTitle = new QWidget(frmMain);
        widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
        widgetTitle->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(widgetTitle);
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(10, 0, 0, 0);
        labTitle1 = new QLabel(widgetTitle);
        labTitle1->setObjectName(QString::fromUtf8("labTitle1"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labTitle1->sizePolicy().hasHeightForWidth());
        labTitle1->setSizePolicy(sizePolicy1);
        labTitle1->setMaximumSize(QSize(120, 40));
        labTitle1->setContextMenuPolicy(Qt::NoContextMenu);
        labTitle1->setToolTipDuration(0);
        labTitle1->setLayoutDirection(Qt::LeftToRight);
        labTitle1->setStyleSheet(QString::fromUtf8(""));
        labTitle1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(labTitle1);

        verticalSpacer_3 = new QSpacerItem(20, 60, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout_2->addItem(verticalSpacer_3);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_7);

        widgetTop = new QWidget(widgetTitle);
        widgetTop->setObjectName(QString::fromUtf8("widgetTop"));
        horizontalLayout_3 = new QHBoxLayout(widgetTop);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);

        horizontalLayout_2->addWidget(widgetTop);

        labelTip = new QLabel(widgetTitle);
        labelTip->setObjectName(QString::fromUtf8("labelTip"));

        horizontalLayout_2->addWidget(labelTip);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        labelRSpacer = new QLabel(widgetTitle);
        labelRSpacer->setObjectName(QString::fromUtf8("labelRSpacer"));

        horizontalLayout_2->addWidget(labelRSpacer);

        widgetMenu = new QWidget(widgetTitle);
        widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
        gridLayout_2 = new QGridLayout(widgetMenu);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 0, 0, 1, 2);


        horizontalLayout_2->addWidget(widgetMenu);


        verticalLayout_6->addWidget(widgetTitle);

        widgetShow = new QWidget(frmMain);
        widgetShow->setObjectName(QString::fromUtf8("widgetShow"));

        verticalLayout_6->addWidget(widgetShow);


        horizontalLayout_4->addLayout(verticalLayout_6);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 1, -1, -1);
        widgetTitle_2_2 = new QWidget(frmMain);
        widgetTitle_2_2->setObjectName(QString::fromUtf8("widgetTitle_2_2"));
        sizePolicy.setHeightForWidth(widgetTitle_2_2->sizePolicy().hasHeightForWidth());
        widgetTitle_2_2->setSizePolicy(sizePolicy);
        horizontalLayout_21 = new QHBoxLayout(widgetTitle_2_2);
        horizontalLayout_21->setSpacing(10);
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        horizontalLayout_21->setContentsMargins(10, 0, 0, 0);
        label_xj = new QLabel(widgetTitle_2_2);
        label_xj->setObjectName(QString::fromUtf8("label_xj"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_xj->sizePolicy().hasHeightForWidth());
        label_xj->setSizePolicy(sizePolicy2);
        label_xj->setMinimumSize(QSize(15, 15));
        label_xj->setMaximumSize(QSize(15, 15));
        label_xj->setStyleSheet(QString::fromUtf8("image: url(:/qss/blacksoft/radiobutton_unchecked_disable.png);"));

        horizontalLayout_21->addWidget(label_xj);

        label_4 = new QLabel(widgetTitle_2_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMouseTracking(true);
        label_4->setTextFormat(Qt::RichText);
        label_4->setTextInteractionFlags(Qt::NoTextInteraction);

        horizontalLayout_21->addWidget(label_4);

        widgetTop1 = new QWidget(widgetTitle_2_2);
        widgetTop1->setObjectName(QString::fromUtf8("widgetTop1"));
        horizontalLayout_31 = new QHBoxLayout(widgetTop1);
        horizontalLayout_31->setSpacing(0);
        horizontalLayout_31->setObjectName(QString::fromUtf8("horizontalLayout_31"));
        horizontalLayout_31->setContentsMargins(0, 0, 0, 0);

        horizontalLayout_21->addWidget(widgetTop1);

        horizontalSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer1);

        widgetMenu1 = new QWidget(widgetTitle_2_2);
        widgetMenu1->setObjectName(QString::fromUtf8("widgetMenu1"));
        gridLayout_21 = new QGridLayout(widgetMenu1);
        gridLayout_21->setSpacing(0);
        gridLayout_21->setObjectName(QString::fromUtf8("gridLayout_21"));
        gridLayout_21->setContentsMargins(0, 0, 0, 0);
        btnMenu_refresh = new QPushButton(widgetMenu1);
        btnMenu_refresh->setObjectName(QString::fromUtf8("btnMenu_refresh"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnMenu_refresh->sizePolicy().hasHeightForWidth());
        btnMenu_refresh->setSizePolicy(sizePolicy3);
        btnMenu_refresh->setMinimumSize(QSize(30, 30));
        btnMenu_refresh->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8("icon/u22.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnMenu_refresh->setIcon(icon);


        btnMenu_setting = new QPushButton(widgetMenu1);
        btnMenu_setting->setObjectName(QString::fromUtf8("btnMenu_setting"));
        sizePolicy3.setHeightForWidth(btnMenu_setting->sizePolicy().hasHeightForWidth());
        btnMenu_setting->setSizePolicy(sizePolicy3);
        btnMenu_setting->setMinimumSize(QSize(30, 30));
        btnMenu_setting->setCursor(QCursor(Qt::ArrowCursor));
        btnMenu_setting->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("icon/u21.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnMenu_setting->setIcon(icon1);


        pushButton_algoConfig = new QPushButton(widgetMenu1);
        pushButton_algoConfig->setObjectName(QString::fromUtf8("pushButton_algoConfig"));
        pushButton_algoConfig->setMinimumSize(QSize(30, 30));
        pushButton_algoConfig->setMaximumSize(QSize(30, 30));


        pushButton_line = new QPushButton(widgetMenu1);
        pushButton_line->setObjectName(QString::fromUtf8("pushButton_algoConfig"));
        pushButton_line->setMinimumSize(QSize(30, 30));
        pushButton_line->setMaximumSize(QSize(30, 30));

        gridLayout_21->addWidget(btnMenu_setting, 0, 1, 1, 1);
        gridLayout_21->addWidget(btnMenu_refresh, 0, 2, 1, 1);
        gridLayout_21->addWidget(pushButton_line, 0, 3, 1, 1);
        gridLayout_21->addWidget(pushButton_algoConfig, 0, 4, 1, 1);


        horizontalLayout_21->addWidget(widgetMenu1);


        verticalLayout->addWidget(widgetTitle_2_2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(frmMain);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(100);
        sizePolicy4.setVerticalStretch(100);
        sizePolicy4.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy4);
        label_2->setMaximumSize(QSize(200, 200));
        label_2->setSizeIncrement(QSize(0, 0));
        label_2->setBaseSize(QSize(0, 0));
        label_2->setLayoutDirection(Qt::LeftToRight);
        label_2->setAutoFillBackground(false);
        label_2->setStyleSheet(QString::fromUtf8("background-image: url(icon/h1.png);"));
        label_2->setFrameShape(QFrame::NoFrame);
        label_2->setFrameShadow(QFrame::Sunken);
        label_2->setLineWidth(1);
        label_2->setMidLineWidth(0);
        label_2->setAlignment(Qt::AlignCenter);
        label_2->setMargin(15);
        label_2->setIndent(1);
        label_2->setTextInteractionFlags(Qt::NoTextInteraction);

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 0, 2, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_6, 0, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        label_3 = new QLabel(frmMain);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMaximumSize(QSize(16777215, 80));
        QFont font;
        font.setFamily(QString::fromUtf8("Noto Sans CJK KR"));
        font.setPointSize(22);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);
        label_3->setTextFormat(Qt::RichText);
        label_3->setScaledContents(true);
        label_3->setAlignment(Qt::AlignCenter);
        label_3->setMargin(18);
        label_3->setIndent(0);
        label_3->setTextInteractionFlags(Qt::NoTextInteraction);

        verticalLayout->addWidget(label_3);

        widgetTitle_2_3 = new QWidget(frmMain);
        widgetTitle_2_3->setObjectName(QString::fromUtf8("widgetTitle_2_3"));
        sizePolicy.setHeightForWidth(widgetTitle_2_3->sizePolicy().hasHeightForWidth());
        widgetTitle_2_3->setSizePolicy(sizePolicy);
        horizontalLayout_22 = new QHBoxLayout(widgetTitle_2_3);
        horizontalLayout_22->setSpacing(10);
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        horizontalLayout_22->setContentsMargins(10, 0, 0, 0);
        widgetMenu_2 = new QWidget(widgetTitle_2_3);
        widgetMenu_2->setObjectName(QString::fromUtf8("widgetMenu_2"));
        gridLayout_22 = new QGridLayout(widgetMenu_2);
        gridLayout_22->setSpacing(0);
        gridLayout_22->setObjectName(QString::fromUtf8("gridLayout_22"));
        gridLayout_22->setContentsMargins(10, 0, 10, 0);
//        gridLayout_22->setAlignment(Qt::AlignCenter);
        btnTotalvalue = new QLabel(widgetMenu_2);
        btnTotalvalue->setObjectName(QString::fromUtf8("btnTotalvalue"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(btnTotalvalue->sizePolicy().hasHeightForWidth());
        btnTotalvalue->setSizePolicy(sizePolicy5);
        btnTotalvalue->setMinimumSize(QSize(28, 60));
        btnTotalvalue->setCursor(QCursor(Qt::ArrowCursor));
        btnTotalvalue->setFocusPolicy(Qt::NoFocus);

        gridLayout_22->addWidget(btnTotalvalue, 2, 0, 1, 1);

        verticalSpacer1 = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_22->addItem(verticalSpacer1, 1, 0, 1, 1);

        btnNgValue = new QLabel(widgetMenu_2);
        btnNgValue->setObjectName(QString::fromUtf8("btnNgValue"));
        sizePolicy3.setHeightForWidth(btnNgValue->sizePolicy().hasHeightForWidth());
        btnNgValue->setSizePolicy(sizePolicy3);
        btnNgValue->setMinimumSize(QSize(30, 60));
        btnNgValue->setFocusPolicy(Qt::NoFocus);
//#ifndef QT_NO_TOOLTIP
//        btnNgValue->setToolTip(QString::fromUtf8(""));
//#endif // QT_NO_TOOLTIP
//#ifndef QT_NO_STATUSTIP
//        btnNgValue->setStatusTip(QString::fromUtf8(""));
//#endif // QT_NO_STATUSTIP
//#ifndef QT_NO_WHATSTHIS
//        btnNgValue->setWhatsThis(QString::fromUtf8(""));
//#endif // QT_NO_WHATSTHIS
//#ifndef QT_NO_ACCESSIBILITY
//        btnNgValue->setAccessibleName(QString::fromUtf8(""));
//#endif // QT_NO_ACCESSIBILITY

        gridLayout_22->addWidget(btnNgValue, 0, 0, 1, 1);


        horizontalLayout_22->addWidget(widgetMenu_2);


        verticalLayout->addWidget(widgetTitle_2_3);

        widgetTitle_2_21 = new QWidget(frmMain);
        widgetTitle_2_21->setObjectName(QString::fromUtf8("widgetTitle_2_21"));
        widgetTitle_2_21->setEnabled(true);
        sizePolicy.setHeightForWidth(widgetTitle_2_21->sizePolicy().hasHeightForWidth());
        widgetTitle_2_21->setSizePolicy(sizePolicy);
        widgetTitle_2_21->setMinimumSize(QSize(60, 60));
        horizontalLayout_23 = new QHBoxLayout(widgetTitle_2_21);
        horizontalLayout_23->setSpacing(10);
        horizontalLayout_23->setObjectName(QString::fromUtf8("horizontalLayout_23"));
        horizontalLayout_23->setContentsMargins(10, 0, 0, 0);
        widgetTop2 = new QWidget(widgetTitle_2_21);
        widgetTop2->setObjectName(QString::fromUtf8("widgetTop2"));
        widgetTop2->setMinimumSize(QSize(100, 120));
        horizontalLayout_32 = new QHBoxLayout(widgetTop2);
        horizontalLayout_32->setSpacing(0);
        horizontalLayout_32->setObjectName(QString::fromUtf8("horizontalLayout_32"));
        horizontalLayout_32->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_4 = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_32->addItem(horizontalSpacer_4);

        btnstart = new QPushButton(widgetTop2);
        btnstart->setObjectName(QString::fromUtf8("btnstart"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(50);
        sizePolicy6.setVerticalStretch(50);
        sizePolicy6.setHeightForWidth(btnstart->sizePolicy().hasHeightForWidth());
        btnstart->setSizePolicy(sizePolicy6);
        btnstart->setMinimumSize(QSize(100, 50));
        btnstart->setMaximumSize(QSize(150, 50));
        btnstart->setCursor(QCursor(Qt::ArrowCursor));
        btnstart->setFocusPolicy(Qt::NoFocus);
        btnstart->setStyleSheet(QString::fromUtf8(""));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/font/iconfont.ttf"), QSize(), QIcon::Normal, QIcon::Off);
        btnstart->setIcon(icon2);

        horizontalLayout_32->addWidget(btnstart);


        horizontalLayout_23->addWidget(widgetTop2);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_23->addItem(horizontalSpacer_3);

        widgetMenu2 = new QWidget(widgetTitle_2_21);
        widgetMenu2->setObjectName(QString::fromUtf8("widgetMenu2"));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(widgetMenu2->sizePolicy().hasHeightForWidth());
        widgetMenu2->setSizePolicy(sizePolicy7);
        gridLayout_23 = new QGridLayout(widgetMenu2);
        gridLayout_23->setSpacing(0);
        gridLayout_23->setObjectName(QString::fromUtf8("gridLayout_23"));
        gridLayout_23->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_23->addItem(horizontalSpacer_2, 0, 3, 1, 1);

        btnsql = new QPushButton(widgetMenu2);
        btnsql->setObjectName(QString::fromUtf8("btnsql"));
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy8.setHorizontalStretch(50);
        sizePolicy8.setVerticalStretch(50);
        sizePolicy8.setHeightForWidth(btnsql->sizePolicy().hasHeightForWidth());
        btnsql->setSizePolicy(sizePolicy8);
        btnsql->setMinimumSize(QSize(100, 50));
        btnsql->setMaximumSize(QSize(150, 50));
        btnsql->setBaseSize(QSize(50, 50));
        btnsql->setFocusPolicy(Qt::NoFocus);
        btnsql->setLayoutDirection(Qt::LeftToRight);
        btnsql->setIcon(icon);

        gridLayout_23->addWidget(btnsql, 0, 2, 1, 1);


        horizontalLayout_23->addWidget(widgetMenu2);


        verticalLayout->addWidget(widgetTitle_2_21);


        horizontalLayout_4->addLayout(verticalLayout);

        horizontalLayout_4->setStretch(0, 3);
        horizontalLayout_4->setStretch(1, 1);

        retranslateUi(frmMain);

        QMetaObject::connectSlotsByName(frmMain);
    } // setupUi

    void retranslateUi(QWidget *frmMain)
    {
        frmMain->setWindowTitle("Quectel");
        labTitle1->setText(QString());
        labelTip->setText(QString());
        labelRSpacer->setText(QString());
        label_xj->setText(QString());
        label_4->setText(QString());
        btnMenu_refresh->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnMenu_setting->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        btnMenu_setting->setText(QString());
        pushButton_algoConfig->setText(QString());
        label_2->setText(QString());
        label_3->setText(QString());
//        label_3->setText(QApplication::translate("frmMain", "<html><head/><body><p><span style=\" color:#81d3f8; vertical-align:super;\">\350\275\264\346\211\277\345\244\226\350\247\202\347\274\272\351\231\267\346\243\200\346\265\213\347\263\273\347\273\237 </span></p></body></html>", nullptr));
#ifndef QT_NO_TOOLTIP
        btnTotalvalue->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        btnTotalvalue->setText(QString());
        btnNgValue->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnstart->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        btnstart->setText(QString());
        btnsql->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class frmMain: public Ui_frmMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMMAIN_H
