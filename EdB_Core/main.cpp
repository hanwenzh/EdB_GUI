#include <QtCore/QCoreApplication>
#include "edbrunserver.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	if (argc != 2)
		return 0;
	QString serverName = argv[1];
	EdBRunServer server(serverName);
	server.RunServer();
	return a.exec();
}
