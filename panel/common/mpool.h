///	\file	mpool.h
///	\brief	mpool.h
#ifndef mpool_h_
#define mpool_h_

#include "define.h"
#include "file.h"

namespace MLS
{

///	\brief	vector와 비슷한 기능을 하는 class
template<typename T, int CHUNK = 500>
class MemoryPool
{
protected:
	std::vector<T*> m_data;
	
	typedef typename std::vector<T*>::iterator Itor;	///< iterator 재정의
	unsigned int m_nData;								///< 데이터 개수
	
	///	\brief	모든 데이터 삭제함수
	void Destroy() 
	{
		for (Itor i = m_data.begin(); i != m_data.end(); ++i)
		{
			delete[] *i;			
		}
		m_data.clear();
	}
	
public:
	MemoryPool()  { m_nData = 0; }	///< 생성자
	~MemoryPool() { Destroy();}		///< 소멸자
	
	///	\brief	빈 데이터 공간을 만들어 낸다.
	///	\return	빈데이터 공간이나\n 공간이 부족한경우 맨마지막 데이터
	T& Get()
	{
		m_nData++;
		
		if (m_data.size() * CHUNK < m_nData)
		{// 새 공간을 할당한다.
			m_data.push_back(new T[CHUNK]);
		}
		
		return m_data[(m_nData-1)/CHUNK][(m_nData-1)%CHUNK];
	}
	
	void Clear() { m_nData = 0; Destroy(); }		///< 데이터 개수를 0으로 만듬

};

static MemoryPool<File> &GetMemPool()
{
	static MemoryPool<File> _pool;
	return _pool;
}

#define	 g_tMemPool		GetMemPool()

};

#endif
