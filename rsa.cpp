#include "rsa.h"
#include "stdio.h"
#include <string.h>

RSA* RSA::instance = NULL;

RSA* RSA::getInstance()
{
	if(!instance)
		instance = new RSA();

	return instance;
}

RSA::RSA()
{
	m_keySet = false;
	mpz_init2(m_mod, 1024);
	mpz_init2(m_e, 1024);
}

RSA::~RSA()
{
	mpz_clear(m_mod);
	mpz_clear(m_e);
}

//m as binary
//e as string
void RSA::setPublicKey(char* m, const std::string& e)
{
	mpz_import(m_mod, 128, 1, 1, 0, 0, m);
	mpz_set_str(m_e, e.c_str(), 10);
}

bool RSA::encrypt(char* msg, long size)
{	
	mpz_t plain,c;
	mpz_init2(plain, 1024);
	mpz_init2(c, 1024);

	mpz_import(plain, 128, 1, 1, 0, 0, msg);

	mpz_powm(c, plain, m_e, m_mod);
	
		
	size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
	memset(msg, 0, 128 - count);
	mpz_export(&msg[128 - count], NULL, 1, 1, 0, 0, c);
	
	mpz_clear(c);
	mpz_clear(plain);
	return true;
}
