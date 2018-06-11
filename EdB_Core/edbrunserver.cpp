#include "edbrunserver.h"
#include "EdBApi.h"
#include <QMutex>

QMutex mutex;

EdBRunServer::EdBRunServer(QString serverName)
	: m_server(NULL)
{
	m_serverName = serverName;
	m_server = new QLocalServer(this);
	QObject::connect(m_server, SIGNAL(newConnection()), this, SLOT(serverNewConnectionHandler()));
}

EdBRunServer::~EdBRunServer()
{
}

void EdBRunServer::RunServer()
{
	qDebug() << "Run Server ok";

	QLocalServer::removeServer(m_serverName);
	bool ok = m_server->listen(m_serverName);
	if (!ok)
	{
		// TODO:
	}
}

void EdBRunServer::serverNewConnectionHandler()
{
	qDebug() << "New Connection";

	QLocalSocket* socket = m_server->nextPendingConnection();
	QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyReadHandler()));
	QObject::connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void EdBRunServer::socketReadyReadHandler()
{
	QLocalSocket* socket = static_cast<QLocalSocket*>(sender());
	if (socket)
	{
		QTextStream stream(socket);
		QString readStr = stream.readAll();
		QStringList readStrList = readStr.split("&&&");
		QString response;
		mutex.lock();
		if (readStrList[0] == "EdB_Login")
		{
			int ret = EdB_Login(readStrList[1].toStdString().c_str(), readStrList[2].toInt(), readStrList[3].toStdString().c_str(), readStrList[4].toStdString().c_str(), readStrList[5].toStdString().c_str(), readStrList[6].toStdString().c_str(), readStrList[7].toStdString().c_str());
			response = QString::number(ret);
			//response = "1";
		}
		else if (readStrList[0] == "EdB_QueryData")
		{
			response = QString::fromLocal8Bit(EdB_QueryData(readStrList[1].toStdString().c_str(), readStrList[2].toInt(), readStrList[3].toInt()));
			//response = QString::fromStdWString(LR"aa(12|24|�ɽ�����|֤ȯ����|֤ȯ����|�ɽ��۸�|�ɽ�����|ҵ������|�������|ʣ����|�ɶ�����|����|��ע|������Ϣ|20170109|||0|0|ת���ת��|0|0||0|ת���ת��������ˮ��:1426||20170109|||0|0|ת���ת��|0|0||0|ת���ת�������ˮ��:1426||20170124|||0|0|����ת��|2892.82|2892.82||0|||20170124|||0|0|����תȡ|-681.68|2211.14||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�||20170124|600802|����ˮ��|11.030|200.00|֤ȯ����|-2211.04|0.10|A723223798|0|֤ȯ����||20170125|600802|����ˮ��|10.480|-200.00|֤ȯ����|2088.86|2088.96|A723223798|0|֤ȯ����||20170126|||0|0|����תȡ|-2088.96|0||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�||20170321|||0|0|��Ϣ�鱾|0.02|0.02||0|��Ϣ�鱾: �鱾��ϢΪ 0.02correct_balance=0||20171016|||0|0|����ת��|40000.00|40000.02||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�correct_balance=0||20171016|||0|0|����תȡ|-40000.00|0.02||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�correct_balance=40000||20171017|||0|0|����ת��|40000.00|40000.02||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�correct_balance=0||20171017|||0|0|����תȡ|-22418.41|17581.61||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�correct_balance=22418.41||20171017|300587|�����ɷ�|30.470|100.00|֤ȯ����|-3052.00|14529.61|0177778780|0|֤ȯ����||20171017|300587|�����ɷ�|30.470|100.00|֤ȯ����|-3052.00|11477.61|0177778780|0|֤ȯ����||20171017|002285|������|11.970|300.00|֤ȯ����|-3596.00|7881.61|0177778780|0|֤ȯ����||20171017|000560|���ٴ��|12.280|100.00|֤ȯ����|-1233.00|6648.61|0177778780|0|֤ȯ����||20171017|002285|������|11.970|300.00|֤ȯ����|-3596.00|3052.61|0177778780|0|֤ȯ����||20171017|300587|�����ɷ�|30.470|100.00|֤ȯ����|-3052.00|0.61|0177778780|0|֤ȯ����||20171018|000560|���ٴ��|12.250|-100.00|֤ȯ����|1218.77|1219.38|0177778780|0|֤ȯ����||20171018|002285|������|12.400|-600.00|֤ȯ����|7425.12|8644.50|0177778780|0|֤ȯ����||20171018|300587|�����ɷ�|31.250|-300.00|֤ȯ����|9356.25|18000.75|0177778780|0|֤ȯ����||20171019|||0|0|����תȡ|-18000.00|0.75||0|���з�����[ ]������Ϣ[0000 ���׳ɹ�]|ת�˳ɹ�correct_balance=18000||20171124|744183|�������|0|0|�깺���|0|0.75|A723223798|0|ʧ��ԭ��:Υ���ظ�;�����:02||20171221|||0|0|��Ϣ�鱾|0.18|0.93||0|��Ϣ�鱾: �鱾��ϢΪ 0.18correct_balance=0||)aa");
		}
		else if (readStrList[0] == "EdB_SendOrder")
		{
			response = QString::fromLocal8Bit(EdB_SendOrder(readStrList[1].toInt(), readStrList[2].toInt(), readStrList[3].toStdString().c_str(), readStrList[4].toStdString().c_str(), readStrList[5].toStdString().c_str(), readStrList[6].toInt(), readStrList[7].toFloat()));
		}
		else if (readStrList[0] == "EdB_CancelOrder")
		{
			response = QString::fromLocal8Bit(EdB_CancelOrder(readStrList[1].toInt(), readStrList[2].toStdString().c_str(), readStrList[3].toStdString().c_str(), readStrList[4].toInt()));
		}
		else if (readStrList[0] == "EdB_GetPrice")
		{
			response = QString::fromLocal8Bit(EdB_GetPrice(readStrList[1].toInt(), readStrList[2].toStdString().c_str()));
		}
		mutex.unlock();
		socket->write(response.toLocal8Bit().data());
		socket->flush();
	}

	// ���ص��ͻ��˵�void sendMessage������m_socket->waitForReadyRead()֮��Ĳ���
}