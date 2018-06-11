#include "loginwidget.h"
#include <QRegExp>
#include <QMessageBox>
#include <QEventLoop>
#include "requestlogin.h"

LoginWidget::LoginWidget(QWidget *parent)
	: QDialog(parent), m_settings(qApp->applicationDirPath() + "/config/login.ini", QSettings::IniFormat)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	connect(ui.button_test, SIGNAL(clicked()), this, SLOT(onButtonTestClicked()));
	connect(ui.button_confirm, SIGNAL(clicked()), this, SLOT(onButtonConfirmClicked()));
	connect(ui.button_loadConfig, SIGNAL(clicked()), this, SLOT(onButtonLoadConfigClicked()));
	connect(ui.button_deleteConfig, SIGNAL(clicked()), this, SLOT(onButtonDelConfigClicked()));
	ui.lineEdit_port->setValidator(new QIntValidator(0, 65536, this));	
	ui.combo_curConfig->addItems(m_settings.childGroups());
	ui.combo_saveConfig->addItems(m_settings.childGroups());
	ui.combo_saveConfig->setCurrentIndex(-1);
}


void LoginWidget::onButtonLoadConfigClicked()
{
	if (ui.combo_curConfig->currentText().isEmpty())
		return;
	QString curGroup = ui.combo_curConfig->currentText();
	ui.lineEdit_ip->setText(m_settings.value(curGroup + "/ip", "").toString());
	ui.lineEdit_port->setText(m_settings.value(curGroup + "/port", "").toString());
	ui.lineEdit_version->setText(m_settings.value(curGroup + "/version", "").toString());
	ui.lineEdit_user->setText(m_settings.value(curGroup + "/user", "").toString());
	ui.lineEdit_password->setText(QByteArray::fromBase64(m_settings.value(curGroup + "/password", "").toByteArray()));
	ui.lineEdit_txpass->setText(QByteArray::fromBase64(m_settings.value(curGroup + "/txpass", "").toByteArray()));
	ui.lineEdit_yyb->setText(m_settings.value(curGroup + "/yyb", "").toString());
	ui.combo_saveConfig->setCurrentText(curGroup);
}


void LoginWidget::onButtonDelConfigClicked()
{
	if (ui.combo_curConfig->currentText().isEmpty())
		return;
	m_settings.remove(ui.combo_curConfig->currentText());
	ui.combo_curConfig->removeItem(ui.combo_curConfig->currentIndex());
}

void LoginWidget::login()
{
	m_loginInfo.ip = ui.lineEdit_ip->text();
	m_loginInfo.port = ui.lineEdit_port->text().toInt();
	m_loginInfo.user = ui.lineEdit_user->text();
	m_loginInfo.version = ui.lineEdit_version->text();
	m_loginInfo.password = ui.lineEdit_password->text();
	m_loginInfo.txpass = ui.lineEdit_txpass->text();
	m_loginInfo.yyb = ui.lineEdit_yyb->text();

	ui.button_test->setEnabled(false);
	ui.button_confirm->setEnabled(false);

	QEventLoop loginLoop;
	RequestLogin loginRequest(m_loginInfo, this);
	connect(&loginRequest, &RequestLogin::finished, [&]() {loginLoop.quit(); });
	loginRequest.start();
	loginLoop.exec();
	ui.button_test->setEnabled(true);
	ui.button_confirm->setEnabled(true);
	m_loginInfo = loginRequest.getResult();
}

void LoginWidget::onButtonTestClicked()
{
	login();

	if (m_loginInfo.clientID == 0)
	{
		QMessageBox::critical(this, QString::fromWCharArray(L"²âÊÔµÇÂ¼"), QString::fromWCharArray(L"µÇÂ¼Ê§°Ü"));
	}
	else
	{
		QMessageBox::information(this, QString::fromWCharArray(L"²âÊÔµÇÂ¼"), QString::fromWCharArray(L"µÇÂ¼³É¹¦"));
	}
}


void LoginWidget::onButtonConfirmClicked()
{
	login();
	if (m_loginInfo.clientID != 0)
	{
		QString curGroup = ui.combo_saveConfig->currentText();
		if (!curGroup.isEmpty())
		{
			m_settings.setValue(curGroup + "/ip", ui.lineEdit_ip->text());
			m_settings.setValue(curGroup + "/port", ui.lineEdit_port->text());
			m_settings.setValue(curGroup + "/version", ui.lineEdit_version->text());
			m_settings.setValue(curGroup + "/user", ui.lineEdit_user->text());
			m_settings.setValue(curGroup + "/password", ui.lineEdit_password->text().toLatin1().toBase64());
			m_settings.setValue(curGroup + "/txpass", ui.lineEdit_txpass->text().toLatin1().toBase64());
			m_settings.setValue(curGroup + "/yyb", ui.lineEdit_yyb->text());
			m_loginInfo.showName = curGroup;
		}

		close();
		setResult(2);
	}
	else
	{
		QMessageBox::critical(this, QString::fromWCharArray(L"µÇÂ¼"), QString::fromWCharArray(L"µÇÂ¼Ê§°Ü"));
	}
}