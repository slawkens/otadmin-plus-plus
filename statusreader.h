#ifndef STATUSREADER_H
#define STATUSREADER_H

#include <QDebug>
#include <QMap>
#include <QXmlSimpleReader>

typedef QMap<QString, QString> StatusInfo_t;

class StatusReader : public QXmlDefaultHandler
{
	public:
		StatusReader()
		{
			statusInfo["uptime"] = "";
			//statusInfo["ip"] = "";
			statusInfo["servername"] = "";
			//statusInfo["port"] = "";
			statusInfo["location"] = "";
			statusInfo["url"] = "";
			statusInfo["server"] = "";
			statusInfo["version"] = "";
			statusInfo["client"] = "";

			statusInfo["name"] = "";
			statusInfo["email"] = "";

			statusInfo["online"] = "";
			statusInfo["max"] = "";
			statusInfo["peak"] = "";
		}

		bool startDocument() {return true;}
		bool endElement(const QString&, const QString&, const QString &name) {return true;}

		bool startElement(const QString&, const QString&, const QString& name, const QXmlAttributes& attrs)
		{
			for(int i = 0; i < attrs.count(); i++)
			{
				if(statusInfo.find(attrs.localName(i)) != statusInfo.end())
					statusInfo[attrs.localName(i)] = attrs.value(i);
			}
			return true;
		}

		StatusInfo_t getStatusInfo() {return statusInfo;}

	private:
		StatusInfo_t statusInfo;
};

#endif
