#pragma once

#include <QRunnable>
#include "useroperatewidget.h" 

class RequestEdB : public QRunnable
{

private:
	UserOperateWidget * m_userOperateWidget;
	QList<QVariant> m_args;
	QString m_cmd;

public:
	RequestEdB(UserOperateWidget *, QString cmd, QList<QVariant> args);
	void run();
};

