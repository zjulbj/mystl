//============================================================================
// Name        : allocator.h
// Author      : Bingjie Lv
// Version     :
// Copyright   : Your copyright notice
// Description : a simple implement of STL::allocator
//============================================================================

#ifndef _MALLOCATOR_H
#define _MALLOCATOR_H
#include <new>		//for placement new
namespace numb
{
	//分配内存
	template <class T>
	inline T* _allocate(ptrdiff_t size,T*)
	{
		std::set_new_handler (0);//设定new_handle为0
		T* temp=static_cast<T*>(::operator new((size_t)(size*sizeof(T))));//分配size个T类型的内存
		if(temp==0)
		{
			throw std::bad_alloc();
		}
		return temp;
	}
	//释放内存
	template <class T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);
	}
	//调用构造函数，placement new
	template <class T1,class T2>
	inline void _construct(T1* p,const T2& value)
	{
		::new(p) T1(value);	//只调用构造函数，不分配内存，p内存已分配好
	}	
	//析构对象
	template <class T>
	inline void _destroy(T* p)
	{
		p->~T();
	}
	
	//allocator 
	template<class T>
	class allocator
	{
		public:
			//7个基本的内部类型
			typedef T 			value_type;
			typedef	 T* 			pointer;
			typedef const T* 	const_pointer;
			typedef T&			reference;
			typedef const T&	const_reference;
			typedef size_t		size_type;
			typedef ptrdiff_t	difference_type;
			
			//默认构造函数
			allocator()throw(){}
			//拷贝构造函数
			allocator(const allocator &)throw() {} 
			//不同类型的拷贝
			template<class Other>  
			allocator(const allocator<Other> &)throw() {} 
			//析构函数
			
			~allocator()throw(){}
			//为类型U重新绑定allocator
			template<class U>
			struct rebind
			{
				typedef allocator<U> other;
			};
			//分配内存
			pointer	allocate(size_type s,void* hint=0)
			{
				return _allocate((difference_type)s,(pointer)0);
			}
			//释放内存
			void deallocate(pointer p,size_type n)
			{
				_deallocate(p);
			}
			//构造对象
			void construct(pointer p,const T& value)
			{
				_construct(p,value);
			}
			//析构对象
			void destroy(pointer p)
			{
				_destroy(p);
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
				return static_cast<size_type>(-1) / sizeof(T);
			}
	};	//end of class allocator
	
	template<typename Tp>
	inline bool
    operator==(const allocator<Tp>&, const allocator<Tp>&)
    { return true; }
  
    template<typename Tp>
    inline bool
    operator!=(const allocator<Tp>&, const allocator<Tp>&)
    { return false; }
	
  /// allocator<void> specialization.
    template<>
    class allocator<void>
    {
    public:
      typedef size_t      size_type;
      typedef ptrdiff_t   difference_type;
      typedef void*       pointer;
      typedef const void* const_pointer;
      typedef void        value_type;

      template<typename T>
        struct rebind
        { typedef allocator<T> other; };
    };
}//end of namespcae numb
#endif