#ifndef ALGOCONFIG_H
#define ALGOCONFIG_H

#include "commonZack.h"
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QPointF>
#include <QTextEdit>
#include <QFileDialog>
#include <QTimer>

#include <iostream>
#include <vector>

using namespace std;

class MyGraphicsView;

class MyPixmapItem : public QGraphicsPixmapItem
{

public:
    MyPixmapItem(MyGraphicsView* parent, QPixmap pixmap, bool clickable=false);

    void on_timer_timeout();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void setZoomStep(bool isUp, float ratio, float& zoom_step, float& zoom_min, float& zoom_max);
    void Zoom(int angle, int x, int y, const QPointF pos);

    QPointF mapToImage(const QPoint& pos);



private:
    MyGraphicsView* _parent;
    bool _clickable;

    bool _isMoving;
    bool _isPressing;
//    QPointF _mousePressPos;
    QList<QPointF> _pts;
    QList<QList<int>> _ptsImage;

    int _originX;
    int _originY;

public:
    float ratio;

};

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    MyGraphicsView(QWidget* parent=0, bool clickable=false);

    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void addCrossLine();
    void addItem(QString imagePath);
    void getNewShowInfo(const QPixmap& pix, int sceneW, int sceneH, float& originX, float& originY, float& imgW_resize, float& imgH_resize, float& ratio);
    void resizeImg(int imgW, int imgH, int sceneW, int sceneH, float& originX, float& originY, float& imgW_resize, float& imgH_resize, float& ratio);

    void updateText(QList<QList<int>> pts);
    void open_pixmap_crossLine(bool isOpened);

    MyPixmapItem* _pixmapItem;
    QGraphicsLineItem* _crossItemX;
    QGraphicsLineItem* _crossItemY;
    bool _isSetROI;

    bool _clickable;
    QGraphicsScene* _scene;
    int _sceneW;
    int _sceneH;
    QTimer* _timer;

    float _originX;
    float _originY;
    float _ratio;
    float imgW_resize;
    float imgH_resize;

    QGraphicsRectItem* _rectItem;
    QGraphicsTextItem* _textItem;
    QPointF _ptStart;
    bool _isDrawingRect;

signals:
    void emit_updateText(QString txt);
    void emit_imageWH(int w, int h);

public slots:
    void on_timer_timeout();

};

class AlgoConfig : public QWidget
{
    Q_OBJECT
public:
    AlgoConfig(QWidget* parent=0);
    ~AlgoConfig();

private:
    void init_UI();

    void on_buttonOpen_clicked();
    void on_buttonYes_clicked();


    QToolButton* _buttonOpen;
    QToolButton* _buttonYes;
    QTextEdit* _textEdit;
    MyGraphicsView* _view;


private slots:
    void on_textEdit_updated(QString text);

};



#endif // ALGOCONFIG_H
