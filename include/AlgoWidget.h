#ifndef ALGOWIDGET_H
#define ALGOWIDGET_H

#include "commonZack.h"
#include "AlgoConfig.h"
#include "AlgoSetting.h"
#include "AlgoCameras.h"

#include <QObject>
#include <QWidget>

class AlgoWidget : public QWidget
{
    Q_OBJECT
public:
    AlgoWidget(QWidget* parent=0);
    ~AlgoWidget() {}

    void on_button_AlgoSetting_clicked();
    void on_button_AlgoConfig_clicked();
    void on_button_AlgoCameras_clicked();

private:
    void init_UI();

    void selectShowWidget(bool isShow1, bool isShow2, bool isShow3);

public:

    QToolButton* button_AlgoSetting;
    QToolButton* button_AlgoConfig;
    QToolButton* button_AlgoCameras;

    AlgoSetting* algoSetting;
    AlgoConfig* algoConfig;
    AlgoCameras* algoCameras;

};

#endif // ALGOWIDGET_H
