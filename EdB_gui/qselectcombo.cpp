#include "qselectcombo.h"
#include <QCheckBox>
#include <QDebug>

QSelectCombo::QSelectCombo(QWidget *parent):QComboBox(parent)
{
	m_listWidget = NULL;
	m_lineEdit = NULL;
}

void QSelectCombo::initCombo(QStringList options, QStringList selectedConfig)
{
	m_listWidget = new QListWidget(this);
	m_lineEdit = new QLineEdit(this);
	m_lineEdit->setReadOnly(true);
	for (int i = 0; i < options.count(); i++)
	{
		QListWidgetItem *pListItem = new QListWidgetItem(m_listWidget);
		m_listWidget->addItem(pListItem);
		pListItem->setData(Qt::UserRole, i);
		QCheckBox *pCheck = new QCheckBox(this);
		pCheck->setText(options[i]);
		connect(pCheck, SIGNAL(stateChanged(int)), this, SLOT(onComboCheckStateChanged(int)));
		if (selectedConfig.indexOf(options[i]) >= 0)
			pCheck->setChecked(true);
		else
			pCheck->setChecked(false);
		m_listWidget->setItemWidget(pListItem, pCheck);
	}
	this->setModel(m_listWidget->model());
	this->setView(m_listWidget);
	this->setLineEdit(m_lineEdit);
	m_lineEdit->setText("please select...");
	connect(m_lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onComboTextChanged(const QString &)));
	onComboCheckStateChanged(0);
}

void QSelectCombo::updateOptions(QStringList options)
{
	QStringList curselected = getSelectItems();
	m_listWidget->clear();
	for (int i = 0; i < options.count(); i++)
	{
		QListWidgetItem *pListItem = new QListWidgetItem(m_listWidget);
		m_listWidget->addItem(pListItem);
		pListItem->setData(Qt::UserRole, i);
		QCheckBox *pCheck = new QCheckBox(this);
		pCheck->setText(options[i]);
		connect(pCheck, SIGNAL(stateChanged(int)), this, SLOT(onComboCheckStateChanged(int)));
		if (curselected.indexOf(options[i]) >= 0)
			pCheck->setChecked(true);
		else
			pCheck->setChecked(false);
		m_listWidget->setItemWidget(pListItem, pCheck);
	}
}

QStringList QSelectCombo::getSelectItems()
{
	if (m_listWidget == NULL)
		return QStringList();
	QStringList selectItems;
	for (int i = 0; i < m_listWidget->count(); i++)
	{
		QCheckBox *pCheck = (QCheckBox *)m_listWidget->itemWidget(m_listWidget->item(i));
		if (pCheck->isChecked())
			selectItems.append(pCheck->text());
	}
	return selectItems;
}


void QSelectCombo::onComboCheckStateChanged(int state)
{
	m_bSelected = true;
	QString strSelectedData("");
	m_strSelectedText.clear();
	QObject *object = QObject::sender();
	QCheckBox *pSenderCheckBox = (QCheckBox *)object;
	int nCount = m_listWidget->count();
	for (int i = 0; i < nCount; ++i)
	{
		QListWidgetItem *pItem = m_listWidget->item(i);
		QWidget *pWidget = m_listWidget->itemWidget(pItem);
		if (pWidget == NULL)
			return;
		QCheckBox *pCheckBox = (QCheckBox *)pWidget;
		if (pCheckBox->isChecked())
		{
			QString strText = pCheckBox->text();
			strSelectedData.append(strText).append(";");
		}
		//所点击的复选框
		if (pSenderCheckBox == pCheckBox)
		{
			int nData = pItem->data(Qt::UserRole).toInt();
		}
	}
	if (strSelectedData.endsWith(";"))
		strSelectedData.remove(strSelectedData.count() - 1, 1);
	if (!strSelectedData.isEmpty())
	{
		//ui.comboBox->setEditText(strSelectedData);
		m_strSelectedText = strSelectedData;
		m_lineEdit->setText(strSelectedData);
		m_lineEdit->setToolTip(strSelectedData);
	}
	else
	{
		m_lineEdit->clear();
		//ui.comboBox->setEditText("");
	}
	m_bSelected = false;
}

void QSelectCombo::onComboTextChanged(const QString &text)
{
	if (!m_bSelected)
	{
		m_lineEdit->setText(text);
	}
}

void QSelectCombo::hidePopup()
{
    if (m_listWidget != NULL)
        m_listWidget->scrollToTop();
    QComboBox::hidePopup();
}
