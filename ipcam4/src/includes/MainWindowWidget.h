#ifndef VIDEOCANVAS_H_
#define VIDEOCANVAS_H_

#include <QByteArray>
#include <QMainWindow>
#include <QObject>
#include <QImage>

#include "RenderArea.h"

class MainWindoWidget : public QMainWindow, public QRunnable
{
    Q_OBJECT

    public:
        explicit MainWindoWidget(QObject *parent = nullptr);
        ~MainWindoWidget();
        void setImage(QImage);
        void setSize(int,int);

    private:
        void run();

    signals:
        //void setVisible(bool);

    private slots:
        void resizeEvent(QResizeEvent*);
        void moveEvent(QMoveEvent*);
        
    protected:

    private:
        RenderArea* renderArea;
};

#endif
