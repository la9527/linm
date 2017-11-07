//
// Created by la9527 on 2017. 10. 30..
//

#ifndef LINM_FSWATCHDETECT_H
#define LINM_FSWATCHDETECT_H

#include "define.h"
#include <thread>
#include <functional>
#include "../../libfswatch/c++/monitor.hpp"

namespace MLS {

    class FSWatchDetect {
    public:
        FSWatchDetect();
        ~FSWatchDetect();

        bool start(const vector<string> &vPaths, std::function<void (const std::vector<string>)> func);
        bool stop();

        const vector<string> &getDetectPaths() { return _vDetectPaths; }

        static FSWatchDetect &GetInstance() {
            static FSWatchDetect tFsWatchDetect;
            return tFsWatchDetect;
        }
    protected:
        static void detectFunc(const std::vector<fsw::event> &events, void *context);
        void run(const vector<string> &vPaths);

    private:
        std::vector<string> _vPaths;
        std::vector<string> _vDetectPaths;
        std::function<void (const std::vector<string>)> _func;
        fsw::monitor *_pActiveMonitor;
        std::thread *_pThread;
        std::mutex _mutex;
    };

    #define g_tFSWatchDetect FSWatchDetect::GetInstance()
};

#endif //LINM_FSWATCHDETECT_H
