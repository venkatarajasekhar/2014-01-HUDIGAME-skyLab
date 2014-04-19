#pragma once

#define WM_SOCKET 104
#define ALIGNMENT_SIZE 16

template <typename T>
inline void SafeDelete( T* &p )
{
	if ( p != nullptr )
	{
		delete p;
		p = nullptr;
	}
}

template <typename T>
inline void SafeArrayDelete( T* &p )
{
	if ( p != nullptr )
	{
		delete[] p;
		p = nullptr;
	}
}

#define CREATE_FUNC(CLASS_NAME) \
	static CLASS_NAME* Create() \
{ \
	CLASS_NAME* pInstance = new CLASS_NAME(); \
	return pInstance; \
}

// string입력 받는 버전
#define CREATE_FUNC_S(CLASS_NAME) \
	static CLASS_NAME* Create(std::wstring str) \
{ \
	CLASS_NAME* pInstance = new CLASS_NAME(str); \
	return pInstance; \
}

template <class T>
class Singleton
{
protected:
	Singleton( void ) {}
	virtual ~Singleton( void ) {}
public:
	static T* GetInstance( void )
	{
		if ( !m_pInstance )
		{
			m_pInstance = (T*)_aligned_malloc( sizeof( T ), ALIGNMENT_SIZE );
			new (m_pInstance)T();
		}
		return m_pInstance;
	}

	static void ReleaseInstance( void )
	{
		m_pInstance->~T();
		_aligned_free( m_pInstance );
	}

private:
	static T*	m_pInstance;
};
template <typename T>
T* Singleton<T>::m_pInstance = nullptr;