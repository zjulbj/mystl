//============================================================================
// Name        : malloc_alloc.h
// Author      : Bingjie Lv
// Version     : 
// Copyright   : Your copyright notice
// Description : standard interface to  STL Standards
//============================================================================
#ifndef _mpool_alloc_H
#define	_mpool_alloc_H
#include "malloc_alloc.h"	//malloc_alloc
namespace numb{
  /**  
   *  Dealing with memory requests of little blocks.  
   *
   *  Important implementation properties:
   *  0. If globally mandated, then allocate objects from new
   *  1. If the clients request an object of size > _S_max_bytes, the resulting
   *     object will be obtained directly from new
   *  2. In all other cases, we allocate an object of size exactly
   *     _S_round_up(requested_size).  Thus the client has enough size
   *     information that we can return the object to the proper free list
   *     without permanently losing part of the object.
   */
   //memory pool allocator的一种实现
   //与STL标准接轨
   template<typename T>
   class pool_alloc
   {
		public:
			//七种基本内部类型
			typedef size_t     size_type;
			typedef ptrdiff_t  difference_type;
			typedef T*       pointer;
			typedef const T* const_pointer;
			typedef T&       reference;
			typedef const T& const_reference;
			typedef T        value_type;
			
			template<typename U>
			struct rebind
			{ typedef pool_alloc<U> other;};
			//constructors and destructor
			pool_alloc() throw() {}
			pool_alloc(const pool_alloc&) throw() {}
			template<typename U>
			pool_alloc(const pool_alloc<U>&) throw() {}
			~pool_alloc() throw() {}
			
			static pointer
			address(reference __x){ return &__x; }
			static const_pointer
			address(const_reference __x){ return &__x; }

			static size_type
			max_size()throw() 
			{ return size_t(-1) / sizeof(T); }
			//构造对象
			static void 
			construct(pointer __p, const T& __val) 
			{ ::new((void *)__p) T(__val); }
			//析构对象
			static void 
			destroy(pointer __p) { __p->~T(); }
			//分配内存
			static pointer
			allocate(size_type __n, const void* = 0);
			//释放内存
			static void
			deallocate(pointer __p, size_type __n);  
		private:
			//返回一块大小为n的对象，并可能为freelist增加适当节点
			static void* _M_refill(size_t n);
			//配置一块大空间,容纳nobjs个大小为n的区块
			//如果配置不够，nobjs可能减小
			static char* _M_chunk_alloc(size_t n, int& nobjs);
			enum { _S_ALIGN = 8 };			//小区块上调边界
			enum { _S_MAX_BYTES = 128 };  	//小区块上界
			enum { _N_FREELISTS= _S_MAX_BYTES / _S_ALIGN};	//空闲列表个数
		
			union Obj			//空闲列表节点结构，一物两用
			{
				union Obj* _M_free_list_link; 
				char 	   _M_client_data[1];	//用户可见
			};
			
			static Obj* volatile  free_list[_N_FREELISTS];	//16个列表
			
			//每个区块的分配状态,只在_M_chunk_alloc()中变化
			static char*       _S_start_free; 
			static char*       _S_end_free;
			static size_t      _S_heap_size;  
			//根据区块大小，确定区块索引n,n从1开始
			static size_t
			Get_Index(size_t bytes)
			{return ( ( (bytes) + _S_ALIGN - 1 ) / _S_ALIGN - 1); }
			//上调大小到8的倍数
			static size_t
			_S_round_up(size_t __bytes) 
			{ return (((__bytes) + (size_t) _S_ALIGN-1) & ~((size_t) _S_ALIGN - 1)); }
   
	};
	//重载两个全局操作符
	template<typename T>
    inline bool
    operator==(const pool_alloc<T>&, const pool_alloc<T>&)
    { return true; }
	template<typename T>
    inline bool
    operator!=(const pool_alloc<T>&, const pool_alloc<T>&)
    { return false; }
	
	//分配内存
	template<typename T>
    T* pool_alloc<T>::allocate(size_type __n, const void*)
    {
		T* ret = 0;
		size_t __size = (__n*sizeof(T));
		if(__size > (size_t) _S_MAX_BYTES)	//大于128字节，交给一级配置器
			ret = malloc_alloc<T>::allocate(__size);	//返回 T*
		else	//采用内存池分配
		{
			//找到合适大小的区块
		   Obj* volatile* _my_free_list 
				= free_list + Get_Index(__size);
		   Obj* result = * _my_free_list;
		   if(0==result)//没找到可用，重新填充
		   {
			   ret = static_cast<T*> (_M_refill(_S_round_up(__size)));	
		   }
		   else	//调整free list
		  {
			   *_my_free_list = result->_M_free_list_link;
               ret = reinterpret_cast<T*> (result);
		  }
		   if (ret == 0)
		   {
			  throw std::bad_alloc();
		   }
		}
		return ret;
	}
	//释放内存
	template<typename T>
    void pool_alloc<T>::deallocate(pointer __p, size_type __n)
	{
		size_t __size=(__n * sizeof(T) );
		if(__size > (size_t) _S_MAX_BYTES )	//调用一级配置器
			malloc_alloc<T>::deallocate(__p,__n);
		else
		{
			Obj* q=(Obj*) __p;
			Obj* volatile* _my_free_list
				=free_list + Get_Index(__size);
			//回收区块
			q->_M_free_list_link = * _my_free_list;
			*_my_free_list=q;
		}
	}
	//重新填充，__n已上调至8的倍数,且已计入了T的类型大小
	template<typename T>
	void* pool_alloc<T>::_M_refill(size_t __n)
	{
		int __nobjs = 20;	//每块20小块
		//获取20个新节点,__nobjs是引用
		char* __chunk = _M_chunk_alloc(__n, __nobjs);
		Obj* volatile* __my_free_list;
		Obj* __result;
		Obj* __current_obj;
		Obj* __next_obj;
		
		//如果只获得一个节点,直接给调用者
		if (1 == __nobjs) return(__chunk);
		//不止一个，调整free list
		__my_free_list = free_list + Get_Index(__n);
	
		//在获取的块中建立空闲列表
		__result = (Obj*)__chunk;	//这块返给调用者
		//表头指向下一块
		*__my_free_list = __next_obj = (Obj*)(__chunk + __n);
		//将剩下的加入空闲列表，表尾设为0
		for (int i = 1; ; i++) 
		{
			__current_obj = __next_obj;	 //第i块
			__next_obj = (Obj*)((char*)__next_obj + __n);//i+1块
			if (__nobjs - 1 == i) //最后块了
			{
				__current_obj -> _M_free_list_link = 0;
				break;
			} 
			else //指向下一块
				__current_obj -> _M_free_list_link = __next_obj;
		}
		return(__result);
	}
	//size已经上调至8倍数，nobjs是引用传递
	//
	template<typename T>
	char* pool_alloc<T>::_M_chunk_alloc(size_t size,int& nobjs)
	{
		char* result;
		size_t total_bytes = size * nobjs;	//总共需要多少
		size_t bytes_left = _S_end_free - _S_start_free;	//内存池还剩多少
		
		//如果内存池完全空间满足需求
		if (bytes_left >= total_bytes)
		{
			result = _S_start_free;
			_S_start_free += total_bytes;
			return(result);
		} 
		//不完全满足，但能提供一块及以上
		else if (bytes_left >= size) 
		{
			nobjs = bytes_left/size;	//能供应几块
			total_bytes = size * nobjs;	
			result = _S_start_free;
			_S_start_free += total_bytes;	//更新内存池
			return(result);
		} 
		//剩余空间连一块也无法满足
		else
		{
			// 先将剩余的空间分配给适当的freelist
			if (bytes_left > 0) 
			{
				Obj* volatile* my_free_list =
							free_list + Get_Index(bytes_left);
	
				((Obj*)_S_start_free) -> _M_free_list_link = *my_free_list;
				*my_free_list = (Obj*)_S_start_free;
			}
			//再通过malloc来补充内存池
			size_t bytes_to_get = 
				2 * total_bytes + _S_round_up(_S_heap_size >> 4);//新开的内存池大小
			_S_start_free = (char*)malloc(bytes_to_get);	//配置
			
			if (0 == _S_start_free) 
			{
				//heap空间不足
				size_t i;
				Obj* volatile* my_free_list;
				Obj* p;
				//从size开始遍历free list
				for (i = size;
					 i <= (size_t) _S_MAX_BYTES;
					 i += (size_t) _S_ALIGN) 
				{
					my_free_list = free_list + Get_Index(i);
					p = *my_free_list;
					if (0 != p)//该区块还没用
					{
						//调整该区块
						*my_free_list = p -> _M_free_list_link;
						_S_start_free = (char*)p;
						_S_end_free = _S_start_free + i;
						//递归调用,修正nobjs
						return(_M_chunk_alloc(size, nobjs));
					}
				}
			    _S_end_free = 0;	// In case of exception.
				_S_start_free = (char*)malloc_alloc<T>::allocate(bytes_to_get);
			}
			_S_heap_size += bytes_to_get;
			_S_end_free = _S_start_free + bytes_to_get;
			return(_M_chunk_alloc(size, nobjs));
		}
	}
	template <typename T>
	char* pool_alloc<T>::_S_start_free = 0;
	
	template <typename T>
	char* pool_alloc<T>::_S_end_free = 0;
	
	template <typename T>
	size_t pool_alloc<T>::_S_heap_size = 0;
	
	template <typename T>
	typename pool_alloc<T>::Obj* volatile
	pool_alloc<T> ::free_list[_N_FREELISTS] 
		= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	}
#endif