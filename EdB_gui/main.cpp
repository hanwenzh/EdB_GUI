#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include "useroperatewidget.h"

int main(int argc, char *argv[])
{
	qApp->setAttribute(Qt::AA_EnableHighDpiScaling, true);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
