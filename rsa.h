#ifndef __OTSERV_RSA_H__
#define __OTSERV_RSA_H__

#include "definitions.h"
#include <string>
#include "stdio.h"
#include "gmp.h"

class RSA
{
	public:
		~RSA();

		static RSA* getInstance();

		void setPublicKey(char* m, const std::string& e);
		bool encrypt(char* msg,long size);

	protected:
		RSA();

		static RSA* instance;
		bool m_keySet;

		mpz_t m_mod, m_e;
};
#endif
