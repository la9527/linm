#include "config.h"

#if defined(__unix__)
	#include <sys/socket.h>
	#include <netinet/in.h>
	
	#include <sys/ioctl.h>
	#include <arpa/inet.h>
#ifndef __CYGWIN_C__
	#include <net/if.h>
	#include <net/if_arp.h>
#endif
#elif defined(VMS)
	#include <types.h>
	#include <socket.h>
	#include <in.h>
	#include <netdb.h>
	#include <inet.h>
#elif defined(_WIN32)
	#include <winsock.h>
#endif

#ifdef __LINM_SFTP_USE__
	#include "openssl/des.h"
#endif

#include "exception.h"
#include "mlslog.h"
#include "passencrypt.h"


using namespace MLSUTIL;
using namespace MLS;

PasswdCrypt::PasswdCrypt(const string& strKey)
{
	_sKeyData = strKey;
}

char	PasswdCrypt::HexValue(char      c)
{
	switch(c)
	{
		case 'a': case 'A': return  10;
		case 'b': case 'B':	return  11;
		case 'c': case 'C': return  12;
		case 'd': case 'D': return  13;
		case 'e': case 'E': return  14;
		case 'f': case 'F': return  15;
		default:
			if('0'<=c && c<='9')
			return  c-'0';
	}
	return  0;
}

#define HEX2BYTE(b,h)   			\
{									\
	(b)=HexValue(*(h));				\
	(b)=(b)<<4;						\
	(b)=(b)|HexValue(*((h)+1));		\
}

void	PasswdCrypt::DesECBEncode(const string& sKeyHex, const string& sDecode, string& sEncode)
{
	#ifdef __LINM_SFTP_USE__
	des_key_schedule	tKeySchedule;

	des_cblock			tHexBlock, tSrcBlock, tDecBlock;
	char   				cHexa[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	unsigned char   	cMask[2]        = {0xF0, 0x0F};
	char				cEncode[17];
	char				cKeyHex[16];
	char				cDecode[16];
	int					i;

	sEncode = "";
	memcpy(&cKeyHex, sKeyHex.c_str(), 16);
	memcpy(&cDecode, sDecode.c_str(), 16);

	if (sDecode.size() > 8)
		throw Exception("Decode string size > 8 !!!");

	if (sKeyHex.size() != 16)
		throw Exception("KeyHex string size != 16 !!!");
	
	for(i=0; i<8; i++)
	{
		HEX2BYTE(*(tHexBlock+i), cKeyHex+i*2);
		if ((int)sDecode.size() > i) 
			tSrcBlock[i] = (unsigned char)sDecode[i];
		else
			tSrcBlock[i] = (unsigned char)' ';
	}

	des_set_key((const_des_cblock*)&tHexBlock, tKeySchedule);
	des_ecb_encrypt((const_des_cblock*)&tSrcBlock, &tDecBlock, tKeySchedule, DES_ENCRYPT);

	for(i=0; i<8; i++)
	{
		cEncode[i*2]    = cHexa[(tDecBlock[i]&cMask[0])>>4];
		cEncode[i*2+1]  = cHexa[tDecBlock[i]&cMask[1]];
	}
	cEncode[16] = 0;
	sEncode = cEncode;
	#endif
}
	

void	PasswdCrypt::DesECBDecode(const string& sKeyHex, const string& sEncode, string& sDecode)
{
	#ifdef __LINM_SFTP_USE__
	des_key_schedule	tKeySchedule;

	des_cblock			tKeyBlock, tEncBlock, tDecBlock;
	char				cKeyHex[16];
	char				cEncode[16];
	int					i;

	sDecode = "";
	memcpy(&cKeyHex, sKeyHex.c_str(), 16);
	memcpy(&cEncode, sEncode.c_str(), 16);

	if (sEncode.size() != 16)
		throw Exception("Decode string size != 8 !!!");

	if (sKeyHex.size() != 16)
		throw Exception("KeyHex string size != 16 !!!");

	for(i=0; i<8; i++)
	{
		HEX2BYTE(*(tKeyBlock+i), cKeyHex+i*2);
		HEX2BYTE(*(tEncBlock+i), cEncode+i*2);
	}

	des_set_key((const_des_cblock*)&tKeyBlock, tKeySchedule);
	des_ecb_encrypt((const_des_cblock*)&tEncBlock, &tDecBlock, tKeySchedule, DES_DECRYPT);

	sDecode = "";
	for(i=0; i<8; i++)
	{
		if (tDecBlock[i] == 0x00) break;
		sDecode.append(1, (char)tDecBlock[i]);
	}
	#endif
}

/// \brief  자신의 시스템의 Network IP 를 알게 해준다.
/// \param  sIP             리턴할 IP
/// \param  sMacAddress		MacAddress
/// \return true, false
bool	PasswdCrypt::ClientMacAddress(const string& sDevice, string& sIP, string& sMacAddress)
{
	bool bRt = true;
	#if defined(__linux__) || defined(__KERNEL__)
	int fd;
	struct sockaddr_in      *sinAddr = NULL;
	struct ifreq            ifr;
	unsigned char*			ptr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0) 
		throw Exception("socket func error [%d][%s]", errno, strerror(errno));

	memset(&ifr, 0, sizeof(ifr));

	/* XXX Work around Linux kernel bug */
	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, sDevice.c_str(), sizeof(ifr.ifr_name));
	if(ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
		bRt = false;
	} else {
		sinAddr = (struct sockaddr_in *)&ifr.ifr_addr;
	}

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
  		bRt = false;
	}
    else
	{
		ptr = (unsigned char*)&ifr.ifr_hwaddr.sa_data[0];
		
		String	sMacAddr;
		sMacAddr.Printf("%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X", 
					*ptr, *(ptr+1),*(ptr+2), *(ptr+3),*(ptr+4),*(ptr+5));
		sMacAddress = sMacAddr.c_str();
		close(fd);
	}
	
	if ( sinAddr )
		sIP = (const char*)inet_ntoa(sinAddr->sin_addr);
		
	close(fd);
	#else
	// cygwin에서는 따로 알아낼 방법을 찾아야 함.
	sMacAddress = "0ABE12EF1239";
	#endif
	return bRt;
}

bool	PasswdCrypt::Encrypt(const string& sDecode, string& sRtEncode)
{
	#ifdef __LINM_SFTP_USE__
	string 		sKeyHex, sTmpDecode1 = sDecode, sTmpDecode2, sEncode;
	
	try
	{
		string	sIP;
		if (ClientMacAddress("eth0", sIP, sKeyHex) == false)
			sKeyHex = "8ABE12AC4269";
		
		sKeyHex.append(sKeyHex.substr(4,2));
		sKeyHex.append(sKeyHex.substr(0,2));

		while(1)
		{
			if (sTmpDecode1.size() > 8)
			{
				sTmpDecode2 = sTmpDecode1.substr(0, 8);
				sTmpDecode1 = sTmpDecode1.substr(8);
			}
			else
			{
				sTmpDecode2 = sTmpDecode1;
				sTmpDecode1 = "";
			}

			if (sTmpDecode2.size() != 8)
				sTmpDecode2.append(8-sTmpDecode2.size(), ' ');

			if (_sKeyData != "")
				DesECBEncode(_sKeyData, sTmpDecode2, sEncode);
			else
				DesECBEncode(sKeyHex, sTmpDecode2, sEncode);

			sRtEncode = sRtEncode + sEncode;
			if (sTmpDecode1.size() == 0) break;
		}
	}
	catch(Exception& ex)
	{
		LOG_WRITE("Encrypt Error [%s]", (char*)ex);
		return false;
	}
	return true;
	#else
	return false;
	#endif
}

bool PasswdCrypt::Decrypt(const string& sEncode, string& sRtDecode)
{
	#ifdef __LINM_SFTP_USE__
	string 		sKeyHex, sDecode, sTmpEncode1 = sEncode, sTmpEncode2;
	
	try
	{
		string	sIP;
		if (ClientMacAddress("eth0", sIP, sKeyHex) == false)
			sKeyHex = "8ABE12AC4269";
		sKeyHex.append(sKeyHex.substr(4,2));
		sKeyHex.append(sKeyHex.substr(0,2));

		while(1)
		{
			if (sTmpEncode1.size() > 16)
			{
				sTmpEncode2 = sTmpEncode1.substr(0, 16);
				sTmpEncode1 = sTmpEncode1.substr(16);
			}
			else
			{
				sTmpEncode2 = sTmpEncode1;
				sTmpEncode1 = "";
			}

			if (sTmpEncode2.size() != 16)
				sTmpEncode2.append(16-sTmpEncode2.size(), '0');

			if (_sKeyData != "")
				DesECBDecode(_sKeyData, sTmpEncode2, sDecode);
			else
				DesECBDecode(sKeyHex, sTmpEncode2, sDecode);

			sRtDecode = sRtDecode + sDecode;
			if (sTmpEncode1.size() == 0) break;
		}
	}
	catch(Exception& ex)
	{
		LOG_WRITE("Decrypt Error [%s]", (char*)ex);
		return false;
	}

	sRtDecode = chop(sRtDecode);
	return true;
	#else
	return false;
	#endif
}
