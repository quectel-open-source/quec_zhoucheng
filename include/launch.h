#ifndef LAUNCH_H
#define LAUNCH_H

#include "frmmain.h"
#include "commonZack.h"

class LaunchWidget : public MyWidget
{
    Q_OBJECT

public:
    LaunchWidget(QWidget* parent=0, MyWidget* widget=0);
    ~LaunchWidget() {};

public slots:
    void get_idLoaded(bool isLoaded);

    void frmMain_isLoaded_received();
private:
    void init_UI();
    void timer_timeout();
    void timer_timeout_showMainwindow();

    frmMain* _mainWindow;
    MyWidget* _centralWidget;

    QProgressBar *_progress;
    QLabel* _label;
    QTimer* _timer;
    QTimer* _timer_showMain;
    int _vaShow;
    int _value;
    bool _isLoaded;

};


class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QWidget* parent=0) {};
    ~MyMainWindow() {};

protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif // LAUNCH_H
