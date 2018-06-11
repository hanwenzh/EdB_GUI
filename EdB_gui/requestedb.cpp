#include "requestedb.h"
#include <QtNetwork>
#include <QProcess>
#include "requestlogin.h"
#include <QMutex>
#include <QFile>

QMutex mutexControl;	//线程锁的锁，用于保证线程锁的安全创建


RequestEdB::RequestEdB(UserOperateWidget *widget, QString cmd, QList<QVariant> args)
{
	m_userOperateWidget = widget;
	m_args = args;
	m_cmd = cmd;
}

void RequestEdB::run()
{
	mutexControl.lock();
	QString guid = m_userOperateWidget->m_loginInfo.coreGuid;

	QLocalSocket localsocket;
	localsocket.connectToServer(QString("EdBRunServer_") + guid);
	bool isConnented = false;
	isConnented = localsocket.waitForConnected(5000);
	if (!isConnented)
	{
		QEventLoop loginLoop;
		RequestLogin loginRequest(m_userOperateWidget->m_loginInfo, NULL);
		QObject::connect(&loginRequest, &RequestLogin::finished, [&]() {loginLoop.quit(); });
		loginRequest.start();
		loginLoop.exec();
		m_userOperateWidget->m_loginInfo = loginRequest.getResult();
		
		localsocket.connectToServer(QString("EdBRunServer_") + m_userOperateWidget->m_loginInfo.coreGuid);
		isConnented = localsocket.waitForConnected();
		
		if (!isConnented)
		{
			mutexControl.unlock();
			return;
		}

	}

	if (m_cmd == "QueryData")
	{
		QStringList sendStrList;
		sendStrList << "EdB_QueryData" << m_args[1].toString() << QString::number(m_userOperateWidget->m_loginInfo.clientID) << QString::number(m_args[3].toInt());
		QString sendStr = sendStrList.join("&&&");
		localsocket.write(sendStr.toStdString().c_str());
		localsocket.flush();
		bool isRead = localsocket.waitForReadyRead(10000);
		if (!isRead)
		{
			m_userOperateWidget->m_queryMutex.lock();
			m_userOperateWidget->m_queryTableCache[m_args[0].toString()] = QString::fromStdWString(L"查询中断，未经处理的异常");
			m_userOperateWidget->m_queryMutex.unlock();
			mutexControl.unlock();
			return;
		}
		QDataStream stream(&localsocket);
		QByteArray readData = localsocket.readAll();

		m_userOperateWidget->m_queryMutex.lock();
		m_userOperateWidget->m_queryTableCache[m_args[0].toString()] = QString::fromLocal8Bit(readData);
		m_userOperateWidget->m_queryMutex.unlock();
	}
	else if (m_cmd == "GetPrice")
	{
		QStringList sendStrList;
		sendStrList << "EdB_GetPrice" << QString::number(m_userOperateWidget->m_loginInfo.clientID) << m_args[2].toString();
		QString sendStr = sendStrList.join("&&&");
		localsocket.write(sendStr.toStdString().c_str());
		localsocket.flush();
		bool isRead = localsocket.waitForReadyRead(10000);
		if (!isRead)
		{
			m_userOperateWidget->m_getPriceMutex.lock();
			m_userOperateWidget->m_getPriceCache[m_args[0].toString()] = QString::fromStdWString(L"查询中断，未经处理的异常");
			m_userOperateWidget->m_getPriceMutex.unlock();
			mutexControl.unlock();
			return;
		}
		QDataStream stream(&localsocket);
		QByteArray readData = localsocket.readAll();
		m_userOperateWidget->m_getPriceCache[m_args[0].toString()] = QString::fromLocal8Bit(readData);
	}
	else if (m_cmd == "SendOrder")
	{
		QStringList sendStrList;
		sendStrList << "EdB_SendOrder" << QString::number(m_userOperateWidget->m_loginInfo.clientID) << QString::number(m_args[2].toInt()) << m_args[3].toString() << m_args[4].toString() << m_args[5].toString() << QString::number(m_args[6].toInt()) << QString::number(m_args[7].toFloat());
		QString sendStr = sendStrList.join("&&&");
		localsocket.write(sendStr.toStdString().c_str());
		localsocket.flush();
		bool isRead = localsocket.waitForReadyRead(10000);
		if (!isRead)
		{
			mutexControl.unlock();
			return;
		}

		QDataStream stream(&localsocket);
		QByteArray readData = localsocket.readAll();
		QString readDataStr = QString::fromLocal8Bit(readData);
		if (readDataStr.toInt() != 0)
		{
			m_userOperateWidget->m_sendOrderMutex.lock();
			if (m_userOperateWidget->m_sendOrderCache.contains(m_args[1].toString()))
			{
				QThread *pThread = m_userOperateWidget->m_sendOrderCache[m_args[1].toString()];
				m_userOperateWidget->m_sendOrderCache.remove(m_args[1].toString());
				if (pThread->isRunning())
					pThread->quit();
				delete pThread;
			}
			m_userOperateWidget->m_sendOrderMutex.unlock();
		}

		localsocket.disconnectFromServer();
	}
	else if (m_cmd == "CancelOrder")
	{
		QStringList sendStrList;
		sendStrList << "EdB_CancelOrder" << QString::number(m_userOperateWidget->m_loginInfo.clientID) << m_args[1].toString() << m_args[2].toString() << QString::number(m_args[3].toInt());
		QString sendStr = sendStrList.join("&&&");
		localsocket.write(sendStr.toStdString().c_str());
		localsocket.flush();
		bool isRead = localsocket.waitForReadyRead(20000);
		if (!isRead)
		{
			mutexControl.unlock();
			return;
		}

		QDataStream stream(&localsocket);
		QByteArray readData = localsocket.readAll();
		QString readDataStr = QString::fromLocal8Bit(readData);
		if (readDataStr.toInt() != 0)
		{
			m_userOperateWidget->m_cancelOrderMutex.lock();
			if (m_userOperateWidget->m_cancelOrderCache.contains(m_args[1].toString()))
			{
				QThread *pThread = m_userOperateWidget->m_cancelOrderCache[m_args[1].toString()];
				m_userOperateWidget->m_cancelOrderCache.remove(m_args[1].toString());
				if (pThread->isRunning())
					pThread->quit();
				delete pThread;
			}
			m_userOperateWidget->m_cancelOrderMutex.unlock();
		}
	}
	localsocket.disconnectFromServer();

	mutexControl.unlock();


}
