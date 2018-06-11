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
			//response = QString::fromStdWString(LR"aa(12|24|成交日期|证券代码|证券名称|成交价格|成交数量|业务名称|发生金额|剩余金额|股东代码|币种|备注|保留信息|20170109|||0|0|转存管转出|0|0||0|转存管转出关联流水号:1426||20170109|||0|0|转存管转入|0|0||0|转存管转入关联流水号:1426||20170124|||0|0|银行转存|2892.82|2892.82||0|||20170124|||0|0|银行转取|-681.68|2211.14||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功||20170124|600802|福建水泥|11.030|200.00|证券买入|-2211.04|0.10|A723223798|0|证券买入||20170125|600802|福建水泥|10.480|-200.00|证券卖出|2088.86|2088.96|A723223798|0|证券卖出||20170126|||0|0|银行转取|-2088.96|0||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功||20170321|||0|0|利息归本|0.02|0.02||0|利息归本: 归本利息为 0.02correct_balance=0||20171016|||0|0|银行转存|40000.00|40000.02||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功correct_balance=0||20171016|||0|0|银行转取|-40000.00|0.02||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功correct_balance=40000||20171017|||0|0|银行转存|40000.00|40000.02||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功correct_balance=0||20171017|||0|0|银行转取|-22418.41|17581.61||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功correct_balance=22418.41||20171017|300587|天铁股份|30.470|100.00|证券买入|-3052.00|14529.61|0177778780|0|证券买入||20171017|300587|天铁股份|30.470|100.00|证券买入|-3052.00|11477.61|0177778780|0|证券买入||20171017|002285|世联行|11.970|300.00|证券买入|-3596.00|7881.61|0177778780|0|证券买入||20171017|000560|昆百大Ａ|12.280|100.00|证券买入|-1233.00|6648.61|0177778780|0|证券买入||20171017|002285|世联行|11.970|300.00|证券买入|-3596.00|3052.61|0177778780|0|证券买入||20171017|300587|天铁股份|30.470|100.00|证券买入|-3052.00|0.61|0177778780|0|证券买入||20171018|000560|昆百大Ａ|12.250|-100.00|证券卖出|1218.77|1219.38|0177778780|0|证券卖出||20171018|002285|世联行|12.400|-600.00|证券卖出|7425.12|8644.50|0177778780|0|证券卖出||20171018|300587|天铁股份|31.250|-300.00|证券卖出|9356.25|18000.75|0177778780|0|证券卖出||20171019|||0|0|银行转取|-18000.00|0.75||0|银行返回码[ ]返回信息[0000 交易成功]|转账成功correct_balance=18000||20171124|744183|生益配号|0|0|申购配号|0|0.75|A723223798|0|失败原因:违规重复;错误号:02||20171221|||0|0|利息归本|0.18|0.93||0|利息归本: 归本利息为 0.18correct_balance=0||)aa");
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

	// 返回到客户端的void sendMessage方法，m_socket->waitForReadyRead()之后的操作
}