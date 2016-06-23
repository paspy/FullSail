#pragma once
#include "fsEventIDs.h"
#define EventFunc void (T::*)(const EventArgs&)

struct EventArgs
{
	virtual ~EventArgs() {};
};

template<typename T>
struct EventArgs1 : public EventArgs
{
public:
	T m_Data;
	EventArgs1(const T _data) : m_Data(_data) {}
private:
	EventArgs1() {};
};

template<typename T, typename T2>
struct EventArgs2 : public EventArgs
{
public:
	T m_Data1;
	T2 m_Data2;
	EventArgs2(const T _data1, const T2 _data2) : m_Data1(_data1), m_Data2(_data2) {}
private:
	EventArgs2() {};
};

struct EventCallback
{
	virtual void Callback(const EventArgs&) = 0;
};

template<typename T>
struct TemplatedEventCallback : public EventCallback
{
	void (T::*m_callback)(const EventArgs&);
	T* m_invokingObject;

	TemplatedEventCallback(T* const _obj, void (T::*_callback)(const EventArgs&)) : m_invokingObject(_obj), m_callback(_callback) {}

	void Callback(const EventArgs& _args) {
		if (m_invokingObject)
			(m_invokingObject->*m_callback)(_args);
	}
};


class fsEventManager
{
	std::map<EventID, std::vector<EventCallback*>> m_mpEventListeners;

	fsEventManager();
	fsEventManager(const fsEventManager&);
	fsEventManager& operator=(const fsEventManager&) { };
	~fsEventManager();

	static fsEventManager *m_pInstance;
public:

	//Call this to get functionality
	static fsEventManager* GetInstance();
	static void	DeleteInstance();

	void FireEvent(const EventID& EVENTID, const EventArgs* = nullptr);

	template<typename T, class Args>
	void RegisterClient(const EventID& EVENTID, T* client, void(T::*_callback)(const Args&));

	template<typename T, class Args>
	void UnRegisterClient(const EventID& EVENTID, T* client, void(T::*_callback)(const Args&));


};

//Make sure your callback function is holding a const & in the parameter.
//This not talking about what goes in the template but what the callback function parameter should be
//ex. NextSong(const EventArgs1<int*>& args)

template<typename T, class Args>
void fsEventManager::RegisterClient(const EventID& EVENTID, T* client, void(T::*_callback)(const Args&))
{
	//creating the call back 
	TemplatedEventCallback<T>* newCallback = new TemplatedEventCallback<T>(client, reinterpret_cast<EventFunc>(_callback));

	//adding it to the vector of CallBack
	m_mpEventListeners[EVENTID].push_back(newCallback);
}

template<typename T, class Args>
void fsEventManager::UnRegisterClient(const EventID& EVENTID, T* client, void(T::*_callback)(const Args&))
{
	auto iter = m_mpEventListeners[EVENTID].begin();
	auto end = m_mpEventListeners[EVENTID].end();

	for (; iter != end; iter++)
	{
		TemplatedEventCallback<T>* newCallback = reinterpret_cast<TemplatedEventCallback<T>*>(*iter);
		if (newCallback->m_invokingObject == client && newCallback->m_callback == reinterpret_cast<EventFunc>(_callback))
		{

			delete (*iter);
			(*iter) = nullptr;
			m_mpEventListeners[EVENTID].erase(iter);

			if (!m_mpEventListeners[EVENTID].size())
			{
				m_mpEventListeners.erase(EVENTID);
			}

			break;
		}
	}

	if (!m_mpEventListeners.size())
	{
		//DeleteInstance();
	}
}
