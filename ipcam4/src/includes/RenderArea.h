#ifndef RENDERAREA_H_
#define RENDERAREA_H_

#include <QtCore>
#include <QObject>
#include <QImage>
#include <QWidget>
#include <QPainter>

class RenderArea : public QWidget
{
    Q_OBJECT
    Q_COMPILER_THREAD_LOCAL

public:
    explicit RenderArea(QWidget *parent = nullptr);
    ~RenderArea();

    public slots:
        void setImage(QImage);
        void Enable(bool);

    protected:
        void paintEvent(QPaintEvent*);
        QElapsedTimer refresh_rate;

    private:
        QAtomicPointer<bool> _isEnabled;
        QImage frame;
        QPainter painter;
};

#endif
