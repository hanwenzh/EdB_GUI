#include "requestlogin.h"
#include <QtNetwork>
#include <QProcess>
#include <QFile>
#include <QUuid>
#include <QStringList>

RequestLogin::RequestLogin(RequestLogin::LoginInfo loginInfo ,QObject *parent)
	: QThread(parent)
{
	m_loginInfo = loginInfo;
	if (m_loginInfo.coreGuid.isEmpty())
		m_loginInfo.coreGuid = QUuid::createUuid().toString();
}

RequestLogin::~RequestLogin()
{
} 

void RequestLogin::run()
{
	QLocalSocket localsocket;
	localsocket.connectToServer(QString("EdBRunServer_") + m_loginInfo.coreGuid);
	bool isConnented = false;
	isConnented = localsocket.waitForConnected(5000);
	if (!isConnented)
	{
		QProcess *process = new QProcess();
		//process->execute("taskkill", QStringList() << "/im" << "EdB_Core.exe" << "/f");
		if (!QFile::exists(qApp->applicationDirPath() + "/EdB_Core.exe"))
		{
			qApp->quit();
		}
		QFile::copy(qApp->applicationDirPath() + "/EdB_Core.exe", qApp->applicationDirPath() + "/EdB_Core_" + m_loginInfo.coreGuid + ".exe");
		process->startDetached(qApp->applicationDirPath() + "/EdB_Core_" + m_loginInfo.coreGuid + ".exe", QStringList() << QString("EdBRunServer_") + m_loginInfo.coreGuid);

		while (!isConnented)
		{
			localsocket.connectToServer(QString("EdBRunServer_") + m_loginInfo.coreGuid);
			isConnented = localsocket.waitForConnected();
		}
	}

	QStringList sendStrList;
	sendStrList << "EdB_Login" << m_loginInfo.ip << QString::number(m_loginInfo.port) << m_loginInfo.version << m_loginInfo.user << m_loginInfo.password << m_loginInfo.txpass << m_loginInfo.yyb;
	QString sendStr = sendStrList.join("&&&");
	localsocket.write(sendStr.toStdString().c_str());
	localsocket.flush();
	bool isRead = localsocket.waitForReadyRead(10000);
	if (!isRead)
	{
		m_loginInfo.clientID = -104;
		return;
	}

	QTextStream stream(&localsocket);
	QString respond = stream.readAll();
	m_loginInfo.clientID = respond.toInt();
	localsocket.disconnectFromServer();

	//m_loginInfo.clientID = EdB_Login(
	//	m_loginInfo.ip.toStdString().c_str(),
	//	m_loginInfo.port,
	//	m_loginInfo.version.toStdString().c_str(),
	//	m_loginInfo.user.toStdString().c_str(),
	//	m_loginInfo.password.toStdString().c_str(),
	//	m_loginInfo.txpass.toStdString().c_str(),
	//	m_loginInfo.yyb.toStdString().c_str()
	//);
	//m_loginInfo.clientID = 1;
}
