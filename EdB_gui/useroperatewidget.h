#pragma once

#include <QWidget>
#include "ui_useroperatewidget.h"
#include "requestlogin.h"
#include <Qtimer>
#include <QRunnable>
#include <QMutex>
#include <QThread>

class UserOperateWidget : public QWidget
{
	Q_OBJECT


public:
	UserOperateWidget(RequestLogin::LoginInfo loginInfo, QWidget *parent = Q_NULLPTR);
	UserOperateWidget(QString lastRecordPath, QWidget *parent = Q_NULLPTR);
	~UserOperateWidget();

	RequestLogin::LoginInfo m_loginInfo;
	QTimer m_queryTimer;
	QMap<QString, int> m_curQueryInfo;
	QMap<QString, QString> m_curQueryScript;
	QTimer m_priceTimer;
	QString m_curPriceScript;
	QString m_lastRecordPath;
	QMap<QString, QString> m_queryTableCache;
	QMap<QString, QString> m_getPriceCache;
	QMap<QString, QThread *> m_sendOrderCache;
	QMap<QString, QThread *> m_cancelOrderCache;
	QMutex m_queryMutex;
	QMutex m_getPriceMutex;
	QMutex m_sendOrderMutex;
	QMutex m_cancelOrderMutex;
	void saveRecord();

private:
	Ui::UserOperateWidget ui;
	void initTab();
	void getPrice();
	void queryData(QString name ,int category);
	QMap<QString, int> m_queryTableIndexMap;
	void rollBackRecord();

	QTimer refreshTimer;

private slots:
	void onButtonQueryClicked();
	void onButtonGetPriceClicked();
	void onButtonSendOrderClicked();
	void onButtonCancelOrderClicked();
	void onUpdateView();		//刷新实时数据
	void onQueryTimerTimeout();
	void onPriceTimerTimeout();
	void onQueryToolBoxIndexChanged(int);
};
