#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "useroperatewidget.h"
#include <QPushButton>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

private:
	Ui::MainWindow ui;
	void rollBackRecord();
	

public slots:
	void onButtonAddUserClicked();
	void onTabCloseRequested(int);

protected:
	void closeEvent(QCloseEvent *event);
	bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result);
};
