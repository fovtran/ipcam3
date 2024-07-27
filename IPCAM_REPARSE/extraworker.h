#ifndef EXTRAWORKER_H
#define EXTRAWORKER_H

#include <QObject>
#include <QThread>

class Worker : public QThread {
    Q_OBJECT
public:
    Worker();
    ~Worker();
public slots:
    void run();
signals:
    void finished();
    void error(QString err);
private:
    // add your variables here
};

#endif // EXTRAWORKER_H
