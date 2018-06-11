#include "edbsetting.h"
#include <QPushButton>
#include <QMessageBox>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QTextStream>

EdBSetting::EdBSetting(QString ip, QWidget *parent)
	: QDialog(parent), m_settings((qApp->applicationDirPath() + "/config/%1.ini").arg(ip), QSettings::IniFormat)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	connect(ui.combo_priceScript, &QComboBox::currentTextChanged, this, &EdBSetting::onComboPriceScriptChanged);
	connect(ui.button_confirm, &QPushButton::clicked, this, &EdBSetting::onButtonConfirmClicked);
	readIni();
}

EdBSetting::~EdBSetting()
{

}

void EdBSetting::readIni()
{
	QString content = m_settings.value("queryconfig/queryinfo", "").toString();
	if (parseQueryInfo(content))
		ui.textEdit_queryData->setPlainText(content);
	else
		ui.textEdit_queryData->setPlainText("");

	parsePriceInfo();
	ui.combo_priceScript->addItems(m_priceScriptFileContents.keys());
	QString priceBindJs = m_settings.value("priceconfig/bindjs", "").toString();
	if (!priceBindJs.isEmpty())
		ui.combo_priceScript->setCurrentText(priceBindJs);
	else
		ui.combo_priceScript->setCurrentIndex(0);
	m_priceScript = m_priceScriptFileContents[ui.combo_priceScript->currentText()];
}

void EdBSetting::writeIni()
{
	m_settings.setValue("queryconfig/queryinfo", ui.textEdit_queryData->toPlainText());
	m_settings.setValue("priceconfig/bindjs", ui.combo_priceScript->currentText());
}

bool EdBSetting::parseQueryInfo(QString content)
{
	m_queryScriptFileContents.clear();
	QDir dir(qApp->applicationDirPath() + "/scripts/querydata");
	if (dir.exists())
	{
		QFileInfoList jsList = dir.entryInfoList(QStringList() << "*.js", QDir::Files);
		for (auto item : jsList)
		{
			QFile file(item.absoluteFilePath());
			bool isOpen = file.open(QIODevice::ReadOnly);
			if (isOpen)
			{
				QString jsData = file.readAll();
				m_queryScriptFileContents.insert(item.completeBaseName(), jsData);
				file.close();
			}
		}
	}

	m_queryKeyList.clear();
	m_queryInfo.clear();
	m_queryScript.clear();
	content = content.replace(QString::fromStdWString(L"，"), ",");
	for (auto data : content.split("\n"))
	{
		int index = data.indexOf("=");
		QString trueData;
		if (index >= 0 && index < data.length() - 1)
			trueData = data.mid(index + 1);
		else
			trueData = data;
		auto dataList = trueData.split(",");
		if (dataList.count() < 2)
		{
			return false;
		}

		if (m_queryScriptFileContents.contains(QString::fromStdWString(L"默认")))
			m_queryScript.insert(dataList[1], m_queryScriptFileContents[QString::fromStdWString(L"默认")]);

		if (dataList.count() > 2)
		{
			QString jsFileName = dataList[2].simplified();
			if (m_queryScriptFileContents.contains(jsFileName))
				m_queryScript[dataList[1]] = m_queryScriptFileContents[jsFileName];
		}
		m_queryInfo.insert(dataList[1], dataList[0].toInt());
		m_queryKeyList.append(dataList[1]);
	}
	return true;
}

bool EdBSetting::parsePriceInfo()
{
	m_priceScriptFileContents.clear();
	QDir dir(qApp->applicationDirPath() + "/scripts/getprice");
	if (dir.exists())
	{
		QFileInfoList jsList = dir.entryInfoList(QStringList() << "*.js", QDir::Files);
		for (auto item : jsList)
		{
			QFile file(item.absoluteFilePath());
			bool isOpen = file.open(QIODevice::ReadOnly);
			if (isOpen)
			{
				QString jsData = file.readAll();
				m_priceScriptFileContents.insert(item.completeBaseName(), jsData);
				file.close();
			}
		}
	}
	return true;
}

void EdBSetting::onComboPriceScriptChanged(const QString &)
{
	QString selectedJS = ui.combo_priceScript->currentText();
	if (m_priceScriptFileContents.contains(selectedJS))
	{
		ui.textEdit_priceScript->setText(m_priceScriptFileContents[selectedJS]);
		m_priceScript = m_priceScriptFileContents[selectedJS];
	}
	else
	{
		ui.textEdit_priceScript->setText("");
		m_priceScript = "";
	}

}

void EdBSetting::onButtonConfirmClicked()
{
	m_queryInfo.clear();
	QString inputData = ui.textEdit_queryData->toPlainText();
	if (!parseQueryInfo(inputData))
	{
		QMessageBox::critical(this, QString::fromStdWString(L"错误"), QString::fromStdWString(L"查询配置填写有误"));
		return;
	}
	QFile file(qApp->applicationDirPath() + "/scripts/getprice/" + ui.combo_priceScript->currentText() + ".js");
	bool isOk = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	if (!isOk)
	{
		QMessageBox::critical(this, QString::fromStdWString(L"错误"), QString::fromStdWString(L"盘口解析文件保存失败"));
		return;
	}

	QTextStream textSream(&file);
	textSream.setCodec("utf-8");
	textSream << ui.textEdit_priceScript->toPlainText();
	file.close();
	m_priceScript = ui.textEdit_priceScript->toPlainText();
	writeIni();
	this->close();
	this->setResult(2);
}