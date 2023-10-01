#include <QDebug>
#include <QtCore>
#include <QString>
#include <QObject>
#include <QtNetwork>
#include <QMessageBox>
#include "XmlConfigReader.h"

XmlConfigReader::XmlConfigReader(QObject *parent) : QObject(parent) {}
XmlConfigReader::~XmlConfigReader(){ delete this; }

void XmlConfigReader::ReadConfigFile(const QString configname)
{
    QFileInfo fi("temp");
    QDir this_path = QDir::currentPath();
    qDebug() << "This path: " << this_path.path();

    QDir application_path = QCoreApplication::applicationDirPath();
    qDebug() << "Application path: " << application_path.path();
    
    filename = application_path.path() + QDir::separator() + configname;

    QDir homedir(QDir::homePath());
    QDir localappdir(homedir.path() + "/.local/qtipcam");

    QFile targetconf(localappdir.path() + QDir::separator() +configname);
    QFile sourceconf(filename);

    if(targetconf.exists())
    {
        filename = targetconf.fileName();
    }
    else
    {
        if(!localappdir.exists())
        {
            qDebug() << "Dropping configuration to .local";
            localappdir.mkpath("./config");
            sourceconf.copy(targetconf.fileName());
            filename = targetconf.fileName();
        }
        filename = application_path.path() + QDir::separator() + configname;
    }
        

    read();
}

/* Parses xml configuration file */
void XmlConfigReader::read()
{
    QFile xmlFile(filename);
        if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qDebug() << "Error reading xml";
                xmlFile.close();
                exit(0);
            }

    QXmlStreamReader xml(&xmlFile);

    // Parses the xml stream
    if(xml.readNextStartElement()) {
        if (xml.name() == "config")
        {
                while (xml.readNextStartElement())
                {
                    if(xml.name() == "ip")
                        ipaddress = xml.readElementText();
                    else if(xml.name() == "url")
                        url = xml.readElementText();
                    else
                        xml.skipCurrentElement();
                }
        }
    }
    xmlFile.close();

    if (xml.hasError())
    {
        qDebug() << "XML error: " << xml.errorString().data();
    }
}
