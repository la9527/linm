#include "define.h"
#include "strutil.h"
#include "mlslog.h"
#include "exception.h"
#include "passencrypt.h"

using namespace MLS;
using namespace MLSUTIL;

int main(int argc, char *argv[]) {
    string sDir = ".";
    if (argc == 3) {
        g_Log.SetFile(argv[2]);
        sDir = argv[1];
    }

    if (argc == 2) {
        sDir = argv[1];
    }

    SFtpReader tSFtpReader;
    tSFtpReader.Init("la9527@localhost");

    if (tSFtpReader.Read(sDir) == false) return 0;
    bool bBreak = false;

    while (tSFtpReader.Next()) {
        MLS::File tFile;
        tSFtpReader.GetInfo(tFile);

        printf("%10s %6s %6s %10d %s\n", tFile.sAttr.c_str(),
               tFile.sDate.c_str(),
               tFile.sTime.c_str(),
               tFile.uSize,
               tFile.sName.c_str());
    }

    return 0;
}


