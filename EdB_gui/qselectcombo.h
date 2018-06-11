#ifndef QSELECTCOMBO_H
#define QSELECTCOMBO_H

#include <QComboBox>
#include <QListWidget>
#include <QStringList>
#include <QLineEdit>

//自定义控件：包含多选框的下拉框
class QSelectCombo : public QComboBox
{
	Q_OBJECT

public:
    explicit QSelectCombo(QWidget *parent = Q_NULLPTR);
	void initCombo(QStringList options , QStringList selectedConfig = QStringList());		//初始化选项
	void updateOptions(QStringList options);
	QStringList getSelectItems();	//获取所有选项所代表的代码
	
private:
	bool m_bSelected;
	QString m_strSelectedText;
	QListWidget *m_listWidget;
	QLineEdit *m_lineEdit;
    void hidePopup();

private slots:
	void onComboCheckStateChanged(int);
	void onComboTextChanged(const QString &text);
};

#endif // QSELECTCOMBO_H
