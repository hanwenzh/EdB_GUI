#ifndef QSELECTCOMBO_H
#define QSELECTCOMBO_H

#include <QComboBox>
#include <QListWidget>
#include <QStringList>
#include <QLineEdit>

//�Զ���ؼ���������ѡ���������
class QSelectCombo : public QComboBox
{
	Q_OBJECT

public:
    explicit QSelectCombo(QWidget *parent = Q_NULLPTR);
	void initCombo(QStringList options , QStringList selectedConfig = QStringList());		//��ʼ��ѡ��
	void updateOptions(QStringList options);
	QStringList getSelectItems();	//��ȡ����ѡ��������Ĵ���
	
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
