#ifndef __NETWORK_MESSAGE_H__
#define __NETWORK_MESSAGE_H__

#include "definitions.h"
#include "rsa.h"
#include <QByteArray>

class NetworkMessage
{
	public:
		// constructor/destructor
		NetworkMessage() {Reset();}
		NetworkMessage(QByteArray data)
		{
			Reset();
			m_ReadPos = 1;
			int i = 0;
			foreach(char byte, data)
			{
				m_MsgBuf[i++] = byte;
				m_MsgSize++;
			}

			SkipBytes(2); //message size
		}

		~NetworkMessage() {}

		// resets the internal buffer to an empty message
		void Reset();

		// socket functions
		bool ReadFromSocket(SOCKET socket);
		bool WriteToSocket(SOCKET socket);

		// simply read functions for incoming message
		uint8_t  InspectByte();
		uint8_t  GetByte();
		uint16_t GetU16();
		uint32_t   GetU32();
		std::string    GetString();
		std::string	 GetRaw();

		void setEncryptionState(bool state);
		void setEncryptionKey(const uint32_t* key);

		// skips count unknown/unused bytes in an incoming message
		void SkipBytes(int count);

		// simply write functions for outgoing message
		void AddByte(uint8_t  value);
		void AddU16 (uint16_t value);
		void AddU32 (uint32_t value);

		void AddString(const std::string &value);
		void AddString(const char* value);

		int getMessageLength(){return m_MsgSize;}

		bool RSA_encrypt();

		void setRSAInstance(RSA* rsa);

	protected:
		inline bool canAdd(int size){
			return (size + m_ReadPos < NETWORKMESSAGE_MAXSIZE - 16);
		};

		void XTEA_encrypt();
		void XTEA_decrypt();

		int m_MsgSize;
		int m_ReadPos;

		uint8_t m_MsgBuf[NETWORKMESSAGE_MAXSIZE];

		static bool m_encryptionEnabled;
		static bool m_keyset;
		static uint32_t m_key[4];

		static RSA* m_RSA;
};


#endif
