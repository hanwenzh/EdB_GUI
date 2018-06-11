#pragma once

#include <QThread>
#include "useroperatewidget.h"

class SendOrderThread : public QThread
{
	Q_OBJECT

public:
	typedef struct SendOrderInfo
	{
		int clientID;
		int fx;
		QString user;
		QString gddm;
		QString gpdm;
		int quantity;
		float price;
	}SendOrderInfo;

public:
	SendOrderThread(SendOrderInfo sendOrderInfo, UserOperateWidget *widget, int interval = 0);
	~SendOrderThread();

signals:
	void sendOrderRet(QString);

private:
	SendOrderInfo m_sendOrderInfo;
	int m_interval;
	UserOperateWidget *m_userOperateWidget;
	void run();
};

class CancelOrderThread : public QThread
{
	Q_OBJECT

public:
	typedef struct CancelOrderInfo
	{
		int clientID;
		QString user;
		QString bho;
		int jys;
	}CancelOrderInfo;

public:
	CancelOrderThread(CancelOrderInfo cancelOrderInfo, UserOperateWidget *widget, int interval = 0);
	~CancelOrderThread();

signals:
	void cancelOrderRet(QString);

private:
	CancelOrderInfo m_cancelOrderInfo;
	int m_interval;
	UserOperateWidget *m_userOperateWidget;
	void run();
};
