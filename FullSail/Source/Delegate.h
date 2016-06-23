#pragma once
//--------------------------------------------------------------------------------
template <typename returnType, typename ...arg>
class Delegate {
	using functionType = returnType(*)(void*, arg...);
public:
	using globalFunction = returnType(*)(arg...);
	template<typename T> using memberFunction = returnType(T::*)(arg...);
	template<typename T> using constmemberFunction = returnType(T::*)(arg...) const;

	Delegate();
	Delegate(const Delegate & _in);
	Delegate(const Delegate && _in);
	Delegate & operator = (const Delegate & _in);
	Delegate & operator = (const Delegate && _in);
	returnType operator()(arg... _in) const;
	bool operator == (const Delegate & _in) {
		if (_in.instance == instance) {
			if (_in.type == type)
				return true;
		}
		return false;
	}
	bool operator!() const;
	operator bool() const;
private:
	void* instance;
	functionType type;

	Delegate(void* _obj, functionType _function);

	//Can't move the implemtnation of the following functions to the inline file.  The const function pointer cast makes the compiler choke
public:
	template <globalFunction _function> static Delegate init() {
		return Delegate(0, &functionCall<_function>);
	}
	template <class T, memberFunction<T> _function> static Delegate init(T* _obj) {
		return Delegate(_obj, &functionCall<T, _function>);
	}
	template <class T, constmemberFunction<T> _function> static  Delegate init(T const* _obj) {
		return Delegate(const_cast<T*>(_obj), &functionCall<T, _function>);
	}
private:
	template <globalFunction _function> static returnType functionCall(void*, arg... _in) {
		return (_function)(_in...);
	}
	template <class T, memberFunction<T> _function> static returnType functionCall(void* _obj, arg... _in) {
		return (static_cast<T*>(_obj)->*_function)(_in...);
	}
	template <class T, constmemberFunction<T> _function> static returnType functionCall(void* _obj, arg... _in) {
		return (static_cast<T const*>(_obj)->*_function)(_in...);
	}
};
//--------------------------------------------------------------------------------
#include "Delegate.hpp"
//--------------------------------------------------------------------------------



