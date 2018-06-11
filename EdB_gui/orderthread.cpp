#include "orderthread.h"
#include <QDebug>
#include <QtNetwork>
#include <QProcess>
#include <QThreadPool>
#include "requestedb.h"


SendOrderThread::SendOrderThread(SendOrderInfo sendOrderInfo, UserOperateWidget *widget, int interval)
	: QThread(widget)
{
	m_sendOrderInfo = sendOrderInfo;
	m_userOperateWidget = widget;
	m_interval = interval;
}

SendOrderThread::~SendOrderThread()
{
}

void SendOrderThread::run()
{
	QList<QVariant> sendData;
	sendData.append(m_sendOrderInfo.gpdm);
	sendData.append(m_sendOrderInfo.clientID);
	sendData.append(m_sendOrderInfo.fx);
	sendData.append(m_sendOrderInfo.user);
	sendData.append(m_sendOrderInfo.gddm);
	sendData.append(m_sendOrderInfo.gpdm);
	sendData.append(m_sendOrderInfo.quantity);
	sendData.append(m_sendOrderInfo.price);
	RequestEdB *request = new RequestEdB(m_userOperateWidget, "SendOrder", sendData);
	QThreadPool::globalInstance()->start(request);

	if (m_interval != 0)
	{
		while (true)
		{
			msleep(m_interval);
			RequestEdB *request = new RequestEdB(m_userOperateWidget, "SendOrder", sendData);
			QThreadPool::globalInstance()->start(request);
		}
	}

	/*QString ret = EdB_SendOrder(
		m_sendOrderInfo.clientID,
		m_sendOrderInfo.fx,
		m_sendOrderInfo.user.toStdString().c_str(),
		m_sendOrderInfo.gddm.toStdString().c_str(),
		m_sendOrderInfo.gpdm.toStdString().c_str(),
		m_sendOrderInfo.quantity,
		m_sendOrderInfo.price);
	emit  sendOrderRet(ret);
	if (m_interval != 0)
	{
		while (true)
		{
			msleep(m_interval);
			ret = EdB_SendOrder(
				m_sendOrderInfo.clientID,
				m_sendOrderInfo.fx,
				m_sendOrderInfo.user.toStdString().c_str(),
				m_sendOrderInfo.gddm.toStdString().c_str(),
				m_sendOrderInfo.gpdm.toStdString().c_str(),
				m_sendOrderInfo.quantity,
				m_sendOrderInfo.price);
			emit sendOrderRet(ret); 
		}
	}*/
}


CancelOrderThread::CancelOrderThread(CancelOrderInfo cancelOrderInfo, UserOperateWidget *widget, int interval)
	: QThread(widget)
{
	m_cancelOrderInfo = cancelOrderInfo;
	m_userOperateWidget = widget;
	m_interval = interval;
}

CancelOrderThread::~CancelOrderThread()
{
}

void CancelOrderThread::run()
{
	QList<QVariant> sendData;
	sendData.append(m_cancelOrderInfo.bho);
	sendData.append(m_cancelOrderInfo.clientID);
	sendData.append(m_cancelOrderInfo.user);
	sendData.append(m_cancelOrderInfo.bho);
	sendData.append(m_cancelOrderInfo.jys);
	RequestEdB *request = new RequestEdB(m_userOperateWidget, "CancelOrder", sendData);
	QThreadPool::globalInstance()->start(request);

	if (m_interval != 0)
	{
		while (true)
		{
			msleep(m_interval);
			RequestEdB *request = new RequestEdB(m_userOperateWidget, "CancelOrder", sendData);
			QThreadPool::globalInstance()->start(request);
		}
	}


	/*QString ret = EdB_CancelOrder(
		m_cancelOrderInfo.clientID,
		m_cancelOrderInfo.user.toStdString().c_str(),
		m_cancelOrderInfo.bho.toStdString().c_str(),
		m_cancelOrderInfo.jys);
	emit  cancelOrderRet(ret);
	if (m_interval != 0)
	{
		while (true)
		{
			msleep(m_interval);
			ret = EdB_CancelOrder(
				m_cancelOrderInfo.clientID,
				m_cancelOrderInfo.user.toStdString().c_str(),
				m_cancelOrderInfo.bho.toStdString().c_str(),
				m_cancelOrderInfo.jys);
			emit cancelOrderRet(ret);
		}
	}*/
}