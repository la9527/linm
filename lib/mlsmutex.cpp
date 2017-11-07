/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@daum.net                                                    *
 ***************************************************************************/
 
/// @file	mlsmutex.cpp
/// @brief	mlsmutex.cpp
#include "mlsmutex.h"

using namespace MLSUTIL;

/// @brief	MutexComm Constructor
MlsMutex::
MlsMutex()
{
	#ifdef PTHREAD_ENABLE
	int			nResult;
//	m_tMutex	= PTHREAD_MUTEX_INITIALIZER;
	m_nReady	= 0;
	m_pData		= NULL;
	nResult	= pthread_mutex_init(	&m_tMutex, NULL);
	if(nResult)	throw	Exception("MlsMutexComm::pthread_mutex_init");
	nResult	= pthread_cond_init(	&m_tReady,	NULL);
	if(nResult)	throw	Exception("MlsMutexComm::CondInit 1");
	nResult	= pthread_cond_init(	&m_tAvail,	NULL);
	if(nResult)	throw	Exception("MlsMutexComm::CondInit 2");
	#endif
}

/// @brief	MutexComm Destoroy
MlsMutex::
~MlsMutex()
{
	#ifdef PTHREAD_ENABLE
    int	nResult;
	nResult	= pthread_cond_destroy(	&m_tReady);
	if(nResult)	throw	Exception("MlsMutexComm::CondDestroy");
	nResult	= pthread_cond_destroy(	&m_tAvail);
	if(nResult)	throw	Exception("MlsMutexComm::CondDestroy");
	nResult	= pthread_mutex_destroy(&m_tMutex);
	if(nResult)	throw	Exception("MlsMutexComm::MutexDestroy");
	#endif
}

/// @brief	Use the mutex lock. Waiting until a unlock call from another thread.
/// @return SUCCESS
int
MlsMutex::
Lock(void)
{
	#ifdef PTHREAD_ENABLE
	int	nResult;
	nResult	= pthread_mutex_lock(	&m_tMutex);
	if(nResult)
		throw	Exception("MutexComm::Lock");
	#endif
	return	SUCCESS;
}

/// @brief	Use the mutex try lock. if thread is lock, return an error.( Did not waiting)
/// @return SUCCESS, ERROR, throw
int
MlsMutex::
TryLock(void)
{
	#ifdef PTHREAD_ENABLE
	int	nResult;
	nResult	= pthread_mutex_trylock(&m_tMutex);
	if(nResult == EBUSY)
		return	ERROR;
	if(nResult)
		throw	Exception("MutexComm::TryLock");
	#endif
	return	SUCCESS;
}

/// @brief	Mutex Unlock.
/// @return SUCCESS
int
MlsMutex::
Unlock(void)
{
	#ifdef PTHREAD_ENABLE
	int	nResult;
	nResult	= pthread_mutex_unlock(	&m_tMutex);
	if(nResult)
		throw	Exception("MutexComm::Unlock");
	#endif
	return	SUCCESS;
}

/// @brief	send to signal in another thread.. 
void
MlsMutex::
Set(void*	pData)
{
	#ifdef PTHREAD_ENABLE
	int nResult;
	Lock();
	while(m_nReady)
	{
		nResult = pthread_cond_wait(&m_tReady,	&m_tMutex);
		if(nResult) throw	Exception("Set::pthread_cond_wait");
	}
	m_pData		= pData;
	m_nReady	= 1;
	
	nResult	= pthread_cond_signal(	&m_tAvail);
	if(nResult) throw	Exception("Set::pthread_cond_signal");
	Unlock();
	#endif
}

/// @brief	from another thread, waiting until the data received. (wait)
/// @return MlsMutexComm pointer
void*
MlsMutex::
Wait(void)
{
	void*	pResult = NULL;
	#ifdef PTHREAD_ENABLE
	int		nResult;
	while(m_nReady != 1)
	{
		nResult = pthread_cond_wait(&m_tAvail,	&m_tMutex);
		if(nResult) throw	Exception("Wait::pthread_cond_wait");
	}
	pResult		= m_pData;
	#endif
	return	pResult;
}

/// @brief	Wait to some seconds until a message is received from another thread (timewait)
/// @return return a MutexComm pointer 
void*
MlsMutex::
TimedWait(int nMicroseconds, int nSeconds)
{
	int			nResult;
	void*		pResult = NULL;
	#ifdef PTHREAD_ENABLE
	timespec	tTimeSpec;
	while(m_nReady	!= 1)
	{
		tTimeSpec.tv_sec	= nSeconds;
		tTimeSpec.tv_nsec	= nMicroseconds;
		nResult	= pthread_cond_timedwait(&m_tAvail, &m_tMutex, &tTimeSpec);
		
		switch(nResult)
		{
			case ETIMEDOUT:
				return	pResult = (void*)-1;
			case EINTR:
			default:
				throw Exception("TimedWait::pthread_cond_timedwait");
		}
	}
	pResult	= m_pData;
	#endif
	return	pResult;
}

/// @brief	Set end point of wait (waitend)
void
MlsMutex::
WaitEnd(void)
{
	#ifdef PTHREAD_ENABLE
	m_pData		= NULL;
	m_nReady	= 0;
	int nResult	= pthread_cond_signal(	&m_tReady);
	if(nResult) throw	Exception("WaitEnd::pthread_cond_signal");
	#endif
}

