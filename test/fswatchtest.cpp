//
// Created by la9527 on 2017. 10. 27..
//

#include <functional>
#include "define.h"
#include "strutil.h"
#include "mlslog.h"
#include "exception.h"
#include "dirreader.h"

#include "../libfswatch/c++/event.hpp"
#include "../libfswatch/c++/monitor.hpp"
#include "../libfswatch/c/error.h"
#include "../libfswatch/c/libfswatch.h"
#include "../libfswatch/c/libfswatch_log.h"
#include "../libfswatch/c++/libfswatch_exception.hpp"

using namespace MLS;
using namespace MLSUTIL;
using namespace fsw;

/*
void func(const std::vector<event>& events, void* context) {
    printf("CHANGE EVENT - SIZE : %ld\n", events.size());
    for(auto const& evt: events) {
        printf("CHANGE EVENT - PATH : %s\n", evt.get_path().c_str());
    }
}
*/

bool fsWatchMonitor(const string& path) {
    try {
        vector<string> vPaths;
        vPaths.push_back(path);

        cout << "path :: " << path.c_str() << endl;
        cout << "monitor_factory::get_types() :: " << monitor_factory::get_types().size() << endl;
        for ( const auto& types : monitor_factory::get_types() ) {
            printf("get_types: %s\n", types.c_str());
        }

        auto callBack = [](const std::vector<event>& events, void* context) {
            printf("FSW_EVENT_CALLBACK\n");
            for(auto const& evt: events) {
                printf("CHANGE EVENT - PATH : %s\n", evt.get_path().c_str());
            }
        };

        FSW_EVENT_CALLBACK* funcResult = reinterpret_cast<FSW_EVENT_CALLBACK*>(&callBack);

        {
            std::vector<event> events;
            (*funcResult)(events, nullptr);
            printf("TEST !!!\n");
        }

        // FSW_EVENT_CALLBACK* callbackFunc = callBack.target<FSW_EVENT_CALLBACK>();

        monitor *pActiveMonitor = monitor_factory::create_monitor(
                fsw_monitor_type::system_default_monitor_type,
                vPaths,
                &funcResult
            );

        // pActiveMonitor->set_directory_only(true);
        pActiveMonitor->start();
    } catch( const libfsw_exception& e ) {
        printf( " Exception e !!! [%s]", e.what() );
        LOG("Exception Error !!!");
    }
    return true;
}


int main(int argc, char *argv[]) {
    string sDir = ".";
    if (argc == 3) {
        g_Log.SetFile(argv[2]);
        sDir = argv[1];
    }

    if (argc == 2) {
        sDir = argv[1];
    }

    fsWatchMonitor( sDir );

    /*
    DirReader dirReader;
    dirReader.Init(".");

    if (dirReader.Read(sDir) == false) return 0;
    bool bBreak = false;

    while (dirReader.Next()) {
        MLS::File tFile;
        dirReader.GetInfo(tFile);

        printf("%10s %6s %6s %10llu %s\n", tFile.sAttr.c_str(),
               tFile.sDate.c_str(),
               tFile.sTime.c_str(),
               tFile.uSize,
               tFile.sName.c_str());
    }
     */

    return 0;
}


