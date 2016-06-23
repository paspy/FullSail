#include "pch.h"
#include "fsEventManager.h"

/*static*/ fsEventManager* fsEventManager::m_pInstance = nullptr;

fsEventManager::fsEventManager()
{
}

fsEventManager::~fsEventManager()
{
	auto eventNameIter = m_mpEventListeners.begin();
	auto eventNameEnd = m_mpEventListeners.end();

	while (eventNameIter != eventNameEnd)
	{
		for (auto&& listener : eventNameIter->second)
			delete listener;

		eventNameIter->second.clear();
		eventNameIter++;
	}
}

fsEventManager* fsEventManager::GetInstance()
{
	//checking to see if it called
	if (m_pInstance == nullptr)
	{
		// if so create the eventmanger
		m_pInstance = new fsEventManager();
	}

	return m_pInstance;
}

void fsEventManager::DeleteInstance()
{

	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
	}
	m_pInstance = nullptr;
}

void fsEventManager::FireEvent(const EventID & EVENTID, const EventArgs * newEventArg)
{
	auto iter = m_mpEventListeners[EVENTID].begin();
	auto end = m_mpEventListeners[EVENTID].end();

	for (; iter != end; iter++)
	{
		(*iter)->Callback(*newEventArg);
	}

}