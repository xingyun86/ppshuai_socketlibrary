#pragma once

namespace SL
{

template <typename T>
class CSLSingleton
{
public:
	static T* Instance()
	{
		return &m_Instance;
	}
private:
	CSLSingleton();
private:
	static T m_Instance;
};

template <typename T>
T SLSingleton<T>::m_pInstance;

};