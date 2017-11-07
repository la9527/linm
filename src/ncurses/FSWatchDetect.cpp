//
// Created by la9527 on 2017. 10. 30..
//

#include <ncurses.h>
#include "FSWatchDetect.h"
#include "../../libfswatch/c++/libfswatch_exception.hpp"

using namespace fsw;
using namespace MLS;
using namespace MLSUTIL;

FSWatchDetect::FSWatchDetect()
    : _pThread(nullptr ), _pActiveMonitor(nullptr) {

}

FSWatchDetect::~FSWatchDetect() {
    this->stop();
}

bool FSWatchDetect::stop() {
    try {
        if (_pThread && _pActiveMonitor && _pActiveMonitor->is_running()) {
            {
                std::lock_guard<std::mutex> guard(_mutex);
                _pActiveMonitor->stop();
                if (_pActiveMonitor) {
                    delete _pActiveMonitor;
                    _pActiveMonitor = nullptr;
                }
            }
            _pThread->join();
            if ( _pThread ) {
                delete _pThread;
                _pThread = nullptr;
            }
        }
    } catch( ... ) {
        LOG( "FSWatchDetect::stop Exception !!!" );
    }
    return true;
}

bool FSWatchDetect::start(const vector<string> &vPaths, std::function<void (const std::vector<string>)> func) {
    if (_pActiveMonitor && _pActiveMonitor->is_running()) {
        if ( this->_vPaths == vPaths ) {
            LOG( "FSWatchDetect SAME PATHS" );
            return false;
        }
    }

    stop();

    try {
        for( const string& path : vPaths ) {
            LOG( "WATCH RUN PATHS [%s]", path.c_str() );
        }

        _pActiveMonitor = monitor_factory::create_monitor(
                fsw_monitor_type::system_default_monitor_type,
                vPaths,
                FSWatchDetect::detectFunc
        );
    } catch ( const fsw::libfsw_exception& e ) {
        LOG("FSWatchDetect Exception : %s", e.what() );
        return false;
    }
    this->_vPaths = vPaths;
    this->_func = func;

    this->_pThread = new std::thread( [&] () {
        // pActiveMonitor->set_directory_only(true);
        _pActiveMonitor->start();
    });
    return true;
}

void FSWatchDetect::detectFunc(const std::vector<event>& events, void* context) {
    vector<string> vEvents;
    for( event evt : events ) {
        vEvents.push_back( evt.get_path() );
        LOG( "watch [%s]", evt.get_path().c_str() );
    }

    std::lock_guard<std::mutex> guard(g_tFSWatchDetect._mutex);
    g_tFSWatchDetect._vDetectPaths = vEvents;
    g_tFSWatchDetect._func( vEvents );
}
