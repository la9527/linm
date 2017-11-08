#include "define.h"
#include "strutil.h"
#include "mlslog.h"
#include "exception.h"
#include <panel/common/file.h>
#include <panel/vfs/libssh2/SFtpReader.h>
#include <lib/mlsdialog.h>
#include "passencrypt.h"

using namespace MLS;
using namespace MLSUTIL;

int main(int argc, char *argv[]) 
{
	string sDir = ".";
	if (argc == 3)
	{
		g_Log.SetFile( argv[2] );
		sDir = argv[1];
	}

	if (argc == 2)
	{
		sDir = argv[1];
	}

	SetDialogProgress( new MlsDialog(), new MlsProgress() );
	
	SFtpReader	tSFtpReader;
	if ( tSFtpReader.Init("la9527@localhost") == false ) {
        printf( "tSFtpReader.Init() - fail." );
    }

	if (tSFtpReader.Read(sDir) == false) return 0;
	bool bBreak = false;
	
	while(tSFtpReader.Next())
	{
		MLS::File	tFile;
		tSFtpReader.GetInfo(tFile);

		printf("%10s %6s %6s %10lld %s\n", tFile.sAttr.c_str(),
						tFile.sDate.c_str(), 
						tFile.sTime.c_str(), 
						tFile.uSize, 
						tFile.sName.c_str());
	}
	
	return 0;
}


