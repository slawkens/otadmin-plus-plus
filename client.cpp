#include <QtWidgets>
#include <QtNetwork>
#include <QTcpSocket>

#include <iostream>
#include <stdio.h>
#include <list>
#include <sstream>
#include <string>

#include "client.h"
#include "statusreader.h"
#include "networkmessage.h"
#include "rsa.h"
#include "definitions.h"

#if defined WIN32 || defined __WINDOWS__
#define ERROR_SOCKET WSAGetLastError()
#else
#include <errno.h>
#define ERROR_SOCKET errno
#endif


Client::Client(QWidget* _parent, QString _host, int _port, QString _password)
{
	parent = _parent;

#if defined WIN32 || defined __WINDOWS__
	WSADATA wsd;
	if(WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return;
#endif

	m_socket = SOCKET_ERROR, m_connected  = false;;
	host = _host, port = _port, password = _password;
}

bool Client::sendCommand(char commandByte, char* command)
{
	if(!isConnected())
	{
		consoleLog("[sendCommand] NOT connected!");
		setLastError("NOT Connected!");
		return false;
	}

	NetworkMessage msg;
	msg.AddByte(AP_MSG_COMMAND);
	msg.AddByte(commandByte);
	if(command)
		msg.AddString(command);

	if(!sendMsg(msg))
	{
		consoleLog("[sendCommand] Error while sending command!");
		setLastError("Error while sending command!");
		return false;
	}

	char ret_code = msg.GetByte();
	if(ret_code == AP_MSG_COMMAND_OK)
		return true;

	if(ret_code == AP_MSG_COMMAND_FAILED)
	{
		std::string error_desc = msg.GetString();
		consoleLog("[sendCommand] Received error: " + QString::fromStdString(error_desc));
		setLastError("Received error: " + QString::fromStdString(error_desc));
		return false;
	}

	consoleLog("[sendCommand] Server returned unknown code");
	setLastError("Server returned unknown code");
	return false;
}

bool Client::sendMsg(NetworkMessage& msg, uint32_t* key/*= NULL*/)
{
#if defined WIN32 || defined __WINDOWS__
	unsigned long mode = 0;
	ioctlsocket(m_socket, FIONBIO, &mode);
#else
	int flags = fcntl(m_socket, F_GETFL);
	fcntl(m_socket, F_SETFL, flags & (~O_NONBLOCK));
#endif
	bool ret = true;

	if(!msg.WriteToSocket(m_socket))
	{
		consoleLog("[sendMsg] Error while sending - ERROR_NUMBER: " + QString::number(ERROR_SOCKET));
		setLastError("Error while sending - ERROR_NUMBER: " + QString::number(ERROR_SOCKET));
		ret = false;
	}

	msg.Reset();

	if(ret)
	{
		if(key)
		{
			msg.setEncryptionState(true);
			msg.setEncryptionKey(key);
		}

		if(!msg.ReadFromSocket(m_socket))
		{
			consoleLog("[sendMsg] Error while reading - ERROR_NUMBER: " + QString::number(ERROR_SOCKET));
			setLastError("Error while reading - ERROR_NUMBER: " + QString::number(ERROR_SOCKET));
			ret = false;
		}
		else
		{
			char ret_code = msg.InspectByte();
			if(ret_code == AP_MSG_ERROR)
			{
				msg.GetByte();
				std::string error_desc = msg.GetString();
				consoleLog("[sendMsg] MSG_ERROR: " + QString::fromStdString(error_desc));
				setLastError("MSG_ERROR: " + QString::fromStdString(error_desc));
				ret = false;
			}
		}
	}

#if defined WIN32 || defined __WINDOWS__
	mode = 1;
	ioctlsocket(m_socket, FIONBIO, &mode);
#else
	flags = fcntl(m_socket, F_GETFL);
	fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
#endif
	return ret;
}

bool Client::connect()
{
	if(isConnected())
	{
		consoleLog("[connect] Already connected!");
		setLastError("Already connected!");
		return false;
	}

	char password[128];
    strcpy(password, getPassword().toLatin1().data());

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	uint32_t remoteIP = inet_addr(getHost().toStdString().c_str());
	if(remoteIP == INADDR_NONE)
	{
		struct hostent* hp = gethostbyname(getHost().toStdString().c_str());
		if(hp != 0)
			remoteIP = *(long*)hp->h_addr;
		else
		{
			closesocket(m_socket);
			consoleLog("[connect] Cannot resolve server:" + getHost() + " - " + QString::number(ERROR_SOCKET));
			setLastError("Cannot resolve server:" + getHost() + " - " + QString::number(ERROR_SOCKET));
			return false;
		}
	}

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = remoteIP;
	serveraddr.sin_port = htons(getPort());

	if(::connect(m_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		consoleLog("[connect] Cannot connect to server: " + getHost() + " - " +  QString::number(ERROR_SOCKET));
		setLastError("Cannot connect to server: " + getHost() + " - " +  QString::number(ERROR_SOCKET));
		return false;
	}

	uint8_t ip[4];
	*(uint32_t*)&ip = remoteIP;
	QString tmp = QString::number(ip[0]) + "." + QString::number(ip[1]) + "." + QString::number(ip[2]) + "." + QString::number(ip[3]);
	consoleLog("Connected to server. [IP: " + tmp + "]");

	NetworkMessage msg;
	msg.AddByte(0xFE);
	if(!msg.WriteToSocket(m_socket))
	{
		closesocket(m_socket);
		consoleLog("[connect] Error while sending first byte - " + QString::number(ERROR_SOCKET));
		setLastError("Error while sending first byte - " + QString::number(ERROR_SOCKET));
		return false;
	}

	msg.Reset();

	//read server hello
	if(!msg.ReadFromSocket(m_socket))
	{
		closesocket(m_socket);
		consoleLog("[connect] Error while reading hello - " + QString::number(ERROR_SOCKET) + "(Probably OTAdmin disabled on remote server or your ip is not allowed.)");
		setLastError("Error while reading hello - " + QString::number(ERROR_SOCKET) + "(Probably OTAdmin disabled on remote server or your ip is not allowed.)");
		return false;
	}

	char byte = msg.GetByte();
	if(byte != AP_MSG_HELLO)
	{
		qDebug() << " NOT HELLO #SLAW DEBUG = " << byte;
		closesocket(m_socket);
		consoleLog("[connect] No valid server hello!");
		setLastError("No valid server hello!");
		return false;
	}

	msg.GetU32();
	std::string strversion = msg.GetString();
	uint16_t security = msg.GetU16();
	uint32_t options = msg.GetU32();
	if(security & REQUIRE_ENCRYPTION)
	{
		strversion = strversion + " encryption";
		if(options & ENCRYPTION_RSA1024XTEA)
			strversion = strversion + "(RSA1024XTEA)";
		else
			strversion = strversion + "(Not supported)";
	}

	if(security & REQUIRE_LOGIN)
		strversion = strversion + " login";

	consoleLog("Hello from " + QString::fromStdString(strversion));

	if(security & REQUIRE_ENCRYPTION)
	{
		if(options & ENCRYPTION_RSA1024XTEA)
		{
			//get public key
			msg.Reset();
			msg.AddByte(AP_MSG_KEY_EXCHANGE);
			msg.AddByte(ENCRYPTION_RSA1024XTEA);

			if(!sendMsg(msg))
			{
				closesocket(m_socket);
				consoleLog("[connect] Error while getting public key");
				setLastError("Error while getting public key");
				return false;
			}

			char ret_code = msg.GetByte();
			if(ret_code == AP_MSG_KEY_EXCHANGE_OK)
			{
				consoleLog("Key exchange OK");
			}
			else if(ret_code == AP_MSG_KEY_EXCHANGE_FAILED)
			{
				std::string error_desc = msg.GetString();
				closesocket(m_socket);
				consoleLog("[connect] Cannot get public key: " + QString::fromStdString(error_desc));
				setLastError("Cannot get public key: " + QString::fromStdString(error_desc));
				return false;
			}
			else
			{
				closesocket(m_socket);
				consoleLog("[connect] Unknown response to key exchange request");
				setLastError("Unknown response to key exchange request");
				return false;
			}

			unsigned char key_type = msg.GetByte();
			if(key_type != ENCRYPTION_RSA1024XTEA)
			{
				closesocket(m_socket);
				consoleLog("[connect] Invalid key returned");
				setLastError("Invalid key returned");
				return false;
			}

			uint32_t rsa_mod[32];
			for(unsigned int i = 0; i < 32; ++i){
				rsa_mod[i] = msg.GetU32();
			}
			RSA::getInstance()->setPublicKey((char*)rsa_mod, "65537");

			uint32_t random_key[32];
			for(unsigned int i = 0; i < 32; ++i)
				random_key[i] = rand() << 16 ^ rand();

			msg.setRSAInstance(RSA::getInstance());
			msg.Reset();
			msg.AddByte(AP_MSG_ENCRYPTION);
			msg.AddByte(ENCRYPTION_RSA1024XTEA);
			//build the 128 bytes block
			msg.AddByte(0);
			for(unsigned int i = 0; i < 31; ++i){
				msg.AddU32(random_key[i]);
			}
			msg.AddByte(0);
			msg.AddByte(0);
			msg.AddByte(0);
			//
			msg.RSA_encrypt();

			if(!sendMsg(msg, random_key))
			{
				closesocket(m_socket);
				consoleLog("[connect] Error while sending private key");
				setLastError("Error while sending private key");
				return false;
			}

			ret_code = msg.GetByte();
			if(ret_code == AP_MSG_ENCRYPTION_FAILED)
			{
				std::string error_desc = msg.GetString();
				closesocket(m_socket);
				consoleLog("[connect] Cannot set private key: " + QString::fromStdString(error_desc));
				setLastError("Cannot set private key: " + QString::fromStdString(error_desc));
				return false;
			}
			else if(ret_code != AP_MSG_ENCRYPTION_OK)
			{
				closesocket(m_socket);
				consoleLog("[connect] Unknown response to set private key request");
				setLastError("Unknown response to set private key request");
				return false;
			}

		}
		else
		{
			closesocket(m_socket);
			consoleLog("[connect] Cannot initiate encryption");
			setLastError("Cannot initiate encryption");
			return false;
		}
	}


	//login
	if(security & REQUIRE_LOGIN)
	{
		msg.Reset();
		msg.AddByte(AP_MSG_LOGIN);
		msg.AddString(std::string(password));

		if(!sendMsg(msg))
		{
			closesocket(m_socket);
			consoleLog("[connect] Error while sending login");
			setLastError("Error while sending login");
			return false;
		}

		char ret_code = msg.GetByte();
		if(ret_code != AP_MSG_LOGIN_OK)
		{
			if(ret_code == AP_MSG_LOGIN_FAILED)
			{
				std::string error_desc = msg.GetString();
				consoleLog("[connect] Cannot login: " + QString::fromStdString(error_desc));
				setLastError("Cannot login: " + QString::fromStdString(error_desc));
			}
			else
			{
				consoleLog("[connect] Unknown response to login request");
				setLastError("Unknown response to login request");
			}

			closesocket(m_socket);
			return false;
		}
	}

	consoleLog("Login OK");
	m_connected = true;
	return true;
}

bool Client::disconnect()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	closesocket(m_socket);
	m_socket = SOCKET_ERROR;

	m_connected = false;

	//delete this;
	return true;
}

bool Client::commandBroadcast(QString message)
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(message.isEmpty() || message.size() > 127)
	{
		consoleLog("[broadcast] Invalid message (Cannot be empty or longer than 127 chars)");
		setLastError("Invalid message (Cannot be empty and longer than 127 chars)");
		return false;
	}

	char param[128];
    strcpy(param, message.toLatin1().data());

	if(!sendCommand(CMD_BROADCAST, param))
	{
		consoleLog("[broadcast] Error sending broadcast");
		setLastError("Error sending broadcast");
		return false;
	}

	consoleLog("[broadcast] Succesfully broadcasted message '" + message + "'");
	return true;
}

bool Client::commandKick(QString name)
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(name.isEmpty() || name.size() > 30)
	{
		consoleLog("[kick] Invalid player name (Cannot be empty or longer than 30 characters)");
		setLastError("Invalid player name (Cannot be empty or longer than 30 characters)");
		return false;
	}

	char param[128];
    strcpy(param, name.toLatin1().data());

	if(!sendCommand(CMD_KICK, param))
	{
		consoleLog("[kick] Error sending kick");
		setLastError("Error sending kick");
		return false;
	}

	consoleLog("[kick] Kicked player '" + name + "'");
	return true;
}

bool Client::commandSetOwner(int houseId, QString owner)
{
	consoleLog("TODO!");
	return true;
}

bool Client::commandOpenServer()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(!sendCommand(CMD_OPEN_SERVER, NULL))
	{
		consoleLog("[openserver] Error in open server");
		setLastError("Error in open server");
		return false;
	}

	return true;
}

bool Client::commandCloseServer()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(!sendCommand(CMD_CLOSE_SERVER, NULL))
	{
		consoleLog("[closeserver] Error in close server");
		setLastError("Error in close server");
		return false;
	}

	return true;
}

bool Client::commandPayHouses()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(!sendCommand(CMD_PAY_HOUSES, NULL))
	{
		consoleLog("[payhouses] Error in payhouses");
		setLastError("Error in payhouses");
		return false;
	}

	return true;
}

bool Client::commandSaveServer()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(!sendCommand(CMD_SERVER_SAVE, NULL))
	{
		consoleLog("[saveserver] Error in serversave");
		setLastError("Error in serversave");
		return false;
	}

	return true;
}

bool Client::commandShutdown()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	if(!sendCommand(CMD_SHUTDOWN_SERVER, NULL))
	{
		consoleLog("[shutdown] Error in server shutdown");
		setLastError("Error in server shutdown");
		return false;
	}

	return true;
}

bool Client::ping()
{
	if(!isConnected())
	{
		setLastError("Not connected!");
		return false;
	}

	NetworkMessage msg;
	msg.AddByte(AP_MSG_PING);

	if(!sendMsg(msg))
	{
		consoleLog("[ping] Error while sending ping");
		setLastError("Error while sending ping");
		return false;
	}

	char ret_code = msg.GetByte();
	if(ret_code != AP_MSG_PING_OK)
	{
		consoleLog("[ping] Invalid respons for ping");
		setLastError("Invalid respons for ping");
		return false;
	}

	return true;
}

connectionError_t Client::getServerData(StatusInfo_t& statusInfo)
{
	connectionError_t ret = CONNECT_SUCCESS;
	QTcpSocket* clientSocket = new QTcpSocket;
	clientSocket->connectToHost(host, port);
	if(clientSocket->waitForConnected(400))
	{
		QByteArray packet;
		packet.resize(7);

		//packet headers
		packet[0] = packet.size() - 1;
		packet[1] = 0x00;

		packet[2] = 0xFF; //status protocol
		packet[3] = 0xFF; //XML server info
		packet[4] = 'i';
		packet[5] = 'n';
		packet[6] = 'f';
		packet[7] = 'o';
		clientSocket->write(packet);
		if(clientSocket->waitForBytesWritten(400))
		{
			if(clientSocket->waitForReadyRead(1000))
			{
				QByteArray XMLData = clientSocket->readAll();
				StatusReader handler;
				QXmlInputSource source;
				source.setData(XMLData);
				QXmlSimpleReader reader;
				reader.setContentHandler(&handler);
				reader.parse(source);

				statusInfo = handler.getStatusInfo();
			}
			else
				ret = FAIL_READ;
		}
		else
			ret = FAIL_WRITE;
	}
	else
		ret = FAIL_CONNECT;

	clientSocket->close();
	return ret;
}

connectionError_t Client::getServerInfo(QString host, int port, RequestedInfo_t type, QByteArray &result)
{
	connectionError_t ret = CONNECT_SUCCESS;
	QTcpSocket* clientSocket = new QTcpSocket;
	clientSocket->connectToHost(host, port);
	if(clientSocket->waitForConnected(400))
	{
		QByteArray packet;
		packet.resize(4);

		//packet headers
		packet[0] = packet.size() - 1;
		packet[1] = 0x00;

		packet[2] = 0xFF; //status protocol
		packet[3] = 0x01; //get info
		packet[4] = type;
		clientSocket->write(packet);
		if(clientSocket->waitForBytesWritten(400))
		{
			if(clientSocket->waitForReadyRead(1000))
				result = clientSocket->readAll();
			else
				ret = FAIL_READ;
		}
		else
			ret = FAIL_WRITE;
	}
	else
		ret = FAIL_CONNECT;

	clientSocket->close();
	return ret;
}
