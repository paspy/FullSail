#pragma once
//--------------------------------------------------------------------------------
//This file is not supposed to be included.  It will break if you include this file on it's own
//Instead include Delegate.h, Register messages.h or call messages.h
template <typename R, typename ...arg> Delegate<R, arg...>::Delegate() : instance(nullptr), type(nullptr) {}
template <typename R, typename ...arg> Delegate<R, arg...>::Delegate(const Delegate & _in) : instance(_in.instance), type(_in.type) {}
template <typename R, typename ...arg> Delegate<R, arg...>::Delegate(const Delegate && _in) : instance(_in.instance), type(_in.type) {}
template <typename R, typename ...arg> Delegate<R, arg...>::Delegate(void* _obj, functionType _function) : instance(_obj), type(_function) { 
	int poop = 0;
}
template <typename R, typename ...arg> inline Delegate<R, arg...>::operator bool() const { 
	return nullptr != type;
}
template <typename R, typename ...arg> inline bool Delegate<R, arg...>::operator!() const { return !(operator bool()); }
template <typename R, typename ...arg> inline R Delegate<R, arg...>::operator()(arg... _in) const{ return (*type)(instance, _in...); }
template <typename R, typename ...arg>
inline Delegate<R, arg...> & Delegate<R, arg...>::operator = (const Delegate & _in) { instance = _in.instance; type = _in.type; return *this; }
template <typename R, typename ...arg>
inline Delegate<R, arg...> & Delegate<R, arg...>::operator = (const Delegate && _in) { instance = _in.instance; type = _in.type; return *this; }
//--------------------------------------------------------------------------------