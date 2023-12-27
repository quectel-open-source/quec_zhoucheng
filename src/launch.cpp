#include "launch.h"


////////////////////
void MyMainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox* mgBox = newMessageBox(this, "警告", "  是否关闭软件？  ", QMessageBox::Yes | QMessageBox::No);
    if (mgBox->exec() == QMessageBox::Yes){
        QMainWindow::closeEvent(event);
    }
    else{
        event->ignore();
    }
}



////////////
LaunchWidget::LaunchWidget(QWidget* parent, MyWidget* widget):MyWidget(parent)
{
    _centralWidget = widget;

    this->setParent(parent);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->init_UI();

    _value = 0;
    _vaShow = 0;
    _isLoaded = false;

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &LaunchWidget::timer_timeout);
    _timer->start(100);

    _timer_showMain = new QTimer(this);
    _timer_showMain->setSingleShot(true);
    connect(_timer_showMain, &QTimer::timeout, this, &LaunchWidget::timer_timeout_showMainwindow);
    _timer_showMain->start(200);
}

void LaunchWidget::init_UI()
{
    printf("##########################    start load mainWindow");
    _label = newLabel(this, "正在加载软件中，请稍候...");
    MyWidget* w1 = new MyWidget(this);
    newHBoxLayout(w1, {_label});

    _progress = new QProgressBar(this);
//    _progress->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    _progress->setRange(0, 100);
    _progress->setValue(0);
    _progress->setFixedHeight(50);
    _progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    newVBoxLayout(this, {w1, _progress}, 0, 20);
    this->setStyleSheet("background-color: transparent;");
    _progress->setStyleSheet("QProgressBar{height: 50px; background-color: #55aaff; text-align: center; color: rgb(255,255,255); font-size: 30px; font-family: Bold;}"
                             "QProgressBar::chunk {background-color: #00A000;}");
    _label->setStyleSheet("QLabel {color: rgb(255, 255, 255); font-size: 30px; font-family: Bold;}");

}

void LaunchWidget::get_idLoaded(bool isLoaded)
{
    _isLoaded = isLoaded;
}

void LaunchWidget::timer_timeout()
{
    if (_isLoaded){
        _progress->setValue(100);
        _label->setText("加载软件完毕！");
    }
    else{
        _value++;
        _vaShow++;
        if (_value > 98){
            _value = 98;
//            frmMain_isLoaded_received();
        }
        if (_vaShow >99){
            _vaShow = 0;
        }
//        _progress->setMaximum(0);
        _progress->setValue(_value);

        int num = _vaShow % 9;
        if (num <= 3){
            _label->setText("正在加载软件中，请稍候.");
        }
        else if (num > 3 && num <= 6){
            _label->setText("正在加载软件中，请稍候..");
        }
        else{
            _label->setText("正在加载软件中，请稍候...");
        }
    }
}

void LaunchWidget::timer_timeout_showMainwindow()
{
     // 2.
    _mainWindow = new frmMain(this->parentWidget());
    connect(_mainWindow, &frmMain::emit_loadCompleted, this, &LaunchWidget::frmMain_isLoaded_received);
    _mainWindow->_useWidget->hide();
}

void LaunchWidget::frmMain_isLoaded_received()
{
    this->close();
    this->get_idLoaded(true);


    _centralWidget->layout()->removeWidget(this);
    _centralWidget->layout()->addWidget(_mainWindow);
    _centralWidget->layout()->addWidget(_mainWindow->_useWidget);
}
