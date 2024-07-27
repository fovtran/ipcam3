#ifndef CONFIGREAD_H_
#define CONFIGREAD_H_

#include <QtCore>
#include <QString>
#include <QObject>

class XmlConfigReader : public QObject
{
    Q_OBJECT

    public:
        explicit XmlConfigReader(QObject *parent=nullptr);
        ~XmlConfigReader();
        void read();
        void ReadConfigFile(const QString configname);
        QString filename;
        QString ipaddress;
        QString url;
        QList<QMap<QString,QString>> iplist;

    private:
        QString errorString;
        QFile *xmlFile;

        QString _filename;
        QXmlStreamReader xml;
};

#endif
