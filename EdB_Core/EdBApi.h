#pragma once


#ifndef BUILD_STATIC
# if defined(EDBAPI_LIB)
#define EDBAPI_EXPORT __declspec(dllexport)
# else
#define EDBAPI_EXPORT __declspec(dllimport)
# endif
#else
#define EDBAPI_EXPORT
#endif


#ifdef __cplusplus
extern "C" {
#endif
	EDBAPI_EXPORT int EdB_Login(const char* IP, int Port, const char* Version, const char* User, const char* Password, const char* TXPass, const char* Yyb);
	EDBAPI_EXPORT const char* EdB_QueryData(const char* User, int ClientID, int Category);
	EDBAPI_EXPORT const char* EdB_SendOrder(int ClientID, int Fx, const char *User, const char *Gddm, const char* Gpdm, int Quantity, float Price);
	EDBAPI_EXPORT const char* EdB_GetPrice(int ClientID, const char * Gpdm);
	EDBAPI_EXPORT const char* EdB_CancelOrder(int ClientID, const char * User, const char * Bho, int Jys);

#ifdef __cplusplus
}
#endif
