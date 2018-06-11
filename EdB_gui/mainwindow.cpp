#include "mainwindow.h"
#include <QTabBar>
#include <QPushButton>
#include <QCheckBox>
#include "loginwidget.h"
#include <windowsx.h>
#include <windows.h>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QFontDatabase>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCloseEvent>
#include <QProcess>
#include <QThreadPool>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.tabWidget->clear();

	connect(ui.button_addUser, SIGNAL(clicked()), this, SLOT(onButtonAddUserClicked()));
	connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));

	QFontDatabase::addApplicationFont(":/fontawesome.ttf");

	QFont font;
	font.setPointSize(11);
	font.setFamily(("FontAwesome")); //注意这里不完全是TTF文件名字
	font.setBold(true);
	ui.label_title->setFont(font);

	font.setPointSize(9);
	font.setBold(false);
	ui.button_min->setFont(font);
	ui.button_max->setFont(font);
	ui.button_close->setFont(font);
	ui.button_addUser->setFont(font);

	ui.button_min->setText(QChar(0xf2d1));
	ui.button_max->setText(QChar(0xf2d0));
	ui.button_close->setText(QChar(0xf2d3));
	ui.button_addUser->setText(QString(QChar(0xf2be)) +"+");

	ui.button_addUser->setToolTip(QString::fromWCharArray(L"新建连接"));

	connect(ui.button_min, &QPushButton::clicked, [=]() {this->showMinimized(); });
	connect(ui.button_max, &QPushButton::clicked, [=]() {this->isMaximized() ? this->showNormal() : this->showMaximized(); });
	connect(ui.button_close, &QPushButton::clicked, [=]() {this->close(); });

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

	HWND hwnd = (HWND)this->winId();
	DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
	::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);

	ui.label_welcome->setText(QString::fromStdWString(L"正在还原上次登录会话，请稍后！"));
	show();
	rollBackRecord();
}

void MainWindow::rollBackRecord()
{
	ui.stackedWidget->setCurrentIndex(0);
	ui.label_welcome->setText(QString::fromStdWString(L"正在还原上次登录会话，请稍后！"));
	QDir dir(qApp->applicationDirPath() + "/data");
	if (!dir.exists())
		return;
	int i = 0;
	ui.label_state->show();
	QFileInfoList iniList = dir.entryInfoList(QStringList() << "*.ini", QDir::Files);
	for (auto item : iniList)
	{
		if (item.suffix() != "ini")
			continue;

		ui.label_state->setText(QString::fromStdWString(L"检测到会话 %1 个，正在还原第 %2 个 ...").arg(iniList.count()).arg(++i));
		UserOperateWidget *pWidget = new UserOperateWidget(item.absoluteFilePath());
		int index = ui.tabWidget->addTab(pWidget, pWidget->m_loginInfo.showName.isEmpty()? pWidget->m_loginInfo.ip + " - " + pWidget->m_loginInfo.user : pWidget->m_loginInfo.showName);
		ui.tabWidget->tabBar()->setTabButton(index, QTabBar::LeftSide, []() -> QCheckBox* {QCheckBox *pCheck = new QCheckBox(); pCheck->setChecked(true); return pCheck; }());
		ui.tabWidget->setTabToolTip(index, QString("IP: ") + pWidget->m_loginInfo.ip + "\nPort: " + QString::number(pWidget->m_loginInfo.port) + "\nVersion: " + pWidget->m_loginInfo.version + "\nUser: " + pWidget->m_loginInfo.user + "\nYyb: " + pWidget->m_loginInfo.yyb);
		if (pWidget->m_loginInfo.clientID == 0)
		{
			pWidget->setEnabled(false);
		}
		ui.stackedWidget->setCurrentIndex(1);
	}
	if (ui.tabWidget->count())
	{
		ui.label_welcome->setText(QString::fromStdWString(L"当前尚未有任何登录信息！"));
	}
	ui.label_state->hide();
}


void MainWindow::onButtonAddUserClicked()
{
	QDir dir(qApp->applicationDirPath() + "/config");
	if (!dir.exists())
		dir.mkdir(qApp->applicationDirPath() + "/config");

	LoginWidget loginWidget(this);
	loginWidget.exec();
	
	if (loginWidget.result() == 2)
	{
		int index = ui.tabWidget->addTab(new UserOperateWidget(loginWidget.m_loginInfo), loginWidget.m_loginInfo.showName.isEmpty() ? loginWidget.m_loginInfo.ip + " - " + loginWidget.m_loginInfo.user : loginWidget.m_loginInfo.showName);
		ui.tabWidget->tabBar()->setTabButton(index, QTabBar::LeftSide, []() -> QCheckBox* {QCheckBox *pCheck = new QCheckBox(); pCheck->setChecked(true); return pCheck; }());
		ui.tabWidget->setTabToolTip(index, QString("IP: ") + loginWidget.m_loginInfo.ip + "\nPort: " + QString::number(loginWidget.m_loginInfo.port) + "\nVersion: " + loginWidget.m_loginInfo.version +"\nUser: " + loginWidget.m_loginInfo.user + "\nYyb: " + loginWidget.m_loginInfo.yyb);
	}

	if (ui.tabWidget->count() == 0)
	{
		ui.stackedWidget->setCurrentIndex(0);
		ui.label_welcome->setText(QString::fromStdWString(L"当前尚未有任何登录信息！"));
	}
	else
		ui.stackedWidget->setCurrentIndex(1);
}


void MainWindow::onTabCloseRequested(int index)
{
	QCheckBox *pCheckBox = (QCheckBox *)ui.tabWidget->tabBar()->tabButton(index, QTabBar::LeftSide);
	UserOperateWidget *pWidget = (UserOperateWidget *)ui.tabWidget->widget(index);
	if (pCheckBox != NULL && pCheckBox->isChecked())
	{
		pWidget->saveRecord();
	}
	else
	{
		if (!pWidget->m_lastRecordPath.isEmpty())
		{
			QFile::remove(pWidget->m_lastRecordPath);
		}
	}
	ui.tabWidget->removeTab(index);
	if (ui.tabWidget->count() == 0)
	{
		ui.label_welcome->setText(QString::fromStdWString(L"当前尚未有任何登录信息！"));
		ui.stackedWidget->setCurrentIndex(0);
	}

	else
		ui.stackedWidget->setCurrentIndex(1);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	for (int i = ui.tabWidget->count() - 1; i >= 0 ; i--)
	{
		onTabCloseRequested(i);
	}
	QThreadPool::globalInstance()->clear();
	QProcess::execute("taskkill", QStringList() << "/im" << "EdB_Core_*" << "/f");
	QDir dir(qApp->applicationDirPath());
	for (auto item: dir.entryList(QStringList() << QString("EdB_Core_*.exe"), QDir::Files))
	{
		QFile::remove(qApp->applicationDirPath() + "/" + item);
	}
	qApp->quit();
}


bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result) {
	MSG* msg = (MSG*)message;
	switch (msg->message) {

	case WM_NCHITTEST:
	{
		int borderWidth = 4;

		QPoint posTemp = mapFromGlobal(QCursor::pos());
		int xPos = posTemp.x();
		int yPos = posTemp.y();
		if (yPos < borderWidth)
		{
			if (xPos < borderWidth)
				*result = HTTOPLEFT;
			else if (xPos > this->size().width() - borderWidth)
				*result = HTTOPRIGHT;
			else
				*result = HTTOP;
			return true;
		}
		else if (yPos > this->size().height() - borderWidth)
		{
			if (xPos < 8)
				*result = HTBOTTOMLEFT;
			else if (xPos > this->size().width() - borderWidth)
				*result = HTBOTTOMRIGHT;
			else
				*result = HTBOTTOM;
			return true;
		}
		else
		{
			if (xPos < borderWidth)
			{
				*result = HTLEFT;
				return true;
			}
			else if (xPos > this->size().width() - borderWidth)
			{
				*result = HTRIGHT;
				return true;
			}
			bool isTop = false;

			QWidget *pWidget = this->childAt(xPos, yPos);
			if (pWidget == NULL)
				isTop = true;
			else
			{
				if (pWidget == ui.centralWidget || pWidget == ui.stackedWidget || pWidget == ui.page_welcome || pWidget == ui.tabWidget || pWidget == ui.label_title || pWidget == ui.label_welcome)
					isTop = true;
			}

			if (isTop)
			{
				*result = HTCAPTION;
				return true;
			}
		}

	}
	break;
	case WM_NCCALCSIZE:
		return true;

	case WM_GETMINMAXINFO:
	{
		HDC screen = GetDC(0);
		int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
		int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
		double scale = dpiX * 100.0 / 96;
		scale /= 100;
		ReleaseDC(0, screen);
		MINMAXINFO *info = (MINMAXINFO *)msg->lParam;
		info->ptMinTrackSize.x = 760 * scale;
		info->ptMinTrackSize.y = 600 * scale;

		if (::IsZoomed(msg->hwnd)) {
			// 最大化时会超出屏幕，所以填充边框间距
			RECT frame = { 0, 0, 0, 0 };
			AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);
			frame.left = abs(frame.left);
			frame.right = abs(frame.left);
			frame.top = abs(frame.bottom);
			this->setContentsMargins(frame.left / scale, frame.top / scale, frame.right / scale, frame.bottom / scale);

			ui.button_max->setText(QChar(0xf2d2));
		}
		else {
			this->setContentsMargins(0, 0, 0, 0);
			ui.button_max->setText(QChar(0xf2d0));
		}

		*result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
		return true;
	}
	break;
	}

	return QMainWindow::nativeEvent(eventType, message, result);
}