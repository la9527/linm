/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 ***************************************************************************/

/// @file	mlsthread.cpp
/// @brief	mlsthread.cpp

#include "mlsthread.h"
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

namespace MLSUTIL {

/// @brief	mlsthread 생성자
    MlsThread::MlsThread(bool bThreadOn) {
        m_pArg = NULL;
        m_tRun = PTHREAD_START;
        m_bMlsThreadOn = bThreadOn;

        LOG_WRITE("Thread::Create");
#ifdef PTHREAD_ENABLE
        if (m_bMlsThreadOn) {
            int nRes = pthread_attr_init(&m_tAttr);
            if (nRes != SUCCESS) {
                throw Exception("pthread_attr_init failed");
            }
            pthread_attr_setstacksize(&m_tAttr, 128);
        }
#endif
    }

/// @brief	mlsthread 소멸자
    MlsThread::~MlsThread() {
        Destroy();
#ifdef PTHREAD_ENABLE
        if (m_bMlsThreadOn)
            pthread_attr_destroy(&m_tAttr);
        LOG_WRITE("Thread::Destory");
#endif
    }

/// @brief	Create로 virtual함수이다.
    int MlsThread::Create(void) {
        return SUCCESS;
    }

/// @brief	Destroy로 virtual함수이다.
    int MlsThread::Destroy(void) {
        return SUCCESS;
    }

/// @brief	Thread version info view
    void MlsThread::ViewThreadVersion(void) {
#ifdef PTHREAD_ENABLE
#ifdef _CS_GNU_LIBPTHREAD_VERSION
        char sVer[64];
        confstr(_CS_GNU_LIBPTHREAD_VERSION, sVer, sizeof(sVer));
        cout << sVer << endl;
#else
        cout << "Unknown" << endl;
#endif
#endif
    }

/// @brief	쓰레드 create로 EntryPoint로 연결된다. 쓰레드의 시작이다.
/// @param	pArg	만들어질 쓰레드에 전달할 void* 데이터
/// @param	bType	JOIN, DETACH 두가지 형태로 JOIN은 End()함수를 이용해
///                 종료여부를 확인 할수 있고, DETACH는 확인하지 않는다.
/// @return 성공 여부를 리턴한다.
    int MlsThread::Start(void *pArg, bool bType) {
        int nResult = 0;
#ifdef PTHREAD_ENABLE
        m_bType = bType;

        m_pArg = pArg;

        nResult = pthread_create(&m_tHandle,
                                 &m_tAttr,
                                 MlsThread::EntryPoint,
                                 this);
#endif
        return nResult;
    }

/// @brief	JOIN으로 Start할시 쓰레드가 종료할때 까지 대기하는 함수
/// @return	쓰레드 즉 Execute() 함수에서 리턴된 void* 를 돌려준다.
    void *MlsThread::End(void) {
        int status;
        void *pJoinData = NULL;
#ifdef PTHREAD_ENABLE
        if (m_bType == JOIN) {
            status = pthread_join(m_tHandle, &pJoinData);
            LOG_WRITE("Thread::End - JOIN");

            if (status != SUCCESS) {
                //throw Exception("pthread_join Error");
                return NULL;
            }
        } else {
            throw Exception("Start Detached Error");
        }
#endif
        return pJoinData;
    }

/// @brief    쓰레드를 취소한다.
    void MlsThread::Cancel(void) {
#ifdef PTHREAD_ENABLE
        int nRt = pthread_cancel(m_tHandle);

        if (nRt == ESRCH) {
            LOG_WRITE("Thread::Cancel - no thread could be found");
        }

        if (nRt)
            throw Exception("Thread::Cancel Error %d", nRt);
#endif
    }

/// @brief    cancel 위치를 정할때 사용한다.
    void MlsThread::CancelPoint(void) {
#ifdef PTHREAD_ENABLE
        pthread_testcancel();
#endif
    }

/// @brief	쓰레드에서 usleep을 이용하지 않고 select를 이용해서 Thread 를 sleep한다.
/// @param 	s_uSec	100만분의 1초단위 시간
    void MlsThread::Thread_uSleep(int s_uSec) {
        struct timeval s_TimeVal;
        if (s_uSec > 1000000) /* uSec의 큰 값은 초단위로 환산 */
        {
            s_TimeVal.tv_sec = s_uSec / 1000000;
            s_uSec %= 1000000;
        } else s_TimeVal.tv_sec = 0;

        s_TimeVal.tv_usec = s_uSec;
        select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &s_TimeVal);
    }

/// @brief	start에서 시작된 쓰레드는 여기에서 먼저 시작 된다. (static 함수)
/// @param	pThis	start에서 pArg가 여기에 들어온다.
    void *MlsThread::EntryPoint(void *pThis) {
        MlsThread *pThread = (MlsThread *) pThis;
        LOG_WRITE("Thread::Execute Start :: ID :: %d", pthread_self());

        // Run 을 실행. Run은 Excute 실행(virtual 함수)
        void *pReturnArg = NULL;
#ifdef PTHREAD_ENABLE
        pReturnArg = pThread->Run((void *) (*pThread));

        LOG_WRITE("Thread::Execute End :: ID :: %d", pthread_self());
        pthread_exit(pReturnArg);
#endif
        return NULL;
    }

/// @brief	start에서 EntryPoint를 거쳐서 Run으로 오게된다.
/// @param	pArg	start에서 pArg가 여기에 들어온다.
    void *MlsThread::Run(void *pArg) {
        int status = 0;
        void *pReturnArg = NULL;
#ifdef PTHREAD_ENABLE
        if (m_bType == DETACH) {
            status = pthread_detach(pthread_self());

            if (status != SUCCESS) {
                throw Exception("pthread_detach Error");
            }
            LOG_WRITE("Thread::Run - Detached ID :: %d", pthread_self());
        }

        m_tRun = PTHREAD_RUNNING;
        pReturnArg = Execute(pArg);
        m_tRun = PTHREAD_END;
#endif
        return pReturnArg;
    }

/// @brief	virtual 함수로 쓰레드를 이용할때 내용을 클래스에 넣어 쓰레드에 이용하면 된다.
/// @param	pArg	start에서 pArg가 여기에 들어온다.
    void *MlsThread::Execute(void *pArg) {
        LOG_WRITE("Execute function");
        return NULL;
    }

};

