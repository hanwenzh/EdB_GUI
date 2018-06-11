#pragma once

#include <QWidget>
#include "ui_dataresultwidget.h"

class DataResultWidget : public QWidget
{
	Q_OBJECT

public:
	DataResultWidget(QWidget *parent = Q_NULLPTR);
	~DataResultWidget();
	Ui::DataResultWidget ui;

};
