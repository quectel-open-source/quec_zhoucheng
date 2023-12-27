#include "AlgoConfig.h"


/////////////////////////////
MyPixmapItem::MyPixmapItem(MyGraphicsView* parent, QPixmap pixmap, bool clickable) : QGraphicsPixmapItem(pixmap)
{
    _parent = parent;
    _clickable = clickable;
    this->setPixmap(pixmap);

    this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    this->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
//    this->setAcceptHoverEvents(true);

    _isMoving = false;
    _isPressing = false;
    _originX = 0;
    _originY = 0;
    _pts = QList<QPointF>();
}

void MyPixmapItem::on_timer_timeout()
{
//    if (!_isMoving){
//        _pts.append(_mousePressPos);
//        QPointF scenePos = mapToScene(_mousePressPos) - QPointF(_originX, _originY);
//        // qDebug() << "Image coordinate: " << scenePos;

//        QList<int> pt;
//        pt.append(int(scenePos.x()));
//        pt.append(int(scenePos.y()));
//        _ptsImage.append(pt);

//        this->update();
//        _parent->updateText(_ptsImage);
//    }
}

void MyPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
    this->setFocus();
//    _originX = this->pos().x();
//    _originY = this->pos().y();

//    _mousePressPos = event->pos();
//    if (event->button() == Qt::LeftButton && _clickable) {
//        _isPressing = true;

////        _parent->_timer->start(50);
//        on_timer_timeout();
//    }
//    else if (event->button() == Qt::RightButton && _clickable) {
//        if (_pts.count() > 0){
//            _pts.removeLast();
//            _ptsImage.removeLast();
//        }

//        this->update();
//        _parent->updateText(_ptsImage);
//    }
    this->setCursor(Qt::ClosedHandCursor);

    QGraphicsPixmapItem::mousePressEvent(event);
}

void MyPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();

    _isPressing = false;
    this->setCursor(Qt::ArrowCursor);

    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void MyPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();

//    if (_isPressing) {
//        QPointF gap = event->scenePos() - _mousePressPos;
//        if (gap.x() > 0.001 || gap.y() > 0.001){
//            _isMoving = true;

//            qDebug() << "move: " << gap;
//            this->setPos(_originX + gap.x(), _originY + gap.y());
//            this->setCursor(Qt::SizeAllCursor);
//        }
//    }

    QGraphicsPixmapItem::mouseMoveEvent(event);
}

void MyPixmapItem::setZoomStep(bool isUp, float ratio, float& zoom_step, float& zoom_min, float& zoom_max)
{
    if (isUp)
    {
        if (ratio >= 1)
            zoom_step = 0.2;
        else if (ratio >= 0.1 && ratio < 1)
            zoom_step = 0.1;
        else
            zoom_step = 0.01;
    }
    else
    {
        if (ratio > 1)
            zoom_step = 0.2;
        else if (ratio > 0.1 && ratio <= 1)
            zoom_step = 0.1;
        else
            zoom_step = 0.01;
    }

    if (this->pixmap().width() > 10000 || this->pixmap().height() > 10000)
        zoom_min = 0.01;
    else
        zoom_min = 0.05;

    zoom_max = 10;
}

void MyPixmapItem::Zoom(int angle, int x, int y, const QPointF pos)
{
    ratio = this->scale();
    if (angle > 0)
    {
        float zoom_step, zoom_min, zoom_max;
        setZoomStep(true, ratio, zoom_step, zoom_min, zoom_max);
        ratio += zoom_step;
        if (ratio > zoom_max)
            ratio = zoom_max;
        else{
            float w = this->pixmap().width() * (ratio - zoom_step);
            float h = this->pixmap().height() * (ratio - zoom_step);
            float x1 = this->pos().x();
            float x2 = this->pos().x() + w;
            float y1 = this->pos().y();
            float y2 = this->pos().y() + h;

            this->setScale(ratio);
            if (x > x1 && x < x2 && y > y1 && y < y2){
                QPointF a1 = pos - this->pos();
                float a2 = ratio / (ratio - zoom_step) - 1;
                QPointF delta = a1 * a2;
                this->setPos(this->pos() - delta);
            }
            else{
                this->setPos(0, 0);
            }
        }
    }
    else
    {
        float zoom_step, zoom_min, zoom_max;
        setZoomStep(false, ratio, zoom_step, zoom_min, zoom_max);
        ratio -= zoom_step;
        if (ratio < zoom_min)
            ratio = zoom_min;
        else
        {
            float w = this->pixmap().width() * (ratio + zoom_step);
            float h = this->pixmap().height() * (ratio + zoom_step);
            float x1 = this->pos().x();
            float x2 = this->pos().x() + w;
            float y1 = this->pos().y();
            float y2 = this->pos().y() + h;
            this->setScale(ratio);
            if (x > x1 && x < x2 && y > y1 && y < y2){
                QPointF a1 = pos - this->pos();
                float a2 = ratio / (ratio + zoom_step) - 1;
                QPointF delta = a1 * a2;
                this->setPos(this->pos() - delta);
            }
            else{
                this->setPos(0, 0);
            }
        }
    }
}

void MyPixmapItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    event->accept();

    Zoom(event->delta(), event->scenePos().x(), event->scenePos().y(), event->scenePos());

    QGraphicsPixmapItem::wheelEvent(event);
}


QPointF MyPixmapItem::mapToImage(const QPoint& pos)
{
    QPointF scenePos = mapToScene(pos);
//    QPointF itemPos = scenePos / scaleFactor;
    return scenePos;
}

QRectF MyPixmapItem::boundingRect() const
{
    return QRectF(0, 0, this->pixmap().width(), this->pixmap().height());
}

//////
void MyPixmapItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawPixmap(this->pixmap().rect(), this->pixmap());

    if (_pts.count() > 0){
//        painter->setPen(QPen(QColor(255, 0, 0), 2));
//        painter->setBrush(QBrush(QColor(255, 0, 0)));
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::red);

        for(QPointF pt: _pts)
            painter->drawEllipse(pt, 5, 5);
    }

//    QGraphicsPixmapItem::paint(painter, option, widget);
}



/////////////////////////////
MyGraphicsView::MyGraphicsView(QWidget* parent, bool clickable):QGraphicsView(parent)
{
    _clickable = clickable;
    this->setParent(parent);
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setSceneRect(0, 0, this->viewport()->width(), this->viewport()->height());

    this->setCacheMode(QGraphicsView::CacheBackground);
    this->setRenderHint(QPainter::Antialiasing);
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    this->setAcceptDrops(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setMouseTracking(true);
//    this->setDragMode(QGraphicsView::ScrollHandDrag);

    _scene = new QGraphicsScene(this);
    _scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->setScene(_scene);

    _pixmapItem = NULL;
    _sceneW = this->rect().width();
    _sceneH = this->rect().height();
    _originX = 0;
    _originY = 0;
    _ratio = 1;
    imgW_resize = 1;
    imgH_resize = 1;

    _crossItemX = NULL;
    _crossItemY = NULL;
    _isSetROI = false;

    _rectItem = NULL;
    _textItem = NULL;
    _isDrawingRect = false;
    _ptStart = QPointF(0,0);
//    _timer = new QTimer();
}

void MyGraphicsView::resizeEvent(QResizeEvent* event)
{
    _sceneW = this->rect().width();
    _sceneH = this->rect().height();
}

void MyGraphicsView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    }
}

void MyGraphicsView::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void MyGraphicsView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()){
        QList<QUrl> urlList = event->mimeData()->urls();
        for (QUrl& url: urlList)
        {
            QString filePath = url.toLocalFile();
            filePath = filePath.replace(_mapDir, "/workspace");

            this->addItem(filePath);
        }
    }
}

////
void MyGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && _isSetROI)
    {
        // 获取鼠标按下的位置
        QPointF mousePos = this->mapToScene(event->pos());
        _ptStart = QPointF((mousePos.x() - _originX) / _ratio, (mousePos.y() - _originY) / _ratio);

        _isDrawingRect = true;
    }

    QGraphicsView::mousePressEvent(event);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (_isDrawingRect && _isSetROI)
    {
        _isDrawingRect = false;

        if (_rectItem){
            QString text = QString("(%1, %2, %3, %4)")
                    .arg(int(_rectItem->rect().x())).arg(int(_rectItem->rect().y())).arg(int(_rectItem->rect().width())).arg(int(_rectItem->rect().height()));
            emit_updateText(text);
        }
//        if (_textItem){
//            _textItem->setPos(5, 5);
//        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (_isSetROI && _isSetROI){
        QPointF mousePos = this->mapToScene(event->pos());

        _originX = _pixmapItem->pos().x();
        _originY = _pixmapItem->pos().y();
        imgW_resize = _pixmapItem->pixmap().rect().width();
        imgH_resize = _pixmapItem->pixmap().rect().height();

        qreal x = (mousePos.x() - _originX) / _ratio;
        qreal y = (mousePos.y() - _originY) / _ratio;

        if (_isDrawingRect){
            // 计算矩形的大小
            qreal width = qAbs(x - _ptStart.x());
            qreal height = qAbs(y - _ptStart.y());

            // 更新矩形的位置和大小
            if (_rectItem)
                _rectItem->setRect(QRectF(_ptStart, QSizeF(width, height)));
        }
        if (_crossItemX)
            _crossItemX->setLine(0, y, imgW_resize / _ratio, y);
        if (_crossItemY)
            _crossItemY->setLine(x, 0, x, imgH_resize / _ratio);
    }

    QGraphicsView::mouseMoveEvent(event);
}

void MyGraphicsView::getNewShowInfo(const QPixmap& pix, int sceneW, int sceneH, float& originX, float& originY, float& imgW_resize, float& imgH_resize, float& ratio)
{
    int imgW = pix.width();
    int imgH = pix.height();

    // 避免初始显示图像出错
    if (sceneH <= 0){
        // 设置图像左上角显示的坐标基点（左上角对准）
        if (sceneW > imgW)
            originX = sceneW / 2 - imgW / 2;
        else
            originX = 0;

        if (sceneH > imgH)
            originY = sceneH / 2 - imgH / 2;
        else
            originY = 0;

        ratio = 1.0;
        imgW_resize = imgW;
        imgH_resize = imgH;
    }
    else{
        // 图像自适应，并居中
        resizeImg(imgW, imgH, sceneW, sceneH, originX, originY, imgW_resize, imgH_resize, ratio);

    }
}

void MyGraphicsView::resizeImg(int imgW, int imgH, int sceneW, int sceneH, float& originX, float& originY, float& imgW_resize, float& imgH_resize, float& ratio)
{
    if (imgW / imgH > sceneW / sceneH)
    {
        if (imgH > sceneH) {
            imgW_resize = sceneW;
            imgH_resize = imgH * imgW_resize / imgW;
            originX = 0;
            originY = sceneH / 2 - imgH_resize / 2;
            ratio = imgW_resize / imgW;
        }
        else{
            if (imgW > sceneW){
                imgW_resize = sceneW;
                imgH_resize = imgH * imgW_resize / imgW;
                originX = 0;
                originY = sceneH / 2 - imgH_resize / 2;
                ratio = imgW_resize / imgW;
            }
            else{
                imgW_resize = imgW;
                imgH_resize = imgH;
                originX = sceneW / 2 - imgW_resize / 2;
                originY = sceneH / 2 - imgH_resize / 2;
                ratio = 1;
            }
        }
    }
    else
    {
        if (imgW > sceneW)
        {
            imgH_resize = sceneH;
            imgW_resize = imgW * imgH_resize / imgH;
            originY = 0;
            originX = sceneW / 2 - imgW_resize / 2;
            ratio = imgW_resize / imgW;
        }
        else
        {
            if (imgH > sceneH)
            {
                imgH_resize = sceneH;
                imgW_resize = imgW * imgH_resize / imgH;
                originY = 0;
                originX = sceneW / 2 - imgW_resize / 2;
                ratio = imgW_resize / imgW;
            }
            else
            {
                imgW_resize = imgW;
                imgH_resize = imgH;
                originX = sceneW / 2 - imgW_resize / 2;
                originY = sceneH / 2 - imgH_resize / 2;
                ratio = 1;
            }
        }
    }
}

void MyGraphicsView::on_timer_timeout()
{
//    _timer->stop();
//    for(QGraphicsItem* item: _scene->items())
//    {
//        MyPixmapItem* pixItem = qgraphicsitem_cast<MyPixmapItem*>(item);
//        if (pixItem){
//            pixItem->on_timer_timeout();
//        }
//    }
}

void MyGraphicsView::addCrossLine()
{
    if (!_crossItemX){
        _crossItemX = new QGraphicsLineItem(_pixmapItem);
        _crossItemX->setPen(QPen(Qt::red, 2, Qt::DashLine));
        _crossItemX->setZValue(10);
        _scene->addItem(_crossItemX);
    }
    if (!_crossItemY){
        _crossItemY = new QGraphicsLineItem(_pixmapItem);
        _crossItemY->setPen(QPen(Qt::red, 2, Qt::DashLine));
        _crossItemY->setZValue(10);
        _scene->addItem(_crossItemY);
    }
    // 创建矩形项并添加到场景中
    if (!_rectItem){
        _rectItem = new QGraphicsRectItem(_pixmapItem);
        _rectItem->setPen(QPen(Qt::green, 2, Qt::SolidLine)); // 设置矩形的笔
        _rectItem->setZValue(10);
        _scene->addItem(_rectItem);
    }

    _crossItemX->show();
    _crossItemY->show();
    _rectItem->show();
}

//////
void MyGraphicsView::addItem(QString imagePath)
{
    for (QGraphicsItem* item: _scene->items()){
        _scene->removeItem(item);
    }
    _scene->clear();
    _crossItemX = NULL;
    _crossItemY = NULL;
    _pixmapItem = NULL;
    _rectItem = NULL;
    _textItem = NULL;

    QPixmap pixmap(imagePath);

    getNewShowInfo(pixmap, _sceneW, _sceneH, _originX, _originY, imgW_resize, imgH_resize, _ratio);
    printf("##############################\n");
    printf("  image width: %d\n", pixmap.width());
    printf("  image height: %d\n", pixmap.height());
    printf("  image resize ox: %f\n", _originX);
    printf("  image resize oy: %f\n", _originY);
    printf("  image resize width: %f\n", imgW_resize);
    printf("  image resize height: %f\n", imgH_resize);
    printf("  image resize ratio: %f\n", _ratio);
    printf("##############################\n");

    _pixmapItem = new MyPixmapItem(this, pixmap, _clickable);
//    connect(_timer, &QTimer::timeout, this, &MyGraphicsView::on_timer_timeout);
    _pixmapItem->ratio = _ratio;
    _pixmapItem->setPos(_originX, _originY);
    _pixmapItem->setScale(_ratio);
    _pixmapItem->setZValue(1);
    _scene->addItem(_pixmapItem);
//    emit emit_imageWH(pixmap.width(), pixmap.height());

    if (!_textItem){
        _textItem = new QGraphicsTextItem();
        QString text = QString(" w: %1, h: %2").arg(pixmap.width()).arg(pixmap.height());
        _textItem->setFont(QFont("SimSun", 16, Qt::red));
        _textItem->setPlainText(text);
        _textItem->setToolTip(text);
        _textItem->setPos(5, 5);
        _textItem->setZValue(10);
        _scene->addItem(_textItem);
    }

    this->fitInView(_pixmapItem, Qt::KeepAspectRatio);
}

void MyGraphicsView::open_pixmap_crossLine(bool isOpened)
{
    _isSetROI = isOpened;
    if (isOpened){
        _pixmapItem->setFlags(QGraphicsItem::ItemIsFocusable);
        this->addCrossLine();
    }
    else{
        if (_pixmapItem){
            _pixmapItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
            if (_crossItemX)
                _crossItemX->hide();
            if (_crossItemY)
                _crossItemY->hide();
            if (_rectItem){
                _rectItem->setRect(QRectF(0,0,0,0));
                _rectItem->hide();
            }
        }
    }
}

//////
void MyGraphicsView::updateText(QList<QList<int>> pts)
{
    QString text = "";
    for(QList<int> pt: pts)
    {
        text += QString("(%1, %2),").arg(pt[0]).arg(pt[1]);
    }
    emit_updateText(text);
}


/////////////////////////
AlgoConfig::AlgoConfig(QWidget*parent):QWidget(parent)
{
    this->setParent(parent);
    this->setWindowTitle("内环面设置窗口");
    this->setWindowFlags(Qt::Tool);
    this->setGeometry(200, 200, 960, 720);
    this->setStyleSheet("background-color:rgb(30,41,54)");

    init_UI();
}

AlgoConfig::~AlgoConfig()
{

}

void AlgoConfig::init_UI()
{
    int ww = 100;
    int hh = 40;
    _buttonOpen = newToolButton(this, "打开图片", "打开新的内环面图片", ww, hh);
    connect(_buttonOpen, &QToolButton::clicked, this, &AlgoConfig::on_buttonOpen_clicked);

    _buttonYes = newToolButton(this, "发送配置", "", ww, hh);
    connect(_buttonYes, &QToolButton::clicked, this, &AlgoConfig::on_buttonYes_clicked);

    _textEdit = newTextEdit(this, -1, hh);
    _textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _textEdit->setStyleSheet("border:1px solid rgb(120, 120, 120); color: white");

    QWidget* temp1 = new QWidget(this);
    temp1->setMinimumWidth(200);
    temp1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QWidget* w1 = new QWidget(this);
    newHBoxLayout(w1, {_buttonOpen, temp1, _buttonYes, _textEdit}, 0, 10);

    _view = new MyGraphicsView(this, true);
    _view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(_view, &MyGraphicsView::emit_updateText, this, &AlgoConfig::on_textEdit_updated);

    newVBoxLayout(this, {w1, _view}, 0, 10, {5, 5, 5, 5});
}

void AlgoConfig::on_buttonOpen_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.bmp *.jpeg)");
    if (imagePath.isEmpty()) {
        return;
    }

    _view->addItem(imagePath);
}

void AlgoConfig::on_buttonYes_clicked()
{

}

//////
void AlgoConfig::on_textEdit_updated(QString text)
{
    _textEdit->clear();
    _textEdit->setText(text);
    _textEdit->setToolTip(text);
}





