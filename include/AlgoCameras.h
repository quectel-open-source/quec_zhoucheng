#ifndef ALGOCAMERAS_H
#define ALGOCAMERAS_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QListWidget>
#include "commonZack.h"


class AlgoCameras: public QWidget
{
    Q_OBJECT

public:
    explicit AlgoCameras(QWidget *parent = 0);
    ~AlgoCameras();

    void init_camera(int net, int state);
    void update_cameras(int net, QString ipStr, int state);

    void updateShow(QString dir);

private:
    void init_UI();

    void on_comboBox_1_changed(int idx);
    void on_comboBox_2_changed(QString text);
    void on_button_remove_clicked();

signals:


private:
    QLineEdit* lineEdit_0;
    QComboBox* comboBox_1;
    QComboBox* comboBox_2;
    QLineEdit* lineEdit_3;
    QComboBox* comboBox_4;

    QPushButton* button_remove;
    QListWidget* listWidget;

    QList<QLabel*> _labels;

};




#endif // ALGOCAMERAS_H
