// //============================================================================
// // Name        : malloc_alloc.h
// // Author      : Bingjie Lv
// // Version     :
// // Copyright   : Your copyright notice
// // Description :  first level numb::allocator ;using malloc
// //============================================================================

#include <cstddef>	//for size_t
#include <cstdlib>	//for malloc,dealloc,etc.
#include <cstdio>	//for fprintf
#ifndef _MALL_ALLOC_H_
#define _MALL_ALLOC_H_
namespace numb
{
	class _malloc_alloc
	{
		private:
			//私有函数，内部处理内存不足
			static void * _handle_oom_malloc(size_t);
			static void * _handle_oom_realloc(void*,size_t);
			static void (* _malloc_handler)();
		public:
			//分配内存
			static void* allocate(size_t _n)
			{
				void *_temp=malloc(_n);
				if(_n==0)//初次分配失败，调用handler
				{
					_temp=_handle_oom_malloc(_n);
				}
				return _temp;
			}
			//释放内存
			static void deallocate(void* _p)
			{
				free(_p);
			}
			//重配置内存
			static void* reallocate(void* _p,size_t _n)
			{
				void *_temp=realloc(_p,_n);
				if(_n==0)	//初次分配失败，调用handler
					_temp=_handle_oom_realloc(_p,_n);
				return _temp;
			}
			//设置处理句柄
			static void (* _set_malloc_handler(void (*_f)()))()
			{
				void (* _old)() = _malloc_handler;
				_malloc_handler = _f;
				return(_old);
			}
	};
	void (* _malloc_alloc::_malloc_handler)()= 0;//初始化为0
	void * _malloc_alloc::_handle_oom_malloc(size_t n)
	{
		void (*my_handler)();
		void *temp;
		while(1)
		{
			my_handler=_malloc_handler;
			if(my_handler==0)//用户没设定
			{
				fprintf(stderr, "out of memory\n"); 
				exit(1);
			}
			(*my_handler)();	//处理例程
			temp=malloc(n);	//再尝试
			if(temp) return temp;//成功
		}
	}
	//处理handler用malloc的就行了
	void * _malloc_alloc::_handle_oom_realloc(void* p,size_t n)
	{
		void (*my_handler)();
		void *temp;
		while(1)
		{
			my_handler=_malloc_handler;
			if(my_handler==0)//用户没设定
			{
				fprintf(stderr, "out of memory\n"); 
				exit(1);
			}
			(*my_handler)();	//处理例程
			temp=realloc(p,n);	//再尝试
			if(temp) return temp;//成功
		}
	}
	typedef _malloc_alloc malloc_alloc;
	typedef _malloc_alloc Alloc;
}//end of namespcae numb
#endif