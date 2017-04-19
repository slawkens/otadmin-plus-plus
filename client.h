#ifndef CLIENT_H
#define CLIENT_H

#include "networkmessage.h"
#include "rsa.h"
#include "definitions.h"
#include "statusreader.h"


enum RequestedInfo_t
{
	REQUEST_BASIC_SERVER_INFO 	= 0x01,
	REQUEST_SERVER_OWNER_INFO	= 0x02,
	REQUEST_MISC_SERVER_INFO	= 0x04,
	REQUEST_PLAYERS_INFO		= 0x08,
	REQUEST_SERVER_MAP_INFO		= 0x10,
	REQUEST_EXT_PLAYERS_INFO	= 0x20,
	REQUEST_PLAYER_STATUS_INFO	= 0x40,
	REQUEST_SERVER_SOFTWARE_INFO	= 0x80
};

enum
{
	AP_MSG_LOGIN = 1,
	AP_MSG_ENCRYPTION = 2,
	AP_MSG_KEY_EXCHANGE = 3,
	AP_MSG_COMMAND = 4,
	AP_MSG_PING = 5,
	//
	AP_MSG_HELLO = 1,
	AP_MSG_KEY_EXCHANGE_OK = 2,
	AP_MSG_KEY_EXCHANGE_FAILED = 3,
	AP_MSG_LOGIN_OK = 4,
	AP_MSG_LOGIN_FAILED = 5,
	AP_MSG_COMMAND_OK = 6,
	AP_MSG_COMMAND_FAILED = 7,
	AP_MSG_ENCRYPTION_OK = 8,
	AP_MSG_ENCRYPTION_FAILED = 9,
	AP_MSG_PING_OK = 10,
	AP_MSG_MESSAGE = 11,
	AP_MSG_ERROR = 12,
};

enum
{
	CMD_BROADCAST = 1,
	CMD_CLOSE_SERVER = 2,
	CMD_PAY_HOUSES = 3,
	CMD_OPEN_SERVER = 4,
	CMD_SHUTDOWN_SERVER = 5,
	CMD_RELOAD_SCRIPTS = 6,
	//CMD_PLAYER_INFO = 7,
	//CMD_GETONLINE = 8,
	CMD_KICK = 9,
	//CMD_BAN_MANAGER = 10,
	//CMD_SERVER_INFO = 11,
	//CMD_GETHOUSE = 12,
	CMD_SETOWNER = 13,

	//server
	CMD_SERVER_OPEN,
	CMD_SERVER_CLOSE,
	CMD_SERVER_SHUTDOWN,
	CMD_SERVER_SAVE,
	CMD_SERVER_CLEANMAP,

	//reloading
/*
	CMD_RELOAD_ACTIONS,
	CMD_RELOAD_CONFIG,
	CMD_RELOAD_CREATUREEVENTS,
	CMD_RELOAD_GAMESERVERS,
	CMD_RELOAD_GLOBALEVENTS,
	CMD_RELOAD_HIGHSCORES,
	CMD_RELOAD_HOUSEPRICES,
	CMD_RELOAD_ITEMS,
	CMD_RELOAD_MONSTERS,
	CMD_RELOAD_MOVEEVENTS,
	CMD_RELOAD_NPCS,
	CMD_RELOAD_OUTFITS,
	CMD_RELOAD_QUESTS,
	CMD_RELOAD_RAIDS,
	CMD_RELOAD_SPELLS,
	CMD_RELOAD_STAGES,
	CMD_RELOAD_TALKACTIONS,
	CMD_RELOAD_VOCATIONS,
	CMD_RELOAD_WEAPONS,
	CMD_RELOAD_MODS,
	CMD_RELOAD_ALL,

	//houses
	CMD_HOUSES_PAY,
	CMD_HOUSES_CLEANINACTIVE,
	CMD_HOUSE_SETOWNER,

	//pvp
	CMD_WORLDTYPE_PVP,
	CMD_WORLDTYPE_NOPVP,
	CMD_WORLDTYPE_PVPE,

	//players
	CMD_PLAYER_KICK,
	CMD_PLAYER_BAN,

	//other
	CMD_EXECUTERAID,
	CMD_BROADCAST,

	CMD_GETBANINFO,
	CMD_GETPLAYERINFO,

	CMD_GETLIST_PLAYERS,
	CMD_GETLIST_BANS,

	CMD_KICK = 9,*/
};

enum
{
	REQUIRE_LOGIN = 1,
	REQUIRE_ENCRYPTION = 2,
};

enum
{
	ENCRYPTION_RSA1024XTEA = 1,
};

enum connectionError_t
{
	CONNECT_SUCCESS = 0,
	FAIL_CONNECT = 1,
	FAIL_WRITE = 2,
	FAIL_READ = 3
};


class Client
{
	public:
		Client(QWidget* _parent = 0, QString _host = "localhost", int _port = 7171, QString _password = "test");
		virtual ~Client() {}

		connectionError_t getServerData(StatusInfo_t& statusInfo);
		connectionError_t getServerInfo(RequestedInfo_t type, QByteArray &result)
		{
			return getServerInfo(host, port, type, result);
		}

		static connectionError_t getServerInfo(QString host, int port, RequestedInfo_t type, QByteArray &result);

		bool sendCommand(char commandByte, char* command);
		bool sendMsg(NetworkMessage& msg, uint32_t* key = NULL);

		virtual void consoleLog(QString message) {}

		bool isConnected() const {return m_connected;}
		void setConnected(bool v) {m_connected = v;}

		QString getHost() const {return host;}
		void setHost(QString _host) {host = _host;}

		QString getPassword() const {return password;}
		void setPassword(QString _password) {password = _password;}

		uint16_t getPort() const {return port;}
		void setPort(int _port) {port = _port;}

		QString getLastError() {return lastError;}
		void setLastError(QString err) {lastError = err;}

		bool connect();
		virtual bool disconnect();

		bool ping();

		bool commandBroadcast(QString message);
		bool commandKick(QString name);
		bool commandSetOwner(int houseId, QString owner);
		bool commandOpenServer();
		bool commandCloseServer();
		bool commandPayHouses();
		bool commandSaveServer();
		bool commandShutdown();

	private:
		QWidget* parent;
		bool m_connected;
		SOCKET m_socket;

		QString host, password;
		uint16_t port;

		QString lastError;
};
#endif
