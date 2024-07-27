#include <QEvent>
#include "extraworker.h"

Worker::Worker() {}
Worker::~Worker() {}

void Worker::run() {
    // allocate resources using new here
    qDebug("Hello World!");
    emit finished();
}

class UserInputEater : public QObject
{
public:
    bool eventFilter(QObject *object, QEvent *event)
    {
        switch(event->type())
        {
        case QEvent::UpdateRequest:
        case QEvent::UpdateLater:
        case QEvent::Paint:
            return QObject::eventFilter(object, event);
        default:
            return true;
        }
    }
};
