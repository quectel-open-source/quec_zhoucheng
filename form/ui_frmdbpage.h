/********************************************************************************
** Form generated from reading UI file 'frmdbpage.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMDBPAGE_H
#define UI_FRMDBPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmDbPage
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QWidget* widget_edit;
    QSpacerItem *horizontalSpacer;
    QLineEdit *lineEdit;
    QPushButton *btnSelect;
    QSplitter *splitter;
    QTableView *tableMain;
    QFrame *frameBottom;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *verticalSpacer;
    QPushButton *btnLast;
    QPushButton *btnFirst;
    QPushButton *btnNext;
    QPushButton *btnPreVious;
    QLabel *labRecordsTotal;
    QLabel *labRecordsPerpage;
    QLabel *labPageCurrent;
    QLabel *labPageTotal;
    QLabel *labSelectTime;

    void setupUi(QWidget *frmDbPage)
    {
        if (frmDbPage->objectName().isEmpty())
            frmDbPage->setObjectName(QString::fromUtf8("frmDbPage"));
        frmDbPage->resize(800, 600);
        verticalLayout = new QVBoxLayout(frmDbPage);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        widget_edit = new QWidget(frmDbPage);
        horizontalLayout->addWidget(widget_edit);

        horizontalSpacer = new QSpacerItem(1, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lineEdit = new QLineEdit(frmDbPage);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMaximumSize(QSize(100, 16777215));
        lineEdit->setStyleSheet(QString::fromUtf8("border-color: rgb(238, 238, 236);\n"
"border-color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(lineEdit);

        btnSelect = new QPushButton(frmDbPage);
        btnSelect->setObjectName(QString::fromUtf8("btnSelect"));
        btnSelect->setMinimumSize(QSize(80, 0));
        btnSelect->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(btnSelect);


        verticalLayout->addLayout(horizontalLayout);

        splitter = new QSplitter(frmDbPage);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        splitter->setOpaqueResize(false);
        tableMain = new QTableView(splitter);
        tableMain->setObjectName(QString::fromUtf8("tableMain"));
        tableMain->setStyleSheet(QString::fromUtf8(""));
        splitter->addWidget(tableMain);

        verticalLayout->addWidget(splitter);

        frameBottom = new QFrame(frmDbPage);
        frameBottom->setObjectName(QString::fromUtf8("frameBottom"));
        frameBottom->setMinimumSize(QSize(190, 0));
        frameBottom->setMaximumSize(QSize(10000, 50));
        frameBottom->setFrameShape(QFrame::Box);
        frameBottom->setFrameShadow(QFrame::Sunken);
        horizontalLayout_3 = new QHBoxLayout(frameBottom);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(6, 6, 6, 6);
        verticalSpacer = new QSpacerItem(20, 9, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout_3->addItem(verticalSpacer);

        btnLast = new QPushButton(frameBottom);
        btnLast->setObjectName(QString::fromUtf8("btnLast"));
        btnLast->setMinimumSize(QSize(50, 0));
        btnLast->setIconSize(QSize(18, 18));

        horizontalLayout_3->addWidget(btnLast);

        btnFirst = new QPushButton(frameBottom);
        btnFirst->setObjectName(QString::fromUtf8("btnFirst"));
        btnFirst->setMinimumSize(QSize(50, 0));
        btnFirst->setIconSize(QSize(18, 18));

        horizontalLayout_3->addWidget(btnFirst);

        btnNext = new QPushButton(frameBottom);
        btnNext->setObjectName(QString::fromUtf8("btnNext"));
        btnNext->setMinimumSize(QSize(50, 0));
        btnNext->setIconSize(QSize(18, 18));

        horizontalLayout_3->addWidget(btnNext);

        btnPreVious = new QPushButton(frameBottom);
        btnPreVious->setObjectName(QString::fromUtf8("btnPreVious"));
        btnPreVious->setMinimumSize(QSize(50, 0));
        btnPreVious->setIconSize(QSize(18, 18));

        horizontalLayout_3->addWidget(btnPreVious);

        labRecordsTotal = new QLabel(frameBottom);
        labRecordsTotal->setObjectName(QString::fromUtf8("labRecordsTotal"));
        labRecordsTotal->setMinimumSize(QSize(0, 9));
        labRecordsTotal->setFrameShape(QFrame::Box);
        labRecordsTotal->setFrameShadow(QFrame::Sunken);
        labRecordsTotal->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(labRecordsTotal);

        labRecordsPerpage = new QLabel(frameBottom);
        labRecordsPerpage->setObjectName(QString::fromUtf8("labRecordsPerpage"));
        labRecordsPerpage->setMinimumSize(QSize(0, 9));
        labRecordsPerpage->setFrameShape(QFrame::Box);
        labRecordsPerpage->setFrameShadow(QFrame::Sunken);
        labRecordsPerpage->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(labRecordsPerpage);

        labPageCurrent = new QLabel(frameBottom);
        labPageCurrent->setObjectName(QString::fromUtf8("labPageCurrent"));
        labPageCurrent->setMinimumSize(QSize(0, 9));
        labPageCurrent->setFrameShape(QFrame::Box);
        labPageCurrent->setFrameShadow(QFrame::Sunken);
        labPageCurrent->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(labPageCurrent);

        labPageTotal = new QLabel(frameBottom);
        labPageTotal->setObjectName(QString::fromUtf8("labPageTotal"));
        labPageTotal->setMinimumSize(QSize(0, 10));
        labPageTotal->setFrameShape(QFrame::Box);
        labPageTotal->setFrameShadow(QFrame::Sunken);
        labPageTotal->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(labPageTotal);

        labSelectTime = new QLabel(frameBottom);
        labSelectTime->setObjectName(QString::fromUtf8("labSelectTime"));
        labSelectTime->setMinimumSize(QSize(0, 9));
        labSelectTime->setFrameShape(QFrame::Box);
        labSelectTime->setFrameShadow(QFrame::Sunken);
        labSelectTime->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(labSelectTime);


        verticalLayout->addWidget(frameBottom);


        retranslateUi(frmDbPage);

        QMetaObject::connectSlotsByName(frmDbPage);
    } // setupUi

    void retranslateUi(QWidget *frmDbPage)
    {
        frmDbPage->setWindowTitle(QApplication::translate("frmDbPage", "\346\225\260\346\215\256\346\237\245\350\257\242", nullptr));

        lineEdit->setText(QApplication::translate("frmDbPage", "\345\205\263\351\224\256\345\255\227NG\\OK", nullptr));
        btnSelect->setText(QApplication::translate("frmDbPage", "\345\257\274\345\207\272", nullptr));
        btnLast->setText(QApplication::translate("frmDbPage", "\346\234\253\344\270\200\351\241\265", nullptr));
        btnFirst->setText(QApplication::translate("frmDbPage", "\347\254\254\344\270\200\351\241\265", nullptr));
        btnNext->setText(QApplication::translate("frmDbPage", "\344\270\213\344\270\200\351\241\265", nullptr));
        btnPreVious->setText(QApplication::translate("frmDbPage", "\344\270\212\344\270\200\351\241\265", nullptr));
        labRecordsTotal->setText(QString());
        labRecordsPerpage->setText(QString());
        labPageCurrent->setText(QString());
        labPageTotal->setText(QString());
        labSelectTime->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class frmDbPage: public Ui_frmDbPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMDBPAGE_H
