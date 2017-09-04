#ifndef __PASSENCRYPT_H__
#define __PASSENCRYPT_H__

#include "define.h"
#include "exception.h"

namespace MLS
{

class PasswdCrypt
{
private:
	string	_sKeyData;

protected:
	bool	ClientMacAddress(const string& sDevice, string& sIP, string& sMacAddress);
	char	HexValue(char c);
	
	void	DesECBEncode(const string& sKeyHex, const string& sDecode, string& sEncode);
	void	DesECBDecode(const string& sKeyHex, const string& sEncode, string& sDecode);
	
public:
	bool	Encrypt(const string& sDecode, string& sRtEncode);
	bool 	Decrypt(const string& sEncode, string& sRtDecode);

	PasswdCrypt(const string& strKey = "");
};

};

#endif // __PASSENCRYPT_H__
