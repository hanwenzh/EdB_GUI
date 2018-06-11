#include "useroperatewidget.h"
#include "edbsetting.h"
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QUuid>
#include <QDir>
#include <QMutex>
#include <QThreadPool>
#include <QDebug>
#include <QEventLoop>
#include <algorithm>
#include "requestedb.h"
#include "requestlogin.h"
#include "orderthread.h"
#include "dataresultwidget.h"
#include <QtScript>
#include <qvector.h>

UserOperateWidget::UserOperateWidget(RequestLogin::LoginInfo loginInfo,  QWidget *parent)
	: QWidget(parent)
{
	m_loginInfo = loginInfo;
	m_lastRecordPath = "";
	ui.setupUi(this);

	initTab();
	rollBackRecord();

	refreshTimer.setInterval(500);
	connect(&refreshTimer, &QTimer::timeout, this, &UserOperateWidget::onUpdateView);
	refreshTimer.start();

}

UserOperateWidget::UserOperateWidget(QString lastRecordPath, QWidget *parent)
	: QWidget(parent)
{
	m_lastRecordPath = lastRecordPath;
	ui.setupUi(this);

	initTab();
	rollBackRecord();

	refreshTimer.setInterval(500);
	connect(&refreshTimer, &QTimer::timeout, this, &UserOperateWidget::onUpdateView);
	refreshTimer.start();
	ui.textBrowser_getPriceTable->setHtml("");
}

UserOperateWidget::~UserOperateWidget()
{
	

}

void UserOperateWidget::initTab()
{
	connect(ui.button_sendOrder, &QPushButton::clicked, this, &UserOperateWidget::onButtonSendOrderClicked);
	connect(ui.button_cancelOrder, &QPushButton::clicked, this, &UserOperateWidget::onButtonCancelOrderClicked);

	ui.lineEdit_queryInterval->setValidator(new QIntValidator(1000, 100000, this));
	ui.lineEdit_queryInterval->setText("2000");

	ui.lineEdit_getPriceInterval->setValidator(new QIntValidator(1000, 100000, this));
	ui.lineEdit_getPriceInterval->setText("2000");

	connect(ui.button_setEdB, &QPushButton::clicked, [=]() {

		EdBSetting queryDataSet(m_loginInfo.ip, this);
		queryDataSet.exec();
		if (queryDataSet.result() == 2)
		{
			ui.combo_category->updateOptions(queryDataSet.m_queryKeyList);
			m_curQueryInfo = queryDataSet.m_queryInfo;
			m_curQueryScript = queryDataSet.m_queryScript;
			m_curPriceScript = queryDataSet.m_priceScript;
		}
	});

	connect(ui.toolBox_queryData, &QToolBox::currentChanged, this, &UserOperateWidget::onQueryToolBoxIndexChanged);

	m_queryTimer.setSingleShot(false);
	connect(ui.button_query, &QPushButton::clicked, this, &UserOperateWidget::onButtonQueryClicked);
	connect(&m_queryTimer, &QTimer::timeout, this, &UserOperateWidget::onQueryTimerTimeout);

	m_priceTimer.setSingleShot(false);
	connect(&m_priceTimer, &QTimer::timeout, this, &UserOperateWidget::onPriceTimerTimeout);

	ui.textBrowser_getPriceTable->setText("");
	ui.textEdit_getPriceText->setText("");
	connect(ui.button_getPrice, &QPushButton::clicked, this, &UserOperateWidget::onButtonGetPriceClicked);
}

void UserOperateWidget::onButtonQueryClicked()
{
	auto queryList = ui.combo_category->getSelectItems();
	for (int i = ui.toolBox_queryData->count() - 1; i >= 0; i--)
	{
		int index = queryList.indexOf(ui.toolBox_queryData->itemText(i));
		if (index == -1)
			ui.toolBox_queryData->removeItem(i);
	}
	m_queryTableIndexMap.clear();
	for (int i = 0; i < ui.toolBox_queryData->count(); i++)
	{
		m_queryTableIndexMap.insert(ui.toolBox_queryData->itemText(i), i);
	}
	for (auto item : queryList)
	{
		if (!m_queryTableIndexMap.contains(item))
		{
			DataResultWidget *pResultView = new DataResultWidget(this);
			QStandardItemModel *pModel = new QStandardItemModel(this);
			pResultView->ui.tableView->setModel(pModel);
			m_queryTableIndexMap.insert(item, ui.toolBox_queryData->addItem(pResultView, item));
		}
	}
	if (ui.toolBox_queryData->count() == 0)
	{
		ui.toolBox_queryData->addItem([]()->QLabel * {auto p = new QLabel(QString::fromWCharArray(L"未设置查询项")); p->setAlignment(Qt::AlignCenter); return p; }(), "");
		return;
	}
	if (ui.toolBox_queryData->currentIndex() == -1)
		ui.toolBox_queryData->setCurrentIndex(0);

	if (ui.check_repeatQuery->isChecked())
	{
		if (m_queryTimer.isActive())
		{
			m_queryTimer.stop();
			ui.combo_category->setEnabled(true);
			ui.check_repeatQuery->setEnabled(true);
			ui.lineEdit_queryInterval->setEnabled(true);
			ui.label_query1->setEnabled(true);
			ui.label_query2->setEnabled(true);
			ui.button_setEdB->setEnabled(true);
			ui.button_query->setText(QString::fromStdWString(L"查询"));
		}
		else
		{
			m_queryTimer.setInterval(ui.lineEdit_queryInterval->text().toInt());
			m_queryTimer.start();
			ui.combo_category->setEnabled(false);
			ui.check_repeatQuery->setEnabled(false);
			ui.lineEdit_queryInterval->setEnabled(false);
			ui.label_query1->setEnabled(false);
			ui.label_query2->setEnabled(false);
			ui.button_setEdB->setEnabled(false);
			ui.button_query->setText(QString::fromStdWString(L"停止"));
			onQueryTimerTimeout();
		}
	}
	else
	{
		for (auto item : queryList)
		{
			queryData(item, m_curQueryInfo[item]);
		}
	}
}

void UserOperateWidget::onButtonGetPriceClicked()
{
	if (ui.lineEdit_getPrice->text().isEmpty())
	{
		ui.textEdit_getPriceText->setText(QString::fromStdWString(L"未设置查询项"));
		ui.textBrowser_getPriceTable->setText(QString::fromStdWString(L"未设置查询项"));
	}
	if (ui.check_repeatGetPrice->isChecked())
	{
		if (m_priceTimer.isActive())
		{
			m_priceTimer.stop();
			ui.lineEdit_getPrice->setEnabled(true);
			ui.lineEdit_getPriceInterval->setEnabled(true);
			ui.check_repeatGetPrice->setEnabled(true);
			ui.label_getPrice1->setEnabled(true);
			ui.label_getPrice2->setEnabled(true);
			ui.button_getPrice->setText(QString::fromStdWString(L"查询"));
		}
		else
		{
			m_priceTimer.setInterval(ui.lineEdit_getPriceInterval->text().toInt());
			m_priceTimer.start();
			ui.lineEdit_getPrice->setEnabled(false);
			ui.lineEdit_getPriceInterval->setEnabled(false);
			ui.check_repeatGetPrice->setEnabled(false);
			ui.label_getPrice1->setEnabled(false);
			ui.label_getPrice2->setEnabled(false);
			ui.button_getPrice->setText(QString::fromStdWString(L"停止"));
			onPriceTimerTimeout();
		}
	}
	else
	{
		getPrice();
	}
}

void UserOperateWidget::getPrice()
{
	QString gpdm = ui.lineEdit_getPrice->text();
	if (!gpdm.isEmpty())
	{
		QList<QVariant> sendData;
		sendData.append(gpdm);
		sendData.append(m_loginInfo.clientID);
		sendData.append(gpdm);
		RequestEdB *request = new RequestEdB(this, "GetPrice", sendData);
		QThreadPool::globalInstance()->start(request);
	}
}

void UserOperateWidget::queryData(QString name,  int category)
{
	QList<QVariant> sendData;
	sendData.append(name);
	sendData.append(m_loginInfo.user);
	sendData.append(m_loginInfo.clientID);
	sendData.append(category);
	RequestEdB *request = new RequestEdB(this, "QueryData", sendData);
	QThreadPool::globalInstance()->start(request);
}

void UserOperateWidget::onQueryTimerTimeout()
{
	int index = ui.toolBox_queryData->currentIndex();
	if (index == -1)
		return;
	QString name = ui.toolBox_queryData->itemText(index);
	if (!name.isEmpty())
		queryData(name, m_curQueryInfo[name]);
}

void UserOperateWidget::onPriceTimerTimeout()
{
	QString gpdm = ui.lineEdit_getPrice->text();
	if (!gpdm.isEmpty())
	{
		QList<QVariant> sendData;
		sendData.append(gpdm);
		sendData.append(m_loginInfo.clientID);
		sendData.append(gpdm);
		RequestEdB *request = new RequestEdB(this, "GetPrice", sendData);
		QThreadPool::globalInstance()->start(request);
	}
}


void UserOperateWidget::onButtonSendOrderClicked()
{
	SendOrderThread::SendOrderInfo sendOrderInfo;
	sendOrderInfo.fx = ui.tableWidget_sendOrder->item(0, 1)->text().toInt();
	sendOrderInfo.gddm = ui.tableWidget_sendOrder->item(1, 1)->text();
	sendOrderInfo.gpdm = ui.tableWidget_sendOrder->item(2, 1)->text();
	sendOrderInfo.price = ui.tableWidget_sendOrder->item(3, 1)->text().toFloat();
	sendOrderInfo.quantity = ui.tableWidget_sendOrder->item(4, 1)->text().toInt();
	sendOrderInfo.clientID = m_loginInfo.clientID;
	sendOrderInfo.user = m_loginInfo.user;
	int interval = ui.tableWidget_sendOrder->item(5, 1)->text().toInt();

	if (interval != 0 && interval < 1000)
	{
		QMessageBox::information(this, QString::fromStdWString(L"发送委托"), QString::fromStdWString(L"时间间隔设置时间过短！"));
		return;
	}
	else if (sendOrderInfo.gddm.isEmpty() || sendOrderInfo.gpdm.isEmpty() ||  fabs(sendOrderInfo.price) < 0.0001 || abs(sendOrderInfo.quantity) == 0)
	{
		QMessageBox::information(this, QString::fromStdWString(L"发送委托"), QString::fromStdWString(L"部分字段设置有误！"));
		return;
	}

	m_sendOrderMutex.lock();
	if (m_sendOrderCache.contains(sendOrderInfo.gpdm))
	{
		SendOrderThread *pCurThread = (SendOrderThread *)m_sendOrderCache[sendOrderInfo.gpdm];
		m_sendOrderCache.remove(sendOrderInfo.gpdm);
		if (pCurThread->isRunning())
			pCurThread->quit();
		delete pCurThread;
	}
	SendOrderThread *pSendOrderThread = new SendOrderThread(sendOrderInfo, this, interval);
	pSendOrderThread->start();
	m_sendOrderCache.insert(sendOrderInfo.gpdm, pSendOrderThread);
	m_sendOrderMutex.unlock();
}

void UserOperateWidget::onButtonCancelOrderClicked()
{
	CancelOrderThread::CancelOrderInfo cancelOrderInfo;
	cancelOrderInfo.bho = ui.tableWidget_cancelOrder->item(0, 1)->text();
	cancelOrderInfo.jys = ui.tableWidget_cancelOrder->item(1, 1)->text().toInt();
	cancelOrderInfo.clientID = m_loginInfo.clientID;
	cancelOrderInfo.user = m_loginInfo.user;
	int interval = ui.tableWidget_cancelOrder->item(2, 1)->text().toInt();
	if (interval != 0 && interval < 1000)
	{
		QMessageBox::information(this, QString::fromStdWString(L"取消委托"), QString::fromStdWString(L"时间间隔设置时间过短！"));
		return;
	}
	else if (cancelOrderInfo.bho.isEmpty())
	{
		QMessageBox::information(this, QString::fromStdWString(L"取消委托"), QString::fromStdWString(L"部分字段设置有误！"));
		return;
	}

	m_cancelOrderMutex.lock();
	if (m_cancelOrderCache.contains(cancelOrderInfo.bho))
	{
		CancelOrderThread *pThread = (CancelOrderThread *)m_cancelOrderCache[cancelOrderInfo.bho];
		m_cancelOrderCache.remove(cancelOrderInfo.bho);
		if (pThread->isRunning())
			pThread->quit();
		delete pThread;
	}
	CancelOrderThread *pCancelOrderThread = new CancelOrderThread(cancelOrderInfo, this, interval);
	pCancelOrderThread->start();
	m_cancelOrderCache.insert(cancelOrderInfo.bho, pCancelOrderThread);
	m_cancelOrderMutex.unlock();
}

void UserOperateWidget::onQueryToolBoxIndexChanged(int)
{
	if (ui.check_repeatQuery->isChecked() && m_queryTimer.isActive())
		onQueryTimerTimeout();
}


void UserOperateWidget::onUpdateView()
{
	m_queryMutex.lock();
	QString curQueryName = ui.toolBox_queryData->itemText(ui.toolBox_queryData->currentIndex());
	QString queryValue = m_queryTableCache[curQueryName];
	DataResultWidget *pResultView = (DataResultWidget*)ui.toolBox_queryData->currentWidget();
	if (!curQueryName.isEmpty() && pResultView != nullptr)
	{
		if (!queryValue.isEmpty())
			pResultView->ui.textEdit_text->setText(queryValue);

		QScriptEngine engine;
		QString script = "";
		if (m_curQueryScript.contains(curQueryName))
			script = m_curQueryScript[curQueryName];
		if (!queryValue.isEmpty())
			pResultView->ui.textEdit_script->setText(script);

		QScriptValue scriptValue = engine.evaluate(QString("data=\"") + queryValue + "\";" + script);
		QStringList result = qscriptvalue_cast<QStringList>(scriptValue);

		QList<QStringList> resultList;
		for (int i = 0; i < result.count(); i++)
		{
			QStringList tmpList = result[i].split("|");
			resultList.append(tmpList);
		}

		if (resultList.count() >= 1)
		{
			QStandardItemModel *pModel = (QStandardItemModel *)pResultView->ui.tableView->model();
			int rowCount = resultList.count();
			int columnCount = resultList[0].count();
			pModel->clear();
			pModel->setHorizontalHeaderLabels(resultList[0]);
			for (int i = 1; i < rowCount; i++)
			{
				QList<QStandardItem *> rowItems;
				for (auto item : resultList[i])
				{
					item = item.replace("<|>", "|");
					rowItems.append(new QStandardItem(item));
				}
				pModel->appendRow(rowItems);
			}
		}
	}
	m_queryTableCache.remove(curQueryName);
	m_queryMutex.unlock();

	m_getPriceMutex.lock();
	QList<QString> values = m_getPriceCache.values();
	if (values.count() > 0)
	{
		QString getPriceValue = values[0];
		ui.textEdit_getPriceText->setText(getPriceValue);

		QScriptEngine engine;
		QScriptValue scriptValue = engine.evaluate(QString("data=\"") + getPriceValue + "\";" + m_curPriceScript);
		ui.textBrowser_getPriceTable->setHtml(scriptValue.toString());
		m_getPriceCache.clear();
	}
	m_getPriceMutex.unlock();

}



void UserOperateWidget::rollBackRecord()
{
	if (!m_lastRecordPath.isEmpty())
	{
		QSettings settings(m_lastRecordPath, QSettings::IniFormat);
		m_loginInfo.showName = settings.value("loginInfo/showName", "").toString();
		m_loginInfo.ip = settings.value("loginInfo/ip", "null").toString();
		m_loginInfo.port = settings.value("loginInfo/port", "0").toInt();
		m_loginInfo.version = settings.value("loginInfo/version", "null").toString();
		m_loginInfo.user = settings.value("loginInfo/user", "null").toString();
		m_loginInfo.password = QString(QByteArray::fromBase64(settings.value("loginInfo/password", "null").toByteArray()));
		m_loginInfo.txpass = QString(QByteArray::fromBase64(settings.value("loginInfo/txpass", "null").toByteArray()));
		m_loginInfo.yyb = settings.value("loginInfo/yyb", "null").toString();
		QEventLoop loginLoop;
		RequestLogin loginRequest(m_loginInfo, this);
		connect(&loginRequest, &RequestLogin::finished, [&]() {loginLoop.quit(); });
		loginRequest.start();
		loginLoop.exec();
		m_loginInfo = loginRequest.getResult();
		EdBSetting queryDataSet(m_loginInfo.ip, this);
		queryDataSet.readIni();
		m_curQueryInfo = queryDataSet.m_queryInfo;
		m_curQueryScript = queryDataSet.m_queryScript;
		m_curPriceScript = queryDataSet.m_priceScript;
		ui.combo_category->initCombo(queryDataSet.m_queryKeyList, settings.value("queryRecord/querylist").toStringList());
		ui.check_repeatQuery->setChecked(settings.value("queryRecord/isrepeatquery", false).toBool());
		ui.lineEdit_queryInterval->setText(settings.value("queryRecord/queryinterval", "2000").toString());
		ui.lineEdit_getPrice->setText(settings.value("priceRecord/getpricereno", "").toString());
		ui.check_repeatQuery->setChecked(settings.value("priceRecord/isrepeatgetprice", false).toBool());
		ui.lineEdit_queryInterval->setText(settings.value("priceRecord/getpriceinterval", "2000").toString());
		if (m_loginInfo.clientID == 0)
			this->setEnabled(false);
	}
	else
	{
		EdBSetting queryDataSet(m_loginInfo.ip, this);
		queryDataSet.readIni();
		m_curQueryInfo = queryDataSet.m_queryInfo;
		m_curQueryScript = queryDataSet.m_queryScript;
		m_curPriceScript = queryDataSet.m_priceScript;
		ui.combo_category->initCombo(queryDataSet.m_queryKeyList);
	}
}

void UserOperateWidget::saveRecord()
{
	if (m_lastRecordPath.isEmpty())
	{
		QDir dir(qApp->applicationDirPath() + "/data");
		if (!dir.exists())
			dir.mkdir(qApp->applicationDirPath() + "/data");
		m_lastRecordPath = (qApp->applicationDirPath() + "/data/%1.ini").arg(QUuid::createUuid().toString());
	}

	QSettings settings(m_lastRecordPath, QSettings::IniFormat);
	settings.setValue("loginInfo/showName", m_loginInfo.showName);
	settings.setValue("loginInfo/ip", m_loginInfo.ip);
	settings.setValue("loginInfo/port", m_loginInfo.port);
	settings.setValue("loginInfo/version", m_loginInfo.version);
	settings.setValue("loginInfo/user", m_loginInfo.user);
	settings.setValue("loginInfo/password", m_loginInfo.password.toLatin1().toBase64());
	settings.setValue("loginInfo/txpass", m_loginInfo.txpass.toLatin1().toBase64());
	settings.setValue("loginInfo/yyb", m_loginInfo.yyb);
	settings.setValue("queryRecord/querylist", ui.combo_category->getSelectItems());
	settings.setValue("queryRecord/isrepeatquery", ui.check_repeatQuery->isChecked());
	settings.setValue("queryRecord/queryinterval", ui.lineEdit_queryInterval->text());
	settings.setValue("priceRecord/getpricereno", ui.lineEdit_getPrice->text());
	settings.setValue("priceRecord/isrepeatgetprice", ui.check_repeatGetPrice->isChecked());
	settings.setValue("priceRecord/getpriceinterval", ui.lineEdit_getPriceInterval->text());
}



