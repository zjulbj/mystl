//============================================================================
// Name        : allocator.h
// Author      : Bingjie Lv
// Version     : 
// Copyright   : Your copyright notice
// Description : standard interface to  STL Standards
//============================================================================

#ifndef _MALLOCATOR_H
#define _MALLOCATOR_H
#include <new>		//for placement new
#include <cstddef>	//for ptrdiff_t,size_t
#include <cstdlib>	//for exit()
#include <climits>	//for UNIT_MAX
#include <iostream>	//for cerr
#include "malloc_alloc.h"	//trult allocator
namespace numb
{
	//allocator 
	template<class T>
	class allocator
	{
		public:
			//7个基本的内部类型
			typedef T 			value_type;
			typedef	T* 			pointer;
			typedef const T* 	const_pointer;
			typedef T&			reference;
			typedef const T&	const_reference;
			typedef size_t		size_type;
			typedef ptrdiff_t	difference_type;
			//rebind
			template<typename U>
			struct rebind
			{
				typedef allocator<U> other;
			};
			//默认构造函数
			allocator()throw(){}
			//拷贝构造函数
			allocator(const allocator &)throw() {} 
			//不同类型的拷贝
			template<typename Other>  
			allocator(const allocator<Other> &)throw() {} 
			//析构函数
			~allocator()throw(){}
			//分配内存
			pointer	allocate(size_type n,void* hint=0)
			{
				if(n==0)
					return 0;
				if(n>max_size())
					std::__throw_bad_alloc();
				pointer ret=static_cast<T*> (Alloc::allocate(n*sizeof(T*)));
				if(!ret)
					std::__throw_bad_alloc();
				else
				{
					return ret;
				}
			}
			//释放内存
			void deallocate(pointer p,size_type n)
			{
				Alloc::deallocate(static_cast<void*>(p));
			}
			//构造对象
			void construct(pointer p,const T& value)
			{
				::new((void *)p) value_type(value);	//只调用构造函数，不分配内存，p内存已分配好
			}
			//析构对象
			void destroy(pointer p)
			{
				p->~T();
			}
			//定义地址
			pointer address(reference x)
			{
				return (pointer)&x;
			}
			const_pointer address(const_reference x)
			{
				return (const_pointer)&x;
			}
			//返回能够分配的最大内存
			size_type max_size()const
			{
				return size_type(UINT_MAX/sizeof(T));
			}
			
	};	//end of class allocator
	template<typename _Tp>
    inline bool
    operator==(const allocator<_Tp>&, const allocator<_Tp>&)
    { return true; }
  
  template<typename _Tp>
    inline bool
    operator!=(const allocator<_Tp>&, const allocator<_Tp>&)
    { return false; }
}//end of namespcae numb
#endif