/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 ***************************************************************************/

/// @file	mlsmutex.cpp
/// @brief	mlsmutex.cpp
#include "mlsmutex.h"

using namespace MLSUTIL;

/// @brief	MutexComm 생성자
MlsMutex::MlsMutex() {
#ifdef PTHREAD_ENABLE
    int nResult;
//	m_tMutex	= PTHREAD_MUTEX_INITIALIZER;
    m_nReady = 0;
    m_pData = NULL;
    nResult = pthread_mutex_init(&m_tMutex, NULL);
    if (nResult) throw Exception("MlsMutexComm::pthread_mutex_init");
    nResult = pthread_cond_init(&m_tReady, NULL);
    if (nResult) throw Exception("MlsMutexComm::CondInit 1");
    nResult = pthread_cond_init(&m_tAvail, NULL);
    if (nResult) throw Exception("MlsMutexComm::CondInit 2");
#endif
}

/// @brief	MutexComm 소멸자
MlsMutex::~MlsMutex() {
#ifdef PTHREAD_ENABLE
    int nResult;
    nResult = pthread_cond_destroy(&m_tReady);
    if (nResult) throw Exception("MlsMutexComm::CondDestroy");
    nResult = pthread_cond_destroy(&m_tAvail);
    if (nResult) throw Exception("MlsMutexComm::CondDestroy");
    nResult = pthread_mutex_destroy(&m_tMutex);
    if (nResult) throw Exception("MlsMutexComm::MutexDestroy");
#endif
}

/// @brief	Mutex Lock 건다. 두개의 쓰레드 중 다른쓰레드가 unlock이 될때 까지 대기한다.
/// @return SUCCESS
int MlsMutex::Lock(void) {
#ifdef PTHREAD_ENABLE
    int nResult;
    nResult = pthread_mutex_lock(&m_tMutex);
    if (nResult)
        throw Exception("MutexComm::Lock");
#endif
    return SUCCESS;
}

/// @brief	Mutex TryLock을 건다. 두개의 쓰레드 중 다른쓰레드가 lock이 되어 있으면 바로 ERROR를 리턴한다.
/// @return SUCCESS, ERROR, throw
int MlsMutex::TryLock(void) {
#ifdef PTHREAD_ENABLE
    int nResult;
    nResult = pthread_mutex_trylock(&m_tMutex);
    if (nResult == EBUSY)
        return ERROR;
    if (nResult)
        throw Exception("MutexComm::TryLock");
#endif
    return SUCCESS;
}

/// @brief	Mutex Unlock을 한다.
/// @return SUCCESS
int MlsMutex::Unlock(void) {
#ifdef PTHREAD_ENABLE
    int nResult;
    nResult = pthread_mutex_unlock(&m_tMutex);
    if (nResult)
        throw Exception("MutexComm::Unlock");
#endif
    return SUCCESS;
}

/// @brief	다른 쓰레드로 데이터를 보낼때 사용(signal)
void MlsMutex::Set(void *pData) {
#ifdef PTHREAD_ENABLE
    int nResult;
    Lock();
    while (m_nReady) {
        nResult = pthread_cond_wait(&m_tReady, &m_tMutex);
        if (nResult) throw Exception("Set::pthread_cond_wait");
    }
    m_pData = pData;
    m_nReady = 1;

    nResult = pthread_cond_signal(&m_tAvail);
    if (nResult) throw Exception("Set::pthread_cond_signal");
    Unlock();
#endif
}

/// @brief	다른 쓰레드로부터 데이터 올때까지 기다리는데 사용 (wait)
/// @return MlsMutexComm 포인터
void *
MlsMutex::
Wait(void) {
    void *pResult = NULL;
#ifdef PTHREAD_ENABLE
    int nResult;
    while (m_nReady != 1) {
        nResult = pthread_cond_wait(&m_tAvail, &m_tMutex);
        if (nResult) throw Exception("Wait::pthread_cond_wait");
    }
    pResult = m_pData;
#endif
    return pResult;
}

/// @brief	다른 쓰레드로부터 데이터 올때까지 기다리는데 사용 시간 제한을 둔다. (timewait)
/// @return MutexComm 포인터
void * MlsMutex::TimedWait(int nMicroseconds, int nSeconds) {
    int nResult;
    void *pResult = NULL;
#ifdef PTHREAD_ENABLE
    timespec tTimeSpec;
    while (m_nReady != 1) {
        tTimeSpec.tv_sec = nSeconds;
        tTimeSpec.tv_nsec = nMicroseconds;
        nResult = pthread_cond_timedwait(&m_tAvail, &m_tMutex, &tTimeSpec);

        switch (nResult) {
            case ETIMEDOUT:
                return pResult = (void *) -1;
            case EINTR:
            default:
                throw Exception("TimedWait::pthread_cond_timedwait");
        }
    }
    pResult = m_pData;
#endif
    return pResult;
}

/// @brief	Wait가 끝났음을 알릴 때 사용 (waitend)
void MlsMutex::WaitEnd(void) {
#ifdef PTHREAD_ENABLE
    m_pData = NULL;
    m_nReady = 0;
    int nResult = pthread_cond_signal(&m_tReady);
    if (nResult) throw Exception("WaitEnd::pthread_cond_signal");
#endif
}

