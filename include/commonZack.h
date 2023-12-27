#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QtCore>
#include <Qt>
#include <QtGui>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QComboBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QTabWidget>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QProgressBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include "qsqldatabase.h"
#include "qsqlquery.h"

#include <iostream>
#include <filesystem>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

extern QSqlQuery query;
extern QSqlDatabase dbConn;

extern int _saveFormatIdx;
extern QList<QString> _saveFormats;
extern QString _saveSuffix;
extern int _saveDays;
extern int _saveImgsNums;
extern QString _saveTriggerMode;
extern QString _mapDir;

/////////////////////////////
class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(QWidget* parent=0, QString qss="border: 0px;");

protected:
    void paintEvent(QPaintEvent *event);
};

/////////////////////////////
class MyTitleBar: public QWidget
{
    Q_OBJECT

public:
    MyTitleBar(QWidget* parent=0, int height=30, QString qss="border: 0px;");

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void toggleMaximized();

private:
    bool isMaximized;
    bool isDragging;
    QPoint dragStartPosition;
    QRect dragStartGeometry;
    QLabel* iconLabel;
    QPushButton* minimizeButton;
    QPushButton* maximizeButton;
    QPushButton* closeButton;
};


// Widget
QLabel* newLabel(QWidget* parent, QString text="", int w=0, int h=0,
                 QString qssStr="border: none;");
QLineEdit* newLineEdit(QWidget* parent, QString text="", int w=0, int h=0, QList<int> validator={0, 0},
                       QString qssStr="border:2px solid rgb(119,183,244); background-color:transparent; padding-left: 8px", QString slotName="", QVariantList args={});
QCheckBox* newCheckBox(QWidget* parent, QString text="", int w=0, int h=0, bool isChecked=false,
                       QString qssStr="border: none;"); //, QWidget* receiver=NULL, void* slot=NULL);
QRadioButton* newRadioButton(QWidget* parent, QString text, int w, int h, bool isChecked,
                          QString qssStr="border: none;");
QPushButton* newPushButton(QWidget* parent, QString text="", QString tooltip="", int w=0, int h=0,
                           QString qssStr="", QString qssStr2="QPushButton{border:2px solid rgb(119,183,244);}  QPushButton:hover{background-color:rgb(119,183,244);}");
QToolButton* newToolButton(QWidget* parent, QString text="", QString tooltip="", int w=0, int h=0, QString iconPath="", bool checkable=true, bool isChecked=false, Qt::ToolButtonStyle style=Qt::ToolButtonTextOnly,
                           QString qssStr="", QString qssStr2="QToolButton{border:2px solid rgb(119,183,244);}  QToolButton:hover{background-color:rgb(119,183,244);}");
QComboBox* newComboBox(QWidget* parent, QList<QString> texts={}, QString defaultText="", int w=0, int h=0, int defaultIdx=1,
                       QString qssBorder="rgb(119,183,244)");
QTextEdit* newTextEdit(QWidget* parent, int w=0, int h=0, bool isReadOnly=true);
QTabWidget* newTabWidget(QWidget* parent, QStringList texts, QStringList tooltips={},int w=0, int h=0, QTabWidget::TabPosition loc=QTabWidget::North,
                         QString qssStr="");
QProgressBar* newProgressBar(QWidget* parent, int w=0, int h=0, int valueCur=0, QList<int> range={0, 100},
                             Qt::Orientation orientation=Qt::Horizontal, Qt::Alignment align=Qt::AlignLeft,
                             QString format="%p%", bool textVisible=true);

QDateTimeEdit* newDateTimeEdit(QWidget* parent, QString tooltip, int w=0, int h=0, QList<int> days={-1, 1}, bool isPopup=false,
                               QString qssStr="border: 1px solid rgb(119,183,244);", QString displayFormat="yyyy-MM-dd hh:mm:ss");


QIcon newIcon(QString path, int w=0, int h=0);

QFrame* newSplit(int w, int h);

void newHBoxLayout(QWidget* parent, QList<QWidget*> widgets, int splitW=0, int space=0, QList<int> margins={0,0,0,0}, Qt::Alignment align=Qt::AlignCenter);
void newVBoxLayout(QWidget* parent, QList<QWidget*> widgets, int splitW=0, int space=0, QList<int> margins={0,0,0,0}, Qt::Alignment align=Qt::AlignCenter);

void newStackLayout(QWidget* parent, QList<QWidget*> widgets, int space=0, QList<int> margins={0,0,0,0}, Qt::Alignment align=Qt::AlignLeft | Qt::AlignTop);

void newGridLayout_1(QWidget* parent, QList<QWidget*> widgets, QList<int> rowLens, QList<int> colLens, int spaceH=0, int spaceV=0, QList<int> margins={0,0,0,0}, bool isLastForOneRow=true);

QGridLayout* newGridLayout(QList<QLabel*> labels, QList<int> rowLens, QList<int> colLens, int spaceH=0, int spaceV=0, QList<int> margins={0,0,0,0});

void updateGridLayout(QGridLayout* layout, QList<QLabel*> labels, QList<int> rowLens, QList<int> colLens);

QGridLayout* newGridLayout_SiYuan(QList<QLabel*> labels, int spaceH, int spaceV, QList<int> margins);
void updateGridLayout_SiYuan(QGridLayout* layout, QList<QLabel*> labels);

void update_checkBoxesLayOut(QWidget* parentWidget, QList<QWidget*> widgets, int spacing=0, QList<int> margins={0,0,0,0}, Qt::Alignment align=Qt::AlignLeft | Qt::AlignTop);

QWidget* newWidgetLine(QWidget* parent, int w=0, int h=0, QString qssStr="");

QMessageBox* newMessageBox(QWidget* parent, QString title, QString text, QMessageBox::StandardButtons buttons=QMessageBox::Ok, QString qss="QLabel {font-size: 18px;} QPushButton{border: 1px solid rgb(119,183,244); font-size: 18px;}");


// 功能
bool endsWith(std::string const &str, std::string const &suffix);

std::vector<std::string> GetFilesFromDir(string dir, string fileType, bool isPathOrName=true);

bool clearDirectory(const QString& path);

bool clearFilesFromDir(QString folderPath, float days=-1);

std::vector<std::string> getAllFiles(const fs::path& directory);


//
#include <opencv2/opencv.hpp>
QImage Mat2QImage(cv::Mat &m);

cv::Mat QImage2Mat(QImage &image);

float getSum(QList<int> list);

cv::Mat MergeImages(std::vector<cv::Mat> images, int rows, int cols, int row_spacing, int col_spacing);

QImage MergeImages_qimage(QList<QImage> images, int rows, int cols, int row_spacing, int col_spacing, int channels=3, int result=0, bool showLabel=false, QList<int> labels={});

QString Int2QString(int num, int countsFor0=6);

bool IsValidText(QString text, QString type="int");

int getCPU();
int getMemory();
int getGPU();
int getDisk();

/////////////////////////////
#include <QTextStream>
//#include <nvml.h>
#include <sys/sysinfo.h>

class SystemUsedWidget : public MyWidget
{
    Q_OBJECT

public:
    SystemUsedWidget(QWidget* parent=0);
    ~SystemUsedWidget() {};

    void updateSystemUsad(bool isStarted);

    void updateShow(int cpu, int mem, int disk, int gpu);

private:
    void init_UI();

    int getCPUUsage();
    int getMemoryUsage();
    int getGPUUsage();


    QProgressBar* _bar1;
    QProgressBar* _bar2;
    QProgressBar* _bar3;
    QProgressBar* _bar4;
};


class ClearDataWidget : public MyWidget
{
    Q_OBJECT

public:
    ClearDataWidget(QWidget* parent=0);
    ~ClearDataWidget() {};

    void init_state();

private:
    void init_UI();
    void on_toolButton_clicked();

    QCheckBox* _checkBox_clearResult;
    QCheckBox* _checkBox_clearSql;
    QCheckBox* _checkBox_clearImgs;

signals:
    void emit_clearAndCreate(int ret_1, int ret_2);
};


// Class
#include <iostream>
#include <vector>

template<typename T>
class CircularQueue
{
private:
    std::vector<T> queue_;
    int size_;
    int front_;
    int rear_;
    int count_;

public:
    CircularQueue(int size) : size_(size), front_(0), rear_(0), count_(0)
    {
        queue_.resize(size);
    }

    bool isEmpty() const {
        return count_ == 0;
    }

    bool isFull() const {
        return count_ == size_;
    }

    void enqueue(const T& value) {
        if (isFull()) {
            std::cout << "队列已满，无法入队！  " << count_ << "/" << size_ << std::endl;
            return;
        }

        queue_[rear_] = value;
        rear_ = (rear_ + 1) % size_;
        count_++;
    }

    T dequeue() {
        if (isEmpty()) {
            std::cout << "队列为空，无法出队！" << std::endl;
            return T(); // 返回默认构造的 T 类型对象，表示出队失败
        }

        T value = queue_[front_];
        front_ = (front_ + 1) % size_;
        count_--;
        return value;
    }

    int size()
    {
        return size_;
    }

    int count()
    {
        return count_;
    }
    void setSize(int size)
    {
        queue_.resize(size);
    }
};

typedef struct CamRusultRecv
{
    QList<int>    pred_results;
    QList<QImage> labels_show;
    QList<int>    labels_index;

    void clear(int nums=4)
    {
//        pred_results.clear();
//        labels_show.clear();
//        labels_index.clear();
        if (pred_results.count() >= nums){
            for(int i=0; i<nums; i++){
                pred_results.removeFirst();
            }
        }
        if (labels_show.count() >= nums){
            for(int i=0; i<nums; i++){
                labels_show.removeFirst();
            }
        }
        if (labels_index.count() >= nums){
            for(int i=0; i<nums; i++){
                labels_index.removeFirst();
            }
        }
    }

    void clearAll()
    {
        pred_results.clear();
        labels_show.clear();
        labels_index.clear();
    }
}CamRusultRecv;


#endif // COMMON_H
