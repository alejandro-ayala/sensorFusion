
#include <iostream>
#include <exception>
using namespace std;

#define ASSERT_TEC_NOT_NULL
#define THROW_FUNC_EXCEPT
#define ASSERT_TECH

enum ERROR_CODE {
	TLS_SERVER_CONNECTION,
	TLS_SERVER_HANDSHAKE,
	TLS_PARSE_CERTF,
	TLS_VERIFY_CHAIN_CERTF,
	TLS_INVALID_MSG_SIZE,
	TLS_INCOMPLETE_SEND,
	TLS_SEND_DATA,
	TLS_CONNECTION_LOST,
	TLS_CLOSE_CONNECTION,
	TLS_INCOMPLETE_RESPONSE,
	TLS_RETRIEVE_ROOT_CA,
	TLS_VALIDATE_ROOT_CA,
	TLS_CERTF_PERIOD,
	NULL_PARAMETER,
	HTTP_INCOMPLETE_SEND
};



struct FunctionalError : public exception {
   const char * what () const throw () {
      return "C++ Exception";
   }
};
