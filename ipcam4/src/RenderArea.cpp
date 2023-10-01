#include <QPainter>
#include <QImage>
#include "RenderArea.h"

using namespace std;

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(false);
    //setMinimumHeight(480);
    setMinimumSize(640,480);
    setMaximumSize(1980,1600);

    bool isEnabled=true;
    refresh_rate.start();
    _isEnabled.storeRelease(&isEnabled);
}

/* painter.drawImage runs only after refresh expired */
void RenderArea::paintEvent(QPaintEvent *e)
{
    painter.begin(this);
    painter.drawImage(QPoint(0, 0), frame);
    //painter.fillRect(0, 0,640,480, Qt::green);
    painter.end();
}

// Signal slots
void RenderArea::setImage(QImage q)
{
    frame = q;
    int width = q.size().width();
    int height = q.size().height();
    int framerate = 24;
    int expiretime = int(1000/framerate);

    if (_isEnabled.loadAcquire()){
        if(refresh_rate.hasExpired(expiretime))
        {
            bool isEnabled = 0;
            _isEnabled.storeRelease( &isEnabled );
            refresh_rate.restart();
                this->repaint();
        }
    }
    else
    {
        if(refresh_rate.hasExpired(25))
        {
            bool isEnabled=1;
            _isEnabled.storeRelease( &isEnabled );
            refresh_rate.restart();
        }
    }
}

void RenderArea::Enable(bool c)
{
    _isEnabled.storeRelease(&c);
    qDebug()<<"RenderArea enabled";
}

// Disposals
RenderArea::~RenderArea(){ delete this;}
