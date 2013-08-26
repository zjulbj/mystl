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
   //memory pool allocator��һ��ʵ��
   //��STL��׼�ӹ�
   template<typename T>
   class pool_alloc
   {
		public:
			//���ֻ����ڲ�����
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
			//�������
			static void 
			construct(pointer __p, const T& __val) 
			{ ::new((void *)__p) T(__val); }
			//��������
			static void 
			destroy(pointer __p) { __p->~T(); }
			//�����ڴ�
			static pointer
			allocate(size_type __n, const void* = 0);
			//�ͷ��ڴ�
			static void
			deallocate(pointer __p, size_type __n);  
		private:
			//����һ���СΪn�Ķ��󣬲�����Ϊfreelist�����ʵ��ڵ�
			static void* _M_refill(size_t n);
			//����һ���ռ�,����nobjs����СΪn������
			//������ò�����nobjs���ܼ�С
			static char* _M_chunk_alloc(size_t n, int& nobjs);
			enum { _S_ALIGN = 8 };			//С�����ϵ��߽�
			enum { _S_MAX_BYTES = 128 };  	//С�����Ͻ�
			enum { _N_FREELISTS= _S_MAX_BYTES / _S_ALIGN};	//�����б����
		
			union Obj			//�����б�ڵ�ṹ��һ������
			{
				union Obj* _M_free_list_link; 
				char 	   _M_client_data[1];	//�û��ɼ�
			};
			
			static Obj* volatile  free_list[_N_FREELISTS];	//16���б�
			
			//ÿ������ķ���״̬,ֻ��_M_chunk_alloc()�б仯
			static char*       _S_start_free; 
			static char*       _S_end_free;
			static size_t      _S_heap_size;  
			//���������С��ȷ����������n,n��1��ʼ
			static size_t
			Get_Index(size_t bytes)
			{return ( ( (bytes) + _S_ALIGN - 1 ) / _S_ALIGN - 1); }
			//�ϵ���С��8�ı���
			static size_t
			_S_round_up(size_t __bytes) 
			{ return (((__bytes) + (size_t) _S_ALIGN-1) & ~((size_t) _S_ALIGN - 1)); }
   
	};
	//��������ȫ�ֲ�����
	template<typename T>
    inline bool
    operator==(const pool_alloc<T>&, const pool_alloc<T>&)
    { return true; }
	template<typename T>
    inline bool
    operator!=(const pool_alloc<T>&, const pool_alloc<T>&)
    { return false; }
	
	//�����ڴ�
	template<typename T>
    T* pool_alloc<T>::allocate(size_type __n, const void*)
    {
		T* ret = 0;
		size_t __size = (__n*sizeof(T));
		if(__size > (size_t) _S_MAX_BYTES)	//����128�ֽڣ�����һ��������
			ret = malloc_alloc<T>::allocate(__size);	//���� T*
		else	//�����ڴ�ط���
		{
			//�ҵ����ʴ�С������
		   Obj* volatile* _my_free_list 
				= free_list + Get_Index(__size);
		   Obj* result = * _my_free_list;
		   if(0==result)//û�ҵ����ã��������
		   {
			   ret = static_cast<T*> (_M_refill(_S_round_up(__size)));	
		   }
		   else	//����free list
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
	//�ͷ��ڴ�
	template<typename T>
    void pool_alloc<T>::deallocate(pointer __p, size_type __n)
	{
		size_t __size=(__n * sizeof(T) );
		if(__size > (size_t) _S_MAX_BYTES )	//����һ��������
			malloc_alloc<T>::deallocate(__p,__n);
		else
		{
			Obj* q=(Obj*) __p;
			Obj* volatile* _my_free_list
				=free_list + Get_Index(__size);
			//��������
			q->_M_free_list_link = * _my_free_list;
			*_my_free_list=q;
		}
	}
	//������䣬__n���ϵ���8�ı���,���Ѽ�����T�����ʹ�С
	template<typename T>
	void* pool_alloc<T>::_M_refill(size_t __n)
	{
		int __nobjs = 20;	//ÿ��20С��
		//��ȡ20���½ڵ�,__nobjs������
		char* __chunk = _M_chunk_alloc(__n, __nobjs);
		Obj* volatile* __my_free_list;
		Obj* __result;
		Obj* __current_obj;
		Obj* __next_obj;
		
		//���ֻ���һ���ڵ�,ֱ�Ӹ�������
		if (1 == __nobjs) return(__chunk);
		//��ֹһ��������free list
		__my_free_list = free_list + Get_Index(__n);
	
		//�ڻ�ȡ�Ŀ��н��������б�
		__result = (Obj*)__chunk;	//��鷵��������
		//��ͷָ����һ��
		*__my_free_list = __next_obj = (Obj*)(__chunk + __n);
		//��ʣ�µļ�������б���β��Ϊ0
		for (int i = 1; ; i++) 
		{
			__current_obj = __next_obj;	 //��i��
			__next_obj = (Obj*)((char*)__next_obj + __n);//i+1��
			if (__nobjs - 1 == i) //������
			{
				__current_obj -> _M_free_list_link = 0;
				break;
			} 
			else //ָ����һ��
				__current_obj -> _M_free_list_link = __next_obj;
		}
		return(__result);
	}
	//size�Ѿ��ϵ���8������nobjs�����ô���
	//
	template<typename T>
	char* pool_alloc<T>::_M_chunk_alloc(size_t size,int& nobjs)
	{
		char* result;
		size_t total_bytes = size * nobjs;	//�ܹ���Ҫ����
		size_t bytes_left = _S_end_free - _S_start_free;	//�ڴ�ػ�ʣ����
		
		//����ڴ����ȫ�ռ���������
		if (bytes_left >= total_bytes)
		{
			result = _S_start_free;
			_S_start_free += total_bytes;
			return(result);
		} 
		//����ȫ���㣬�����ṩһ�鼰����
		else if (bytes_left >= size) 
		{
			nobjs = bytes_left/size;	//�ܹ�Ӧ����
			total_bytes = size * nobjs;	
			result = _S_start_free;
			_S_start_free += total_bytes;	//�����ڴ��
			return(result);
		} 
		//ʣ��ռ���һ��Ҳ�޷�����
		else
		{
			// �Ƚ�ʣ��Ŀռ������ʵ���freelist
			if (bytes_left > 0) 
			{
				Obj* volatile* my_free_list =
							free_list + Get_Index(bytes_left);
	
				((Obj*)_S_start_free) -> _M_free_list_link = *my_free_list;
				*my_free_list = (Obj*)_S_start_free;
			}
			//��ͨ��malloc�������ڴ��
			size_t bytes_to_get = 
				2 * total_bytes + _S_round_up(_S_heap_size >> 4);//�¿����ڴ�ش�С
			_S_start_free = (char*)malloc(bytes_to_get);	//����
			
			if (0 == _S_start_free) 
			{
				//heap�ռ䲻��
				size_t i;
				Obj* volatile* my_free_list;
				Obj* p;
				//��size��ʼ����free list
				for (i = size;
					 i <= (size_t) _S_MAX_BYTES;
					 i += (size_t) _S_ALIGN) 
				{
					my_free_list = free_list + Get_Index(i);
					p = *my_free_list;
					if (0 != p)//�����黹û��
					{
						//����������
						*my_free_list = p -> _M_free_list_link;
						_S_start_free = (char*)p;
						_S_end_free = _S_start_free + i;
						//�ݹ����,����nobjs
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