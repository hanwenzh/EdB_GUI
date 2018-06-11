#pragma once

#include <QObject>
#include <QtNetwork>

class EdBRunServer : public QObject
{
	Q_OBJECT

public:
	EdBRunServer(QString serverName);
	~EdBRunServer();

	void RunServer();

private slots:
	void serverNewConnectionHandler();
	void socketReadyReadHandler();

private:
	QLocalServer * m_server;
	QString m_serverName;
};
