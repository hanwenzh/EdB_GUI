#pragma once

#include <QDialog>
#include "ui_loginwidget.h"
#include <QSettings>
#include "requestlogin.h"

class LoginWidget : public QDialog
{
	Q_OBJECT

public:
	LoginWidget(QWidget *parent = Q_NULLPTR);
	RequestLogin::LoginInfo m_loginInfo;
	QSettings m_settings;

private:
	Ui::LoginWidget ui;
	void login();

private slots:
	void onButtonLoadConfigClicked();
	void onButtonDelConfigClicked();
	void onButtonTestClicked();
	void onButtonConfirmClicked();
};
