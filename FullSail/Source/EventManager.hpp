#pragma once
//--------------------------------------------------------------------------------
inline CBaseEventArgs::~CBaseEventArgs() {}
//--------------------------------------------------------------------------------
template<typename T> inline CGeneralEventArgs<T>::CGeneralEventArgs() {}
//--------------------------------------------------------------------------------
template<typename T> inline CGeneralEventArgs<T>::CGeneralEventArgs(const T data) { m_Data = data; }
//--------------------------------------------------------------------------------
template<typename T> inline const T& CGeneralEventArgs<T>::GetData() const{ return m_Data; }
//--------------------------------------------------------------------------------
template<typename T, typename T2> inline CGeneralEventArgs2<T, T2>::CGeneralEventArgs2() {}
//--------------------------------------------------------------------------------
template<typename T, typename T2> inline CGeneralEventArgs2<T, T2>::CGeneralEventArgs2(const T data, const T2 data2) { m_Data = data; m_Data2 = data2; }
//--------------------------------------------------------------------------------
template<typename T, typename T2> inline const T& CGeneralEventArgs2<T, T2>::GetData() const { return m_Data; }
//--------------------------------------------------------------------------------
template<typename T, typename T2> inline const T2& CGeneralEventArgs2<T, T2>::GetData2() const { return m_Data2; }
//--------------------------------------------------------------------------------
inline CEvent::CEvent(const EVENTID& ID, const CBaseEventArgs* pArgs, const int nPriority) : m_ID(ID), m_pArgs(pArgs), m_nPriority(nPriority) {}
//--------------------------------------------------------------------------------
inline const CBaseEventArgs& CEvent::GetArg() const { return *m_pArgs; }
//--------------------------------------------------------------------------------
inline EVENTID CEvent::GetID() const { return m_ID; }
//--------------------------------------------------------------------------------
inline int CEvent::GetPriority() const { return m_nPriority; }
//--------------------------------------------------------------------------------
template<class ObjectType> inline CEventManager::MemberFunctorThisCall::MemberFunctorThisCall(ObjectType* p, void (ObjectType::*fn)(const CBaseEventArgs& pEventArgs)) {
	m_pObject = reinterpret_cast<NeverShouldExist*>(p);
	class AnotherNeverExistClass;
	typedef void (AnotherNeverExistClass::*LargePointerToMethod)(const CBaseEventArgs&);
	sizeof(LargePointerToMethod);
	class AnotherNeverExistClass : public ObjectType {};
	reinterpret_cast<LargePointerToMethod&>(m_fpFunction) = fn;
}
//--------------------------------------------------------------------------------
inline void  CEventManager::MemberFunctorThisCall::operator()(const CBaseEventArgs& pEventArgs) const {
	(m_pObject->*(*(thiscall_method*)(&m_fpFunction)))(pEventArgs);
}
//--------------------------------------------------------------------------------
inline  CEventManager::MemberFunctorCDeclCall::MemberFunctorCDeclCall(void(__cdecl *fn)(const CBaseEventArgs&) ) {
	m_pObject = 0;
	reinterpret_cast<cdecl_function&>(m_fpFunction) = fn;
	// fill redundant bytes by ZERO for later sorting
	memset((char*)(&m_fpFunction) + sizeof(fn), 0, sizeof(m_fpFunction) - sizeof(fn));
}
//--------------------------------------------------------------------------------
inline void  CEventManager::MemberFunctorCDeclCall::operator()(const CBaseEventArgs& pEventArgs) const {
	return (*(*(cdecl_function*)(&m_fpFunction) ) )(pEventArgs);
}
//--------------------------------------------------------------------------------
inline const CEventManager::MemberFunctorBase& CEventManager::MemberFunctor::strategy() const {
	return *reinterpret_cast<MemberFunctorBase const*>(&m_cData);
}
//--------------------------------------------------------------------------------
inline CEventManager::MemberFunctor::MemberFunctor(void(__cdecl *fn)(const CBaseEventArgs&) ) {
	new (&m_cData) MemberFunctorCDeclCall(fn);
}
//--------------------------------------------------------------------------------
inline bool CEventManager::MemberFunctor::operator==(const MemberFunctor& rhs) const {
	if (memcmp(m_cData, rhs.m_cData, sizeof(MemberFunctorBase) ) == 0)
		return true;
	return false;
}
//--------------------------------------------------------------------------------
inline void CEventManager::MemberFunctor::operator()(const CBaseEventArgs& pEventArgs) const {
	return strategy()(pEventArgs);
}
//--------------------------------------------------------------------------------
template<class T> inline CEventManager::MemberFunctor::MemberFunctor(T* p, void (T::*fn)(const CBaseEventArgs&) ) {
	new (&m_cData) MemberFunctorThisCall(p, fn);
}
//--------------------------------------------------------------------------------
inline CEventManager::MemberFunctorBase const& CEventManager::MemberFunctor::operator*() const throw() {
	return strategy();
}
//--------------------------------------------------------------------------------
inline CEventManager* EventManager() { return CEventManager::GetInstance(); }
//--------------------------------------------------------------------------------
template<class Arg> bool  CEventManager::RegisterClient(const EVENTID& id, void(__cdecl *fn)(const Arg&) ) {
	if (fn == NULL)
		return false;
	MemberFunctor newClient(reinterpret_cast<void(__cdecl *)(const CBaseEventArgs&)>(fn) );
	map <EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);

	if (iter != m_ClientDatabase.end() ) {
		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x<clients->size(); ++x)
			if ((*clients)[x] == newClient)
				return false;
		clients->push_back(newClient);
	}
	else {
		std::pair<EVENTID, vector<MemberFunctor>> newPair(id, vector<MemberFunctor>() );
		newPair.second.push_back(newClient);
		m_ClientDatabase.insert(newPair);
	}
	return true;
}
//--------------------------------------------------------------------------------
template<class T, class Arg> bool  CEventManager::RegisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&) ) {
	using namespace std;

	if (p == NULL || fn == NULL)
		return false;
	MemberFunctor newClient(p, reinterpret_cast<void (T::*)(const CBaseEventArgs&)>(fn) );
	map <EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);

	if (iter != m_ClientDatabase.end() ) {
		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x<clients->size(); ++x)
			if ((*clients)[x] == newClient)
				return false;
		clients->push_back(newClient);
	}
	else {
		std::pair<EVENTID, vector<MemberFunctor>> newPair(id, vector<MemberFunctor>() );
		newPair.second.push_back(newClient);
		m_ClientDatabase.insert(newPair);
	}
	return true;
}
//--------------------------------------------------------------------------------
template<class T, class Arg> void CEventManager::UnregisterClientAll(T* p, void (T::*fn)(const Arg&) ) {
	map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.begin();
	MemberFunctor clientToDelete(p, reinterpret_cast<void (T::*)(const CBaseEventArgs&)>(fn) );
	while (iter != m_ClientDatabase.end() ) {
		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x<clients->size(); ++x) {
			if ((*clients)[x] == clientToDelete) {
				clients->erase(clients->begin() + x);
				break;
			}
		}
		++iter;
	}
}
//--------------------------------------------------------------------------------
template<class Arg> void CEventManager::UnregisterClientAll(void(__cdecl *fn)(const Arg&) ) {
	using namespace std;
	map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.begin();
	MemberFunctor clientToDelete(reinterpret_cast<void(__cdecl *)(const CBaseEventArgs&)>(fn) );
	while (iter != m_ClientDatabase.end() ) {
		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x<clients->size(); ++x) {
			if ((*clients)[x] == clientToDelete)
			{
				clients->erase(clients->begin() + x);
				break;
			}
		}
		++iter;
	}
}
//--------------------------------------------------------------------------------
template<class T, class Arg> void  CEventManager::UnregisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&) ) {
	using namespace std;
	map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);
	if (m_ClientDatabase.end() == iter)
		return;
	MemberFunctor clientToDelete(p, reinterpret_cast<void (T::*)(const CBaseEventArgs&)>(fn) );
	vector<MemberFunctor> *clients = &(*iter).second;
	for (unsigned int x = 0; x<clients->size(); ++x) {
		if ((*clients)[x] == clientToDelete)
		{
			clients->erase(clients->begin() + x);
			break;
		}
	}
}
//--------------------------------------------------------------------------------
template<class Arg> void  CEventManager::UnregisterClient(const EVENTID& id, void(__cdecl *fn)(const Arg&) ) {
	map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);
	if (m_ClientDatabase.end() == iter)
		return;
	MemberFunctor clientToDelete(reinterpret_cast<void(__cdecl *)(const CBaseEventArgs&)>(fn) );
	vector<MemberFunctor> *clients = &(*iter).second;
	for (unsigned int x = 0; x<clients->size(); ++x) {
		if ((*clients)[x] == clientToDelete) {
			clients->erase(clients->begin() + x);
			break;
		}
	}
}
//--------------------------------------------------------------------------------