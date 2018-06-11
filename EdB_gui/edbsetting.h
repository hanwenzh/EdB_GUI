#pragma once

#include <QDialog>
#include <QMap>
#include <QSettings>
#include "ui_edbsetting.h"

class EdBSetting : public QDialog
{
	Q_OBJECT

public:
	EdBSetting(QString ip ,QWidget *parent = Q_NULLPTR);
	~EdBSetting();
	
public:
	QMap<QString, int> m_queryInfo;
	QMap<QString, QString> m_queryScript;
	QMap<QString, QString> m_queryScriptFileContents;
	QString m_priceScript;
	QMap<QString, QString> m_priceScriptFileContents;
	QStringList m_queryKeyList;
	int m_buyNo;
	int m_sellNo;
	QSettings m_settings;
	void readIni();

private:
	Ui::EdBSetting ui;
	bool parseQueryInfo(QString content);
	bool parsePriceInfo();
	void writeIni();

private slots:
	void onComboPriceScriptChanged(const QString &);
	void onButtonConfirmClicked();
};