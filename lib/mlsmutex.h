/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 ***************************************************************************/

#ifndef	__MlsMutex_h__
#define	__MlsMutex_h__

#include <errno.h>

#ifdef PTHREAD_ENABLE
	#include <pthread.h>
#endif

#include "define.h"
#include "exception.h"

namespace MLSUTIL
{
	class	MlsMutex
	{
		public:
			MlsMutex();
			virtual	~MlsMutex();
			
			int		Lock(void);
			int		TryLock(void);
			int		Unlock(void);
	
			void	Set(void* pData);
			void*	Wait(void);
			void*	TimedWait(int nMicroseconds, int nSeconds);
			void	WaitEnd(void);

			#ifdef PTHREAD_ENABLE
			operator	pthread_mutex_t*()	{ return	&m_tMutex; }
			#endif
			
		protected:
			#ifdef PTHREAD_ENABLE
			pthread_cond_t		m_tAvail;	///<	Condition
			pthread_cond_t		m_tReady;	///<	Condition
			pthread_mutex_t     m_tMutex;	///<	Mutex
			int					m_nReady;	///<	int
			void*				m_pData;	///<	void*
			#endif
	};

	class MlsMutexSelf
	{
		public:
			MlsMutexSelf( MlsMutex* pMutex )
			{
				_pMutex = pMutex;
				if ( _pMutex ) _pMutex->Lock();
			}

			~MlsMutexSelf()
			{
				if ( _pMutex ) _pMutex->Unlock();
			}

		private:
			MlsMutex*	_pMutex;
	};
};

#endif
