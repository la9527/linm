/***************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 ***************************************************************************/
 
/// @file	mlsthread.h
/// @brief	MlsThread class header file.
#ifndef	__MlsThread_h__
#define	__MlsThread_h__

#ifdef PTHREAD_ENABLE
#include <pthread.h>
#endif

#include "define.h"
#include "exception.h"

namespace	MLSUTIL
{
	#define		JOIN				true
	#define		DETACH				false
	
	enum PT_CUR
	{
		PTHREAD_START,
		PTHREAD_END,
		PTHREAD_RUNNING,
		PTHREAD_EXIT
	};

	/// @brief	Thread class for linm.
	class	MlsThread
	{
		public:
			MlsThread(bool bMlsThreadOn = true);
			virtual	~MlsThread();

			virtual	int	Create(void);
			virtual	int	Destroy(void);
			
			void		ViewThreadVersion(void);

			virtual int			Start(void*	pArg, bool bType = DETACH);
			virtual void*	 	End(void);

			virtual void		Cancel(void);
			virtual void		CancelPoint(void);

			virtual void		Thread_uSleep(int s_uSec);

			/// @brief	Is this running?
			PT_CUR		IsRun(void)			{return	m_tRun;}
			/// @brief	Set the running thread type. (PTHREAD_START, END, RUNNING, EXIT)
			void		SetRun(PT_CUR bRun)	{m_tRun = bRun;}
			#ifdef PTHREAD_ENABLE
			/// @brief	return to pthread_t
			operator	pthread_t		()	{return	m_tHandle;}
			/// @brief	return to pthread_attr_t 
			operator	pthread_attr_t*	()	{return	&m_tAttr;}
			#endif
			/// @brief	return to pArg value of Start()
			operator	void*			()	{return	m_pArg;}

		protected:
			virtual	void*	Execute(void*);

		private:
			void*			Run(void*	pArg);
			static	void*	EntryPoint(void*);
			#ifdef PTHREAD_ENABLE
			pthread_t		m_tHandle;
			pthread_attr_t	m_tAttr;
			#endif
			void*			m_pArg;
			PT_CUR			m_tRun;
			bool			m_bType;
			bool			m_bMlsThreadOn;
	};
}

#endif
