#pragma once

// Created by Justin Murphy
// Reference : Quynh Nguyen http://www.codeproject.com/KB/cpp/CppDelegateImplementation.aspx
#include <vector>
#include <map>
#include "./Event.h"

//--------------------------------------------------------------------------------
class CBaseEventArgs {
public:
	virtual ~CBaseEventArgs();
};
//--------------------------------------------------------------------------------
template<typename T>
class CGeneralEventArgs : public CBaseEventArgs {
	T m_Data;
	CGeneralEventArgs();
public:
	CGeneralEventArgs(const T data);
	const T& GetData() const;
};
//--------------------------------------------------------------------------------
template<typename T, typename T2>
class CGeneralEventArgs2 : public CBaseEventArgs {
	T m_Data;
	T2 m_Data2;
	CGeneralEventArgs2();
public:
	CGeneralEventArgs2(const T data, const T2 data2);
	const T& GetData() const;
	const T2& GetData2() const;
};
//--------------------------------------------------------------------------------
class CEvent {
	friend class CEventManager;
	const CBaseEventArgs* m_pArgs;
	EVENTID m_ID;
	int m_nPriority;
public:
	CEvent(const EVENTID& ID, const CBaseEventArgs* pArgs = 0, const int nPriority = 0);
	const CBaseEventArgs& GetArg() const;
	EVENTID GetID() const;
	int GetPriority() const;
};
//--------------------------------------------------------------------------------
class CEventManager {
public:
	CEventManager();
	~CEventManager() {};

private:

	CEventManager(const CEventManager&);
	CEventManager& operator=(const CEventManager&);

	class MemberFunctorBase {
	protected:

		class NeverShouldExist;
		typedef void (NeverShouldExist::*thiscall_method)(const CBaseEventArgs&);
		typedef void(__cdecl NeverShouldExist::*cdecl_method)(const CBaseEventArgs&);
		typedef void(__stdcall NeverShouldExist::*stdcall_method)(const CBaseEventArgs&);
		typedef void(__fastcall NeverShouldExist::*fastcall_method)(const CBaseEventArgs&);
		typedef void(__cdecl *cdecl_function)(const CBaseEventArgs&);
		typedef void(__stdcall *stdcall_function)(const CBaseEventArgs&);
		typedef void(__fastcall *fastcall_function)(const CBaseEventArgs&);

		struct tLargestPointerType {
		private:
			char m_cNeverUse[sizeof(thiscall_method)];
		};

		NeverShouldExist* m_pObject;
		tLargestPointerType m_fpFunction;

	public:
		virtual void operator()(const CBaseEventArgs& pEventArgs) const = 0;
	};

	class MemberFunctorThisCall : public MemberFunctorBase {
	public:
		template<class ObjectType> MemberFunctorThisCall(ObjectType* p, void (ObjectType::*fn)(const CBaseEventArgs& pEventArgs) );
		virtual void operator()(const CBaseEventArgs& pEventArgs) const;
	};

	class MemberFunctorCDeclCall : public MemberFunctorBase {
	public:
		MemberFunctorCDeclCall(void(__cdecl *fn)(const CBaseEventArgs&) );
		virtual void operator()(const CBaseEventArgs& pEventArgs) const;
	};

	class MemberFunctor {
	private:
		char m_cData[sizeof(MemberFunctorBase)];
		const MemberFunctorBase& strategy() const;
	public:
		MemberFunctor(void(__cdecl *fn)(const CBaseEventArgs&) );
		template<class T>
		MemberFunctor(T* p, void (T::*fn)(const CBaseEventArgs&) );
		MemberFunctorBase const& operator*() const throw();
		void operator()(const CBaseEventArgs& pEventArgs) const;
		bool operator==(const MemberFunctor& rhs) const;
	};

	std::map<EVENTID, std::vector<MemberFunctor>> m_ClientDatabase;
	std::vector<CEvent> m_vLowEvents;
	std::vector<CEvent> m_vBelowNormalEvents;
	std::vector<CEvent> m_vNormalEvents;
	std::vector<CEvent> m_vAboveNormalEvents;
	std::vector<CEvent> m_vHighEvents;

	unsigned int m_nLowEventNum;
	unsigned int m_nBelowNormalEventNum;
	unsigned int m_nNormalEventNum;
	unsigned int m_nAboveNormalEventNum;
	unsigned int m_nHighEventNum;

	void DispatchEvent(const CEvent& pEvent);
	bool AlreadyRegistered(const EVENTID& id, const MemberFunctor& pClient);

private:
	static CEventManager*	s_pInstance;
public:
	enum EVENT_PRIORITIES { eEventLow, eEventBelowNormal, eEventNormal, eEventAboveNormal, eEventHigh, eEventInstant };

	static CEventManager* GetInstance();
	static void CreateInstance();
	static void DestroyInstance();

	void SendEvent(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL, const EVENT_PRIORITIES ePriority = eEventNormal);
	void SendEventLow(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
	void SendEventBelowNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
	void SendEventNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
	void SendEventAboveNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
	void SendEventHigh(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
	void SendEventInstant(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);

	template<class T, class Arg>
	bool RegisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&) );

	template<class Arg>
	bool RegisterClient(const EVENTID& id, void(__cdecl *fn)(const Arg&) );

	template<class T, class Arg>
	void UnregisterClientAll(T* p, void (T::*fn)(const Arg&) );

	template<class Arg>
	void UnregisterClientAll(void(__cdecl *fn)(const Arg&) );

	template<class T, class Arg>
	void UnregisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&) );

	template<class Arg>
	void UnregisterClient(const EVENTID&, void(__cdecl *fn)(const Arg&) );

	void ProcessEvents();
	void ClearAllEvents();
	void Shutdown();
};
//--------------------------------------------------------------------------------
#include "EventManager.hpp"
//--------------------------------------------------------------------------------