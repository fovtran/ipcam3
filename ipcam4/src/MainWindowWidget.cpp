#include <QDebug>
#include <QObject>
#include <QImage>
#include <QPushButton>
#include <QVBoxLayout>
#include <qendian.h>
#include "MainWindowWidget.h"

MainWindoWidget::MainWindoWidget(QObject*) : renderArea(nullptr)
{
    //connect(this, SIGNAL(Enabled(bool)), renderArea, SLOT(_setVisible(bool)), Qt::DirectConnection);
    setMinimumSize(320,200);
    resize(768,640);
    setMaximumSize(1980,1600);

    QWidget *e = new QWidget(this);
    //e->move(10,10);
    e->resize(768, 768);
    e->show();
    QScopedPointer<QVBoxLayout> layout1(new QVBoxLayout(e));
    renderArea = new RenderArea();
    renderArea->resize(640,480);
    renderArea->move(0,32);

    QPushButton *b1 = new QPushButton();
    b1->setMaximumSize(64,32);
    b1->setText("Whatev");
    b1->show();

    layout1->addWidget(b1);
    layout1->addWidget(renderArea);
    e->setLayout(layout1.data());
    //layout1.take();
    //e->setLayout(layout1.data());
    e->show();
}

void MainWindoWidget::run()
{
}

// Signal Slots
void MainWindoWidget::setImage(QImage qim)
{
    renderArea->setImage(qim);
}

void MainWindoWidget::setSize(int x, int y){}

void MainWindoWidget::moveEvent(QMoveEvent *)
{
    QRect x = this->geometry();
    qDebug() << "moved window" << x.x() << "" << x.y();
}
void MainWindoWidget::resizeEvent(QResizeEvent *)
{
    QRect x = this->geometry();
    qDebug() << "resized window" << x.x() << "" << x.y();
}
// Disposals
MainWindoWidget::~MainWindoWidget() { delete this; }
