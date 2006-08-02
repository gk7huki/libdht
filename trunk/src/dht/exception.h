#ifndef _DHT_EXCEPTION_H_
#define _DHT_EXCEPTION_H_
/**
 * @file    exception.h
 * @date    Apr 9, 2005
 * @author  Arto Jalkanen
 * @brief   defines exceptions that might thrown by this module
 *
 * Fatal errors will be thrown by this module as exceptions. This file
 * defines used exception classes. Each exception carries a code
 * and an additional error string.
 */

#include <exception>
#include <stdarg.h>

#include "common.h"
 
namespace dht {
	/*
	class exception : public std::exception {
	private:
		const char *_str;
		int         _code;
	public:
		exception(int code, const char *reason) :
		  _str(reason), _code(code) {
		}
		virtual ~exception() throw();
		virtual const char *what() const throw() { return _str; }
		virtual int         code() const { return _code; }
	};
	
	class exceptionf : public dht::exception {
	private:
		char _str_store[256];
		
	public:
		exceptionf(int code, const char *format, ...) 
			: exception(code, format) {
			va_list a;
			va_start(a, format);
			vsnprintf(_str_store, sizeof(_str_store)/sizeof(_str_store[0]), 
			          format, a);
			va_end(a);

		}
		virtual ~exceptionf() throw();
		virtual const char *what() const throw() { return _str_store; }
	};
	*/
	class exception : public std::exception {
	private:
		const char *_str;
	public:
		exception(const char *reason) :
		  _str(reason) {
		  	ACE_ERROR((LM_ERROR, "dht::exception: %s\n", reason));
		}
		virtual ~exception() throw();
		virtual const char *what() const throw() { return _str; }
	};
	
	
	class exceptionf : public dht::exception {
	private:
		char _str_store[256];
	protected:
		inline void _initv(const char *format, va_list a) {
			vsnprintf(_str_store, sizeof(_str_store)/sizeof(_str_store[0]), 
			          format, a);			
		}
		struct initv_ctor {};
		exceptionf(const char *format, initv_ctor)
			: exception(format) {}
	public:
		exceptionf(const char *format, ...) 
			: exception(format) {
			va_list a;
			va_start(a, format);
			_initv(format, a);
			va_end(a);
		}
		virtual ~exceptionf() throw();
		virtual const char *what() const throw() { return _str_store; }
	};
	
	template <typename ParentType = exception>
	class exception_class : public ParentType {
	public:
		exception_class(const char *reason) : ParentType(reason) {}
	};
	
	template <typename ParentType = exceptionf>
	class exception_classf : public ParentType {
	public:
		exception_classf(const char *format, ...) 
		  : ParentType(format, typename ParentType::initv_ctor()) {
			va_list a;
			va_start(a, format);
			ParentType::_initv(format, a);
			va_end(a);
		}
	};
	
	#define _DHT_EXCEPTION_CLASS(name) \
		typedef exception_class<>  name;\
		typedef exception_classf<> name##f;

	#define _DHT_EXCEPTION_SUBCLASS(name, parent) \
		typedef exception_class<parent>  name;\
		typedef exception_classf<parent##f> name##f;
				
	_DHT_EXCEPTION_CLASS(call_error)
	_DHT_EXCEPTION_CLASS(operation_error)
	_DHT_EXCEPTION_CLASS(io_error)
	_DHT_EXCEPTION_CLASS(unexpected_error)
	
}
 
#endif //_DHT_EXCEPTION_H_
