#pragma once

#include <QThread>

class RequestLogin : public QThread
{
	Q_OBJECT

public:
	typedef struct LoginInfo
	{
		QString showName;
		QString coreGuid = "";

		int clientID;
		QString ip;
		int port;
		QString version;
		QString user;
		QString password;
		QString txpass;
		QString yyb;
	} LoginInfo;

public:
	RequestLogin(LoginInfo loginInfo, QObject *parent);
	~RequestLogin();
	inline LoginInfo getResult() {
		return m_loginInfo;
	}

private:
	LoginInfo m_loginInfo;
	void run();

};
