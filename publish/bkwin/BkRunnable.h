/*************************************************
//  File name:       BkRunnable.h
//  Date:              2020/11/12
//  <author>      hexujun@cmcm.com
//  <time>          2020/11/12
//   <version>     1.0    
*************************************************/
#ifndef _BkRunnable_h_
#define _BkRunnable_h_

#include "BkRef.h"

namespace BkWin
{
	class IRunnable : public BkRefImpl
	{
	public:
		virtual void Run() = 0;
	};


	template <class R>
	class Func_0 : public IRunnable
	{
	public:
		typedef R (*FuncType)(void);
		Func_0(FuncType func_ptr) : _func_ptr(func_ptr){}
		virtual void Run(void)
		{
			(*_func_ptr)();
		}

	private:
		FuncType _func_ptr;
	};

	template <class R, class P1>
	class Func_1 : public IRunnable
	{
	public:
		typedef R (*FuncType)(P1);
		Func_1(FuncType func_ptr, P1& p1) : _func_ptr(func_ptr), _p1(p1){}
		virtual void Run(void)
		{
			(*_func_ptr)(_p1);
		}

	private:
		P1 _p1;
		FuncType _func_ptr;
	};

	template <class R, class P1, class P2>
	class Func_2 : public IRunnable
	{
	public:
		typedef R (*FuncType)(P1, P2);
		Func_2(FuncType func_ptr, P1& p1, P2& p2) : _func_ptr(func_ptr), _p1(p1), _p2(p2){}
		virtual void Run(void)
		{
			(*_func_ptr)(_p1, _p2);
		}

	private:
		P1 _p1;
		P2 _p2;
		FuncType _func_ptr;
	};

	template <class R, class P1, class P2, class P3>
	class Func_3 : public IRunnable
	{
	public:
		typedef R (*FuncType)(P1, P2, P3);
		Func_3(FuncType func_ptr, P1& p1, P2& p2, P3& p3) : func_ptr_(func_ptr), _p1(p1), _p2(p2), _p3(p3){}
		virtual void Run(void)
		{
			(*func_ptr_)(_p1, _p2, _p3);
		}

	private:
		P1 _p1;
		P2 _p2;
		P3 _p3;
		FuncType func_ptr_;
	};

	template <class Class, class R>
	class ClassFunc_0 : public IRunnable
	{
	public:
		typedef R (Class::*FuncType)(void);
		ClassFunc_0(Class* inst, FuncType func_ptr) : inst_(inst), func_ptr_(func_ptr){}
		virtual void Run(void)
		{
			(inst_->*func_ptr_)();
		}

	private:
		FuncType func_ptr_;
		Class* inst_;
	};

	template <class Class, class R, class P1>
	class ClassFunc_1 : public IRunnable
	{
	public:
		typedef R (Class::*FuncType)(P1);
		ClassFunc_1(Class* inst, FuncType func_ptr, P1& p1) : inst_(inst), func_ptr_(func_ptr), _p1(p1){}
		virtual void Run(void)
		{
			(inst_->*func_ptr_)(_p1);
		}

	private:
		P1 _p1;
		FuncType func_ptr_;
		Class* inst_;
	};

	template <class Class, class R, class P1, class P2>
	class ClassFunc_2 : public IRunnable
	{
	public:
		typedef R (Class::*FuncType)(P1, P2);
		ClassFunc_2(Class* inst, FuncType func_ptr, P1& p1, P2& p2) : inst_(inst), func_ptr_(func_ptr), _p1(p1), _p2(p2){}
		virtual void Run(void)
		{
			(inst_->*func_ptr_)(_p1, _p2);
		}

	private:
		P1 _p1;
		P2 _p2;
		FuncType func_ptr_;
		Class* inst_;
	};


	template <class Class, class R, class P1, class P2, class P3>
	class ClassFunc_3 : public IRunnable
	{
	public:
		typedef R (Class::*FuncType)(P1, P2, P3);
		ClassFunc_3(Class* inst, FuncType func_ptr, P1& p1, P2& p2, P3& p3) : inst_(inst), func_ptr_(func_ptr), _p1(p1), _p2(p2), _p3(p3){}

		virtual void Run(void)
		{
			(inst_->*func_ptr_)(_p1, _p2, _p3);
		}

	private:
		P1 _p1;
		P2 _p2;
		P3 _p3;
		FuncType func_ptr_;
		Class* inst_;
	};

	class FuncFactory
	{
	public:
	

		/*
		* @brief 创建一个函数包，在另外一个地方执行，一般用于丢到另外一个线程去执行
		* @example 
			int sum(int a, int b){ return a+b; }
			BkSafePtr<IRunnable> p = FuncFactory::create_func(&sum, 1, 2);

			//any where
			p->run();
		*/
		template <class R>
		static BkSafePtr<IRunnable> create_func(R (*func_ptr)(void))
		{
			BkSafePtr<IRunnable> pFunc(new Func_0<R>(func_ptr), false);

			return pFunc;
		}

		template <class R, class P1>
		static BkSafePtr<IRunnable> create_func(R (*func_ptr)(P1), P1& p1)
		{
			BkSafePtr<IRunnable> pFunc(new Func_1<R, P1>(func_ptr, p1), false);
			return pFunc;
		}

		template <class R, class P1, class P2>
		static BkSafePtr<IRunnable> create_func(R (*func_ptr)(P1, P2), P1& p1, P2& p2)
		{
			BkSafePtr<IRunnable> pFunc(new Func_2<R, P1, P2>(func_ptr, p1, p2), false);
			return pFunc;
		}

		template <class R, class P1, class P2, class P3>
		static BkSafePtr<IRunnable> create_func(R (*func_ptr)(P1, P2, P3), P1& p1, P2& p2, P3& p3)
		{
			BkSafePtr<IRunnable> pFunc(new Func_3<R, P1, P2, P3>(func_ptr, p1, p2, p3), false);
			return pFunc;
		}


		/*
		* @brief 创建一个对象的函数包，在另外一个地方执行，一般用于丢到另外一个线程去执行
		* @example 
				int class::sum(int a, int b){ return a+b; }
				BkSafePtr<IRunnable> p = FuncFactory::create_func(&instance,&class::sum, 1, 2);

				//any where, the instance must be existing
				p->run();
		*/
		template <class Class, class R>
		static BkSafePtr<IRunnable> create_class_func(Class* inst, R (Class::*func_ptr)(void))
		{
			BkSafePtr<IRunnable> pFunc(new ClassFunc_0<Class, R>(inst, func_ptr), false);
			return pFunc;
		}

		template <class Class, class R, class P1>
		static BkSafePtr<IRunnable> create_class_func(Class* inst, R (Class::*func_ptr)(P1), P1 p1)
		{
			BkSafePtr<IRunnable> pFunc(new ClassFunc_1<Class, R, P1>(inst, func_ptr, p1), false);
			return pFunc;
		}

		template <class Class, class R, class P1, class P2>
		static BkSafePtr<IRunnable> create_class_func(Class* inst, R (Class::*func_ptr)(P1, P2), P1 p1, P2 p2)
		{
			BkSafePtr<IRunnable> pFunc(new ClassFunc_2<Class, R, P1, P2>(inst, func_ptr, p1, p2), false);
			return pFunc;
		}

		template <class Class, class R, class P1, class P2, class P3>
		static BkSafePtr<IRunnable> create_class_func(Class* inst, R (Class::*func_ptr)(P1, P2, P3), P1 p1, P2 p2, P3 p3)
		{
			BkSafePtr<IRunnable> pFunc(new ClassFunc_3<Class, R, P1, P2, P3>(inst, func_ptr, p1, p2, p3), false);
			return pFunc;
		}
	};

}


#endif