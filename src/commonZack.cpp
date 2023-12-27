#include "commonZack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QObject>
#include <QStyleOptionHeader>
#include <QtConcurrent>

#if 1
//头文件
#include <iostream>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <vector>
#include <string.h>


#else
#include <iostream>
#include <sys/io.h>
#include <fstream>
#include <string>
#include <vector>

#endif

using namespace std;

QSqlQuery query;
QSqlDatabase dbConn;

int _saveFormatIdx = 0;
QList<QString> _saveFormats = {"%1_%2_%3"};
QString _saveSuffix = "jpg";
int _saveDays = 10;
int _saveImgsNums = 0;  // 0: 全部   >0:部分   <0:不保存
QString _saveTriggerMode = "LINE0";
QString _mapDir = "/home/quec";

/////////////////////////////
MyWidget::MyWidget(QWidget* parent, QString qss):QWidget(parent)
{
    if (parent)
        this->setParent(parent);

    this->setStyleSheet(qss);
}
void MyWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/////////////////////////////
MyTitleBar::MyTitleBar(QWidget* parent, int height, QString qss):QWidget(parent)
{
    setFixedHeight(height);  // default 30
    isMaximized = false;

    // 添加标题栏图标
    iconLabel = new QLabel(this);
    iconLabel->setPixmap(QIcon("").pixmap(height-6, height-6));
    iconLabel->move(10, 3);

    // 添加标题栏按钮
    minimizeButton = new QPushButton("-", this);
    maximizeButton = new QPushButton("+", this);
    closeButton = new QPushButton("x", this);

    minimizeButton->move(width() - 70, 5);
    maximizeButton->move(width() - 50, 5);
    closeButton->move(width() - 30, 5);

    connect(minimizeButton, &QPushButton::clicked, parentWidget(), &QWidget::showMinimized);
    connect(maximizeButton, &QPushButton::clicked, this, &MyTitleBar::toggleMaximized);
    connect(closeButton, &QPushButton::clicked, parentWidget(), &QWidget::close);
}
void MyTitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        toggleMaximized();
    }
}
void MyTitleBar::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPosition = event->globalPos();
        dragStartGeometry = parentWidget()->geometry();
    }
}
void MyTitleBar::mouseMoveEvent(QMouseEvent* event)
{
    if (isDragging) {
        int dx = event->globalX() - dragStartPosition.x();
        int dy = event->globalY() - dragStartPosition.y();
        parentWidget()->setGeometry(dragStartGeometry.x() + dx, dragStartGeometry.y() + dy, dragStartGeometry.width(), dragStartGeometry.height());
    }
}
void MyTitleBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}
void MyTitleBar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 255, 0, 50)); // 绿色半透明边框
}
void MyTitleBar::toggleMaximized()
{
    if (isMaximized) {
        parentWidget()->showNormal();
    } else {
        parentWidget()->showMaximized();
    }
    isMaximized = !isMaximized;
}


QLabel* newLabel(QWidget* parent, QString text, int w, int h, QString qssStr)
{
    QLabel* label = new QLabel(text, parent);
    if (w > 0)
        label->setFixedWidth(w);
    if (h > 0)
        label->setFixedHeight(h);

    if (qssStr.compare("") != 0)
        label->setStyleSheet(qssStr);

    return label;
}

QLineEdit* newLineEdit(QWidget* parent, QString text, int w, int h, QList<int> validator, QString qssStr, QString slotName, QVariantList args)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setText(text);
    if (w > 0)
        lineEdit->setFixedWidth(w);
    if (h > 0)
        lineEdit->setFixedHeight(h);
//    if (validator[1] > 0)
//        lineEdit->setValidator((QValidator)QIntValidator(validator[0], validator[1]));
    if (qssStr.compare("") != 0)
        lineEdit->setStyleSheet(qssStr);

    if (slotName.compare("") != 0){
        if (!QMetaObject::invokeMethod(lineEdit, slotName.toUtf8(), Qt::DirectConnection, Q_ARG(QVariantList, args))){
            qDebug() << "创建槽函数失败： " << slotName;
        }
    }

    return lineEdit;
}

QCheckBox* newCheckBox(QWidget* parent, QString text, int w, int h, bool isChecked, QString qssStr) //, QWidget* receiver, void* slot)
{
    QCheckBox* checkBox = new QCheckBox(parent);
    checkBox->setText(text);
    if (w > 0)
        checkBox->setFixedWidth(w);
    if (h > 0)
        checkBox->setFixedHeight(h);
    checkBox->setChecked(isChecked);

    if (qssStr.compare("") != 0)
        checkBox->setStyleSheet(qssStr);

    return checkBox;
}

QRadioButton* newRadioButton(QWidget* parent, QString text, int w, int h, bool isChecked, QString qssStr) //, QWidget* receiver, void* slot)
{
    QRadioButton* checkBox = new QRadioButton(parent);
    checkBox->setText(text);
    if (w > 0)
        checkBox->setFixedWidth(w);
    if (h > 0)
        checkBox->setFixedHeight(h);
    checkBox->setChecked(isChecked);

    if (qssStr.compare("") != 0)
        checkBox->setStyleSheet(qssStr);

    return checkBox;
}

QPushButton* newPushButton(QWidget* parent, QString text, QString tooltip, int w, int h, QString qssStr, QString qssStr2)
{
    QPushButton* button = new QPushButton(parent);
    button->setText(text);
    button->setToolTip(tooltip);

    if (w > 0)
        button->setFixedWidth(w);
    if (h > 0)
        button->setFixedHeight(h);

    if (qssStr2.compare("") != 0){
        button->setStyleSheet(qssStr + qssStr2);
    }

    return button;
}


QToolButton* newToolButton(QWidget* parent, QString text, QString tooltip, int w, int h, QString iconPath, bool checkable, bool isChecked, Qt::ToolButtonStyle style, QString qssStr, QString qssStr2)
{
    QToolButton* button = new QToolButton(parent);
    button->setText(text);
    button->setToolTip(tooltip);

    if (w > 0)
        button->setFixedWidth(w);
    if (h > 0)
        button->setFixedHeight(h);

    if (iconPath.compare("") != 0){
        button->setIcon(QIcon(iconPath));
    }

    if (checkable){
        button->setCheckable(checkable);
        button->setChecked(isChecked);
    }
    else
        button->setCheckable(checkable);


    button->setToolButtonStyle(style);
    if (qssStr2.compare("") != 0){
        button->setStyleSheet(qssStr + qssStr2);
    }

    return button;
}

QComboBox* newComboBox(QWidget* parent, QList<QString> texts, QString defaultText, int w, int h, int defaultIdx, QString qssBorder)
{
    QComboBox* box = new QComboBox(parent);

    if (!texts.contains(defaultText)){
        box->addItem(defaultText);
    }
    if (texts.count() > 0){
        for (auto tt: texts){
            box->addItem(tt);
        }
    }

    if (w > 0)
        box->setFixedWidth(w);
    if (h > 0)
        box->setFixedHeight(h);

    if (texts.contains(defaultText)){
        box->setCurrentText(defaultText);
    }
    else{
        box->setCurrentIndex(defaultIdx);
    }

    box->setStyleSheet("QComboBox{border:2px solid " + qssBorder + "; background-color:transparent; padding-left: 8px}"
                       "QComboBox::drop-down{border-left-width: 0px;}"
                       "QComboBox::down-arrow{image: url(:/uimain/icon/u21.png);}");

    return box;
}

QTextEdit* newTextEdit(QWidget* parent, int w, int h, bool isReadOnly)
{
    QTextEdit* text = new QTextEdit(parent);
    if (w > 0)
        text->setFixedWidth(w);
    if (h > 0)
        text->setFixedHeight(h);

    text->setReadOnly(isReadOnly);

    return text;
}

QIcon newIcon(QString path, int w, int h)
{
    QIcon icon;
    if (w > 0 && h > 0)
    {
        QPixmap pix(path);
        icon.addPixmap(pix.scaled(w, h));
    }
    else{
        icon.addFile(path, QSize(), QIcon::Normal, QIcon::Off);
    }

    return icon;
}

QTabWidget* newTabWidget(QWidget* parent, QStringList texts, QStringList tooltips,int w, int h, QTabWidget::TabPosition loc, QString qssStr)
{
    QTabWidget* tab = new QTabWidget(parent);
    tab->setTabPosition(loc);

    for (int i = 0; i < texts.count(); i++)
    {
        tab->setTabText(i, texts[i]);
        if (tooltips.count() > 0)
            tab->setTabToolTip(i, tooltips[i]);
    }

    if (w > 0)
        tab->setFixedWidth(w);
    if (h > 0)
        tab->setFixedHeight(h);

    if (qssStr.compare("") != 0)
        tab->setStyleSheet(qssStr);

    return tab;
}

QProgressBar* newProgressBar(QWidget* parent, int w, int h, int valueCur, QList<int> range, Qt::Orientation orientation, Qt::Alignment align, QString format, bool textVisible)
{
    QProgressBar* bar = new QProgressBar(parent);
    if (w > 0){
        bar->setFixedWidth(w);
    }
    if (h > 0){
        bar->setFixedHeight(h);
    }
    bar->setValue(valueCur);
    bar->setRange(range[0], range[1]);
    bar->setOrientation(orientation);
    bar->setAlignment(align);
    bar->setFormat(format);  // "cur value: %1%".arg(QString::number(50.43, 'f', 2));
    bar->setTextVisible(textVisible);

    return bar;
}

QDateTimeEdit* newDateTimeEdit(QWidget* parent, QString tooltip, int w, int h, QList<int> days, bool isPopup, QString qssStr, QString displayFormat)
{
    QDateTimeEdit* edit = new QDateTimeEdit(parent);
    edit->setToolTip(tooltip);
    if (w > 0)
        edit->setFixedWidth(w);
    if (h > 0)
        edit->setFixedHeight(h);

    edit->setMinimumDate(QDate::currentDate().addDays(days[0]));
    edit->setMaximumDate(QDate::currentDate().addDays(days[1]));

    edit->setCalendarPopup(isPopup);

    if (qssStr != "")
        edit->setStyleSheet(qssStr);
    if (displayFormat != "")
        edit->setDisplayFormat(displayFormat);

    return edit;
}

////////////////////
QFrame* newSplit(int w, int h)
{
    QFrame* frame = new QFrame;
    frame->setFrameShape(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFixedSize(w, h);
    return frame;
}

void newHBoxLayout(QWidget* parent, QList<QWidget*> widgets, int splitW, int space, QList<int> margins, Qt::Alignment align)
{
    QHBoxLayout* layout = new QHBoxLayout(parent);
    foreach(QWidget* w, widgets)
    {
        layout->addWidget(w);
    }
    layout->setSpacing(space);
    layout->setContentsMargins(margins[0], margins[1], margins[2], margins[3]);
    layout->setAlignment(align);
}

void newVBoxLayout(QWidget* parent, QList<QWidget*> widgets, int splitW, int space, QList<int> margins, Qt::Alignment align)
{
    QVBoxLayout* layout = new QVBoxLayout(parent);
    foreach(QWidget* w, widgets)
    {
        layout->addWidget(w);
    }
    layout->setSpacing(space);
    layout->setContentsMargins(margins[0], margins[1], margins[2], margins[3]);
    layout->setAlignment(align);
}

QWidget* newWidgetLine(QWidget* parent, int w, int h, QString qssStr)
{
    QWidget* wLine = new QWidget(parent);
    wLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if (w > 0)
        wLine->setFixedWidth(w);
    if (h > 0)
        wLine->setFixedHeight(h);
    wLine->setStyleSheet(qssStr);

    return wLine;
}


QMessageBox* newMessageBox(QWidget* parent, QString title, QString text, QMessageBox::StandardButtons buttons, QString qss)
{
    QMessageBox* mgBox;
    if (title == "错误"){
        mgBox = new QMessageBox(QMessageBox::Critical, title, text, buttons, parent);
    }
    else if (title == "警告"){
        mgBox = new QMessageBox(QMessageBox::Warning, title, text, buttons, parent);
    }
    else{
        mgBox = new QMessageBox(QMessageBox::Information, title, text, buttons, parent);
    }
    mgBox->setStyleSheet(qss);
    QString qss2 = "QPushButton{border:2px solid rgb(119,183,244); font-size: 18px;}  QPushButton:hover{background-color:rgb(119,183,244);}";
    if (mgBox->buttons().count() >= 1){
        mgBox->buttons()[0]->setStyleSheet(qss2);
        if (mgBox->buttons().count() >= 2){
            mgBox->buttons()[1]->setStyleSheet(qss2);
        }
    }

    return mgBox;
}


/////////////////////
void newGridLayout_1(QWidget* parent, QList<QWidget*> widgets, QList<int> rowLens, QList<int> colLens, int spaceH, int spaceV, QList<int> margins, Qt::Alignment align, bool isLastForOneRow)
{
    QGridLayout* layout = new QGridLayout(parent);
    int col_sums = 0;
    for (int n: colLens){
        col_sums += n;
    }

    int cnts = widgets.count();
    int row = 0;
    for (int i=0; i< rowLens.count(); i++)
    {
        if (i > 0){
            row += rowLens[i - 1];
        }

        int col = 0;
        for (int j=0; j < colLens.count(); j++)
        {
            if (j > 0){
                col += colLens[j - 1];
            }

            int loc = i * colLens.count() + j;
            if (loc < cnts - 1){
                layout->addWidget(widgets[loc], row, col, rowLens[i], colLens[j]);
//                std::cout << row << col << rowLens[i] << colLens[j] << std::endl;
            }
            else if (loc == cnts - 1){
                if (isLastForOneRow){
                    layout->addWidget(widgets[loc], row, col, rowLens[i], col_sums);
//                std::cout << row << col << rowLens[i] << col_sums << std::endl;
                    break;
                }
                else
                    layout->addWidget(widgets[loc], row, col, rowLens[i], colLens[j]);
            }
        }
    }

    layout->setHorizontalSpacing(spaceH);
    layout->setVerticalSpacing(spaceV);

    layout->setContentsMargins(margins[0], margins[1], margins[2], margins[3]);

    layout->setAlignment(align);

}

QGridLayout* newGridLayout(QList<QLabel*> labels, QList<int> rowLens, QList<int> colLens, int spaceH, int spaceV, QList<int> margins)
{
    QGridLayout* layout = new QGridLayout();
    int col_sums = 0;
    for (int n: colLens){
        col_sums += n;
    }

    int cnts = labels.count();
    int row = 0;
    for (int i=0; i< rowLens.count(); i++)
    {
        if (i > 0){
            row += rowLens[i - 1];
        }

        int col = 0;
        for (int j=0; j < colLens.count(); j++)
        {
            if (j > 0){
                col += colLens[j - 1];
            }

            int loc = i * colLens.count() + j;
            if (loc < cnts - 1){
                layout->addWidget(labels[loc], row, col, rowLens[i], colLens[j]);
//                std::cout << row << col << rowLens[i] << colLens[j] << std::endl;
            }
            else if (loc == cnts - 1){

                layout->addWidget(labels[loc], row, col, rowLens[i], col_sums);
//                std::cout << row << col << rowLens[i] << col_sums << std::endl;
                break;
            }
        }
    }

    layout->setHorizontalSpacing(spaceH);
    layout->setVerticalSpacing(spaceV);

    layout->setContentsMargins(margins[0], margins[1], margins[2], margins[3]);

    return layout;
}

void updateGridLayout(QGridLayout* layout, QList<QLabel*> labels, QList<int> rowLens, QList<int> colLens)
{
    int col_sums = 0;
    for (int n: colLens){
        col_sums += n;
    }

    int cnts = labels.count();
    int row = 0;
    for (int i=0; i< rowLens.count(); i++)
    {
        if (i > 0){
            row += rowLens[i - 1];
        }

        int col = 0;
        for (int j=0; j < colLens.count(); j++)
        {
            if (j > 0){
                col += colLens[j - 1];
            }

            int loc = i * colLens.count() + j;
            if (loc < cnts - 1){
                layout->addWidget(labels[loc], row, col, rowLens[i], colLens[j]);
//                std::cout << row << col << rowLens[i] << colLens[j] << std::endl;
            }
            else if (loc == cnts - 1){

                layout->addWidget(labels[loc], row, col, rowLens[i], col_sums);
//                std::cout << row << col << rowLens[i] << col_sums << std::endl;
                break;
            }
        }
    }

}

QGridLayout* newGridLayout_SiYuan(QList<QLabel*> labels, int spaceH, int spaceV, QList<int> margins)
{
    QGridLayout* layout = new QGridLayout();

    layout->addWidget(labels[0], 0, 0, 1, 1);
    layout->addWidget(labels[1], 0, 1, 1, 1);
    layout->addWidget(labels[2], 0, 2, 1, 1);

    layout->addWidget(labels[3], 1, 0, 1, 3);

    layout->addWidget(labels[4], 2, 0, 1, 1);
    layout->addWidget(labels[5], 2, 1, 1, 1);
    layout->addWidget(labels[6], 2, 2, 1, 1);

    layout->setHorizontalSpacing(spaceH);
    layout->setVerticalSpacing(spaceV);

    layout->setContentsMargins(margins[0], margins[1], margins[2], margins[3]);

    return layout;
}


void updateGridLayout_SiYuan(QGridLayout* layout, QList<QLabel*> labels)
{
    layout->addWidget(labels[0], 0, 0, 1, 1);
    layout->addWidget(labels[1], 0, 1, 1, 1);
    layout->addWidget(labels[2], 0, 2, 1, 1);

    layout->addWidget(labels[3], 1, 0, 1, 3);

    layout->addWidget(labels[4], 2, 0, 1, 1);
    layout->addWidget(labels[5], 2, 1, 1, 1);
    layout->addWidget(labels[6], 2, 2, 1, 1);
}



void newStackLayout(QWidget* parent, QList<QWidget*> widgets, int space, QList<int> margins, Qt::Alignment align)
{
    QStackedLayout* layout = new QStackedLayout(parent);
    foreach(QWidget* w, widgets)
    {
        layout->addWidget(w);
    }
    layout->setSpacing(space);
    layout->setContentsMargins(margins[0], margins[1], margins[2], margins[3]);
    layout->setAlignment(align);

}

void update_checkBoxesLayOut(QWidget* parentWidget, QList<QWidget*> widgets, int spacing, QList<int> margins, Qt::Alignment align)
{
    int cnts = widgets.count();
    int rows = 0;
    if (rows % 3 == 0){
        rows = cnts / 3;
    }
    else{
        rows = cnts / 3 + 1;
    }
    qDebug() << rows;

    rows = 2;
    if (rows == 1){
        newHBoxLayout(parentWidget, widgets, 0, spacing, {margins[0], margins[1], margins[2], margins[3]}, align);
    }
    else{
        QList<QWidget*> widgets_new;
        for (int i=0; i<rows; i++)
        {
            QList<QWidget*> widgets_row;
            for (int j=0; j<3; j++)
            {
                if (i*3+j >= cnts)
                    break;

                widgets_row.append(widgets[i*3+j]);
            }
            MyWidget* widget_row = new MyWidget(parentWidget);
            newHBoxLayout(widget_row, widgets_row, 0, spacing, {margins[0], margins[1], margins[2], margins[3]}, align);
            widgets_new.append(widget_row);
        }
        newVBoxLayout(parentWidget, widgets_new, 0, spacing, {margins[0], margins[1], margins[2], margins[3]}, align);
    }
}


/////////////////////
QToolButton * createSidebarButton(QWidget* parent, const QString& iconPath, const QString& title)
{
    QIcon icon(iconPath);

    QToolButton * btn = new QToolButton(parent);
    btn->setIcon(icon);
    btn->setIconSize(QSize(42, 42));
    btn->setText(title);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setFixedSize(160, 160);
    btn->setObjectName(title);
    btn->setCheckable(true);
    btn->setStyleSheet("background-color:rgb(30,41,54); text-align:left; font-size:24px; color: white");

    return btn;
}


/////////////////////
bool endsWith(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::vector<std::string> GetFilesFromDir(string dir, string fileType, bool isPathOrName)
{
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.is_regular_file())
        {
            if (fileType == "") {
                string info = isPathOrName ? entry.path().string(): entry.path().filename().string();
                files.push_back(entry.path().string());
            }
            else{
                string info = entry.path().filename().string();
                if (endsWith(info, fileType)){
                    if(isPathOrName){
                        info = entry.path().string();
                    }
                    files.push_back(info);
                }
            }
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

bool clearDirectory(const QString& path)
{
    QDir directory(path);

    if (!directory.exists()) {
        qDebug() << "Directory does not exist.";
        return false;
    }

    // 获取文件夹内的所有文件
    QStringList files = directory.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);

    // 删除每个文件
    foreach (QString file, files) {
        QString filePath = directory.absoluteFilePath(file);
        QFile::remove(filePath);
    }

    // 获取文件夹内的所有子文件夹
    QStringList subDirectories = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    // 递归清除每个子文件夹
    foreach (QString subDirectory, subDirectories) {
        QString subDirectoryPath = directory.absoluteFilePath(subDirectory);
        clearDirectory(subDirectoryPath);
    }

    // 删除当前文件夹
    return directory.rmdir(".");
}

bool clearFilesFromDir(QString folderPath, float days)
{
    QDir dir(folderPath);
    if (!dir.exists()){
        return false;
    }

    QStringList files = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for(QString file : files)
    {
        QString path = folderPath + QDir::separator() + file;
        QFileInfo fileInfo(path);
        if (fileInfo.isDir()){
            if (days > 0){
                QDateTime created = fileInfo.birthTime();
                qint64 dayDiff = created.daysTo(QDateTime::currentDateTime());
                if (dayDiff > days){
                    clearFilesFromDir(path);
                    dir.rmdir(path);
                }
            }
            else{
                clearFilesFromDir(path);
                dir.rmdir(path);
            }
        }
        else{
            if (days > 0){
                QDateTime created = fileInfo.birthTime();
                qint64 dayDiff = created.daysTo(QDateTime::currentDateTime());
                if (dayDiff > days)
                    QFile::remove(path);
            }
            else{
                QFile::remove(path);
            }
        }
    }

    return true;
}

/////////////////

QString getCPUID()
{
    QString cpuModel;

    ifstream cpuInfoFile("/proc/cpuinfo");
    string line;
    while (getline(cpuInfoFile, line))
    {

        QStringList fields = QString(line.c_str()).split(':');
        if (fields.count() == 2){
            QString key = fields[0].trimmed();
            QString value = fields[1].trimmed();

            if (key.remove("\t") == "model name"){
                cpuModel = value;
                break;
            }
        }
    }

    cpuInfoFile.close();
    return cpuModel;
}



QImage Mat2QImage(cv::Mat &m) //Mat转QImage
{
    //判断m的类型，可能是CV_8UC1  CV_8UC2  CV_8UC3  CV_8UC4
    switch(m.type())
    { //QIamge 构造函数, ((const uchar *data, 宽(列),高(行), 一行共多少个（字节）通道，宽度*字节数，宏参数)
    case CV_8UC1:
    {
        QImage img((uchar *)m.data,m.cols,m.rows,m.cols * 1,QImage::Format_Grayscale8);
        return img;
    }
        break;
    case CV_8UC3:   //一个像素点由三个字节组成
    {
        //cvtColor(m,m,COLOR_BGR2RGB); BGR转RGB
        QImage img((uchar *)m.data,m.cols,m.rows,m.cols * 3,QImage::Format_RGB888);
        return img.rgbSwapped(); //opencv是BGR  Qt默认是RGB  所以RGB顺序转换
    }
        break;
    case CV_8UC4:
    {
        QImage img((uchar *)m.data,m.cols,m.rows,m.cols * 4,QImage::Format_RGBA8888);
        return img;
    }
        break;
    default:
    {
        QImage img; //如果遇到一个图片均不属于这三种，返回一个空的图片
        return img;
    }
    }
}

cv::Mat QImage2Mat(QImage &image) //QImage转Mat
{
    cv::Mat mat = cv::Mat::zeros(image.height(), image.width(),image.format()); //初始化Mat
    switch(image.format()) //判断image的类型
    {
        case QImage::QImage::Format_Grayscale8:  //灰度图
            mat = cv::Mat(image.height(), image.width(),
            CV_8UC1,(void*)image.constBits(),image.bytesPerLine());
            break;
        case QImage::Format_RGB888: //3通道彩色
            mat = cv::Mat(image.height(), image.width(),
                CV_8UC3,(void*)image.constBits(),image.bytesPerLine());
            break;
        case QImage::Format_ARGB32: //4通道彩色
            mat = cv::Mat(image.height(), image.width(),
                CV_8UC4,(void*)image.constBits(),image.bytesPerLine());
            break;
        default:
            return mat;
    }
    return mat;
}

float getSum(QList<int> list)
{
    float sum = 0.0;
    for (int i: list)
    {
        sum += i;
    }

    return sum;
}


cv::Mat MergeImages(std::vector<cv::Mat> images, int rows, int cols, int row_spacing, int col_spacing)
{
    int total_width = images[0].cols * cols + col_spacing * (cols - 1);
    int total_height = images[0].rows * rows + row_spacing * (rows - 1);
    cv::Mat composite_image(total_height, total_width, images[0].type(), cv::Scalar(0, 0, 0)); // Create a white canvas
    int y_offset = 0;
    int cnts = images.size();
    for (int r = 0; r < rows; ++r) {
        int x_offset = 0;
        for (int c = 0; c < cols; ++c) {
            int image_index = r * cols + c;

            if (image_index < cnts) {
                cv::Rect roi(x_offset, y_offset, images[image_index].cols, images[image_index].rows);
                images[image_index].copyTo(composite_image(roi));
            }
            x_offset += images[0].cols + col_spacing;
        }
        y_offset += images[0].rows + row_spacing;
    }

    return composite_image;
}

QImage MergeImages_qimage(QList<QImage> images, int rows, int cols, int row_spacing, int col_spacing, int channels, int result, bool showLabel, QList<int> labels)
{
    int w0 = images[0].width();
    int h0 = images[0].height() ;
    int total_width = w0 * cols + col_spacing * (cols - 1);
    int total_height = h0* rows + row_spacing * (rows - 1);

    QImage mergeImage;
    if (channels == 3)
        mergeImage = QImage(total_width, total_height, QImage::Format_RGB888);
    else
        mergeImage = QImage(total_width, total_height, QImage::Format_Grayscale8);
    mergeImage.fill(Qt::black);

   QPainter painter(&mergeImage);
   painter.setRenderHint(QPainter::Antialiasing);

   int y_offset = 0;
   int cnts = images.size();
   for (int r = 0; r < rows; ++r) {
       int x_offset = 0;
       for (int c = 0; c < cols; ++c) {
           int image_index = r * cols + c;

           if (image_index < cnts) {
               QRect roi;
               if (images[image_index].width() >= w0)
                   roi = QRect(x_offset, y_offset, images[image_index].width(), images[image_index].height());
               else
                   roi = QRect(x_offset + (w0 - images[image_index].width())/2, y_offset + (h0 - images[image_index].height())/2, images[image_index].width(), images[image_index].height());

               painter.drawImage(roi, images[image_index]);

               if (showLabel){
                   QFont font1;
                   int size1 = images[image_index].width() >= images[image_index].height() ? images[image_index].height() / 10 : images[image_index].width() / 10;
                   font1.setPointSize(size1);

                   QString text1;
                   QColor color1;
                   if (labels[image_index] == 0){
                       text1 = "OK";
                       color1 = QColor(0, 255, 0);
                   }
                   else{
                       text1 = "NG";
                       color1 = QColor(255, 0, 0);
                   }
                   painter.setFont(font1);
                   painter.setPen(color1);
                   painter.drawText(roi.x() + 10, roi.y() + 10+size1, text1);
               }

           }
           x_offset += images[0].width() + col_spacing;
       }
       y_offset += images[0].height() + row_spacing;
   }

   QFont font;
   int size = total_width >= total_height ? total_height / 10 : total_width / 10;
   font.setPointSize(size);
   font.setBold(true);

   QString text;
   QColor color;
   if (result == 0){
       text = "OK";
       color = QColor(0, 255, 0);
   }
   else{
       text = "NG";
       color = QColor(255, 0, 0);
   }
   painter.setFont(font);
   painter.setPen(color);
   painter.drawText(10, 10+size, text);

   return mergeImage;
}


QString Int2QString(int num, int countsFor0)
{
    QString str = QString("%1").arg(num, countsFor0, 10, QChar('0'));

    return str;
}


bool IsValidText(QString text, QString type)
{
    bool isInt = false;
    int value;
    if (type == "int")
        value = text.toInt(&isInt);
    else if (type == "float")
        value = text.toFloat(&isInt);

    return isInt;
}

int getCPU()
{
//    QProcess process;
//    process.start("mpstat", QStringList() << "-P" << "ALL" << "1" << "1"); // 每秒获取一次CPU使用率
//    process.waitForFinished();

//    if (process.exitCode() != 0) {
//        qWarning() << "Failed to run mpstat command";
//        return 0;
//    }

//    QString output = process.readAllStandardOutput();
//    QStringList lines = output.split("\n", QString::SkipEmptyParts);

//    // 查找包含CPU使用率信息的行
//    for (const QString &line : lines) {
//        if (line.contains("CPU ")) {
//            QStringList tokens = line.split(" ", QString::SkipEmptyParts);
//            if (tokens.size() >= 12) {
//                QString cpuName = tokens[1];
//                float cpuUsage = 100.0f - tokens[12].toFloat(); // 第12个字段表示CPU使用率

//                return (int)cpuUsage;
//            }
//        }
//    }

    QProcess process;
    process.start("top", QStringList() << "-b" << "-n" << "1"); // 运行top命令并输出一次
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qWarning() << "Failed to run top command";
        return 1;
    }

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);

    // 查找包含CPU使用率信息的行
    for (const QString &line : lines) {
        if (line.startsWith("%Cpu(s):")) {
            QStringList tokens = line.split(" ", QString::SkipEmptyParts);
            if (tokens.size() >= 10) {
                QString cpuUsageStr = tokens[7];
                cpuUsageStr.chop(1); // 去掉百分号
                float cpuUsage = cpuUsageStr.toFloat();

                return (int)cpuUsage;
            }
        }
    }

    return 0;
}
int getMemory()
{
//    struct sysinfo memInfo;
//    sysinfo(&memInfo);
//    unsigned long long totalRAM = memInfo.totalram;
//    unsigned long long freeRAM = memInfo.freeram;
//    unsigned long long cacheRAM = memInfo.;

//    if (totalRAM > 0) {
//        float memoryUsage = 100.0f * (1.0f - (float)freeRAM / totalRAM);
//        return memoryUsage;
//    }

//    return -1.0f;

    // 2.
    QProcess process;
    process.start("free", QStringList() << "-b"); // 使用 -b 选项以字节为单位显示内存信息
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qWarning() << "Failed to run free command";
        return 0;
    }

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);

    if (lines.size() >= 2) {
        QString memoryInfoLine = lines[1]; // 第二行包含内存信息

        QStringList tokens = memoryInfoLine.split(" ", QString::SkipEmptyParts);
        if (tokens.size() >= 7) {
            unsigned long long totalMemory = tokens[1].toULongLong();
            unsigned long long usedMemory = tokens[2].toULongLong();
//            unsigned long long freeMemory = tokens[3].toULongLong();
//            unsigned long long sharedMemory = tokens[4].toULongLong();
//            unsigned long long cacheMemory = tokens[5].toULongLong();
//            unsigned long long buffersMemory = tokens[6].toULongLong();
            float memoryUsage = 100.0f * ((float)usedMemory / (float)totalMemory);
            return (int)memoryUsage;
        }
    }

    return 0;
}
int getGPU()
{
    QProcess process;
    process.start("nvidia-smi --query-gpu=memory.total,memory.used,utilization.gpu --format=csv,noheader,nounits"); /*utilization.gpu*/
    process.waitForFinished();
    if (process.exitCode() == 0) {
        QByteArray output = process.readAllStandardOutput();
        QString outputStr = QString::fromLocal8Bit(output);
        QStringList lines = outputStr.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (lines.count() >= 1) {
            QStringList infos = lines[0].split(",");
            float gpuTotal = infos[0].trimmed().toFloat();
            float gpuUsed = infos[1].trimmed().toFloat();

            float gpuUtil = infos[2].trimmed().toFloat();

//            return int(100.0f * gpuUsed / gpuTotal);
            return int(1.0f * gpuUtil);
        }
    }
    return 0;
}
int getDisk()
{
//    struct statvfs stat;
//    const char* path = "/";
//    if (statvfs(path, &stat) == 0){
//        unsigned long totalSpace = stat.f_blocks * stat.f_frsize;
//        unsigned long usedSpace = (stat.f_blocks - stat.f_bfree) * stat.f_frsize;
////        unsigned long freeSpace = stat.f_bavail * stat.f_frsize;

//        return int(100.0f *usedSpace / totalSpace);
    QProcess process;
    process.start("nvidia-smi --query-gpu=memory.total,memory.used,utilization.gpu --format=csv,noheader,nounits"); /*utilization.gpu*/
    process.waitForFinished();
    if (process.exitCode() == 0) {
        QByteArray output = process.readAllStandardOutput();
        QString outputStr = QString::fromLocal8Bit(output);
        QStringList lines = outputStr.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (lines.count() >= 1) {
            QStringList infos = lines[0].split(",");
            float gpuTotal = infos[0].trimmed().toFloat();
            float gpuUsed = infos[1].trimmed().toFloat();

            float gpuUtil = infos[2].trimmed().toFloat();

//            return int(100.0f * gpuUsed / gpuTotal);
            return int(1.0f * gpuUtil);
        }
    }

    return 0;
}

/////////////////////////////
SystemUsedWidget::SystemUsedWidget(QWidget*parent):MyWidget(parent)
{
    this->setParent(parent);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    init_UI();
}
void SystemUsedWidget::init_UI()
{
    int ww = 160;
    int hh = 28;

    QLabel* label1 = newLabel(this, "CPU使用率:", 0, hh);
    label1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _bar1 = newProgressBar(this, 0, hh);
    _bar1->setMinimumWidth(ww);
    _bar1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MyWidget* w1 = new MyWidget(this);
    w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    newHBoxLayout(w1, {label1, _bar1}, 0, 8);

    QLabel* label2 = newLabel(this, "内存使用率:", 0, hh);
    label2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _bar2 = newProgressBar(this, 0, hh);
    _bar2->setMinimumWidth(ww);
    _bar2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MyWidget* w2 = new MyWidget(this);
    w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    newHBoxLayout(w2, {label2, _bar2}, 0, 8);

    QLabel* label3 = newLabel(this, "磁盘使用率:", 0, hh);
    label3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _bar3 = newProgressBar(this, 0, hh);
    _bar3->setMinimumWidth(ww);
    _bar3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MyWidget* w3 = new MyWidget(this);
    w3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    newHBoxLayout(w3, {label3, _bar3}, 0, 8);

    QLabel* label4 = newLabel(this, "GPU利用率:", 0, hh); // 磁盘使用率   GPU利用率
    label4->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _bar4 = newProgressBar(this, 0, hh);
    _bar4->setMinimumWidth(ww);
    _bar4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MyWidget* w4 = new MyWidget(this);
    w4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    newHBoxLayout(w4, {label4, _bar4}, 0, 8);

    QWidget* temp1 = new QWidget(this);
    temp1->setMinimumWidth(20);
    temp1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    newHBoxLayout(this, {w1, w2, w3, w4, temp1}, 0, 12, {10,4,4,4}, Qt::AlignLeft | Qt::AlignVCenter);
    this->setStyleSheet("background-color: rgb(40,49,60);");

    QString qss = "background-color: rgb(40,49,60); color: rgb(200,200,200); font-size: 22px;";
    w1->setStyleSheet(qss);
    w2->setStyleSheet(qss);
    w3->setStyleSheet(qss);
    w4->setStyleSheet(qss);
    temp1->setStyleSheet(qss);
    label1->setStyleSheet(qss);
    label2->setStyleSheet(qss);
    label3->setStyleSheet(qss);
    label4->setStyleSheet(qss);

    QString qss1 = "QProgressBar{border: 0px solid rgb(200,200,200); border-radius: 5px; background-color: rgb(10,10,10); text-align: center; "
                   "color: rgb(255,255,255); font-size: 22px;}";
    QString qss2 = "QProgressBar::chunk{background-color: rgb(0,100,100);}"; // width: 20px; margin: 0.5px;}";
    _bar1->setStyleSheet(qss1 + qss2);
    _bar2->setStyleSheet(qss1 + qss2);
    _bar3->setStyleSheet(qss1 + qss2);
    _bar4->setStyleSheet(qss1 + qss2);
}

void SystemUsedWidget::updateShow(int cpu, int mem, int disk, int gpu)
{
    _bar1->setValue(cpu);
    _bar2->setValue(mem);
    _bar3->setValue(disk);
    _bar4->setValue(gpu);
}

int SystemUsedWidget::getCPUUsage() {
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open /proc/stat";
        return 0;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList tokens = line.split(" ", QString::SkipEmptyParts);
    file.close();

    if (tokens.size() < 9) {
        qWarning() << "Invalid /proc/stat format";
        return 0;
    }

    qint64 total = 0;
    qint64 idle = tokens[4].toLongLong();
    for (int i = 1; i < tokens.size(); i++) {
        total += tokens[i].toLongLong();
    }

    float usage = 100.0f * (1.0f - (float)idle / total);
    return (int)usage;
}
int SystemUsedWidget::getMemoryUsage()
{
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open /proc/meminfo";
        return 0;
    }

    QTextStream in(&file);
    long long totalMemory = -1;
    long long freeMemory = -1;
    long long cachedMemory = -1;
    long long buffersMemory = -1;
    file.close();

    qDebug() << "1111111";
    while (!in.atEnd()) {
        qDebug() << "22222222";
        QString line = in.readLine();
        QStringList tokens = line.split(":", QString::SkipEmptyParts);

        if (tokens.size() == 2) {
            QString key = tokens[0].trimmed();
            QString value = tokens[1].trimmed().split(" ")[0];

            if (key == "MemTotal") {
                totalMemory = value.toLongLong();
            } else if (key == "MemFree") {
                freeMemory = value.toLongLong();
            } else if (key == "Cached") {
                cachedMemory = value.toLongLong();
            } else if (key == "Buffers") {
                buffersMemory = value.toLongLong();
            }
        }

        if (totalMemory != -1 && freeMemory != -1 && cachedMemory != -1 && buffersMemory != -1) {
            break; // 已经找到所需信息，退出循环
        }
    }

    if (totalMemory > 0 && freeMemory >= 0 && cachedMemory >= 0 && buffersMemory >= 0) {
        long long usedMemory = totalMemory - freeMemory - cachedMemory - buffersMemory;
        float usage = 100.0f * (usedMemory / (float)totalMemory);
        return (int)usage;
    }

    return 0;
}
int SystemUsedWidget::getGPUUsage() {
//    nvmlReturn_t result;
//    result = nvmlInit();
//    if (result != NVML_SUCCESS) {
//        qWarning() << "Failed to initialize NVIDIA NVML library";
//        return -1.0f;
//    }

//    nvmlDevice_t device;
//    result = nvmlDeviceGetHandleByIndex(0, &device); // Assuming there's only one GPU
//    if (result != NVML_SUCCESS) {
//        qWarning() << "Failed to get GPU handle";
//        nvmlShutdown();
//        return -1.0f;
//    }

//    nvmlUtilization_t utilization;
//    result = nvmlDeviceGetUtilizationRates(device, &utilization);
//    if (result != NVML_SUCCESS) {
//        qWarning() << "Failed to get GPU utilization";
//        nvmlShutdown();
//        return -1.0f;
//    }

//    nvmlShutdown();

//    return utilization.gpu;
}


void SystemUsedWidget::updateSystemUsad(bool isStarted)
{
//    if (isStarted){
//        _timer->start(1000);
//    }
//    else{
//        _timer->stop();
//    }
}

///////////////////////////
ClearDataWidget::ClearDataWidget(QWidget* parent):MyWidget(parent)
{
    this->setParent(parent);
    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);

    this->init_UI();
    this->setWindowTitle("清除窗口");
    QRect desktop = QApplication::desktop()->availableGeometry();
    this->move((desktop.width() - this->width()) / 2, (desktop.height() - this->height()) / 2);

}

void ClearDataWidget::init_UI()
{
    int ww = 210;
    int hh = 40;
   _checkBox_clearResult = newCheckBox(this, "清除 统计结果", ww, hh, false, "QCheckBox {border: 0px; font-size: 24px;} QCheckBox::indicator{width: 20px; height: 20px;}");
//   _checkBox_clearSql = newCheckBox(this, "清除数据库文件", ww, hh, false, "QCheckBox {border: 0px; font-size: 24px;} QCheckBox::indicator{width: 20px; height: 20px;}");
   _checkBox_clearImgs = newCheckBox(this, "清除 图像", ww, hh, false, "QCheckBox {border: 0px; font-size: 24px;} QCheckBox::indicator{width: 20px; height: 20px;}");
   MyWidget* w_1 = new MyWidget(this);
   newVBoxLayout(w_1, {_checkBox_clearResult, _checkBox_clearImgs}, 0, 10, {0,0,0,0}, Qt::AlignCenter);

   QToolButton* button = newToolButton(this, "确 定", "", ww+20, hh, "", false, false, Qt::ToolButtonTextOnly, "QToolButton{font-size: 24px;}");
   MyWidget* w_btn = new MyWidget(this);
   newVBoxLayout(w_btn, {button}, 0, 0, {0,0,0,0}, Qt::AlignLeft | Qt::AlignVCenter);
   connect(button, &QToolButton::clicked, this, &ClearDataWidget::on_toolButton_clicked);

   newVBoxLayout(this, {w_1, button}, 0, 10, {20,20,20,20}, Qt::AlignCenter);
}

void ClearDataWidget::init_state()
{
    _checkBox_clearResult->setChecked(false);
    _checkBox_clearImgs->setChecked(false);
}

void ClearDataWidget::on_toolButton_clicked()
{
    QMessageBox* mgBox_1 = newMessageBox(this, "警告", "清除前，请先停止机构运行！！！");
    mgBox_1->exec();

    QMessageBox* mgBox = newMessageBox(this, "提示", "是否确定清除所选项？", QMessageBox::Yes | QMessageBox::No);
    mgBox->buttons()[0]->setFixedSize(120, 40);
    mgBox->buttons()[1]->setFixedSize(120, 40);
    if (mgBox->exec() != QMessageBox::Yes){
        return;
    }

    int clear_1 = -1;
    int clear_2 = -1;
    if (_checkBox_clearResult->isChecked()){
        clear_1 = 0;
    }
    if (_checkBox_clearImgs->isChecked()){
        clear_2 = 0;
    }

    if (clear_1 == 0 || clear_2 == 0){
        emit emit_clearAndCreate(clear_1, clear_2);
    }
}






















