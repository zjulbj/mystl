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
			//˽�к������ڲ������ڴ治��
			static void * _handle_oom_malloc(size_t);
			static void * _handle_oom_realloc(void*,size_t);
			static void (* _malloc_handler)();
		public:
			//�����ڴ�
			static void* allocate(size_t _n)
			{
				void *_temp=malloc(_n);
				if(_n==0)//���η���ʧ�ܣ�����handler
				{
					_temp=_handle_oom_malloc(_n);
				}
				return _temp;
			}
			//�ͷ��ڴ�
			static void deallocate(void* _p)
			{
				free(_p);
			}
			//�������ڴ�
			static void* reallocate(void* _p,size_t _n)
			{
				void *_temp=realloc(_p,_n);
				if(_n==0)	//���η���ʧ�ܣ�����handler
					_temp=_handle_oom_realloc(_p,_n);
				return _temp;
			}
			//���ô�����
			static void (* _set_malloc_handler(void (*_f)()))()
			{
				void (* _old)() = _malloc_handler;
				_malloc_handler = _f;
				return(_old);
			}
	};
	void (* _malloc_alloc::_malloc_handler)()= 0;//��ʼ��Ϊ0
	void * _malloc_alloc::_handle_oom_malloc(size_t n)
	{
		void (*my_handler)();
		void *temp;
		while(1)
		{
			my_handler=_malloc_handler;
			if(my_handler==0)//�û�û�趨
			{
				fprintf(stderr, "out of memory\n"); 
				exit(1);
			}
			(*my_handler)();	//��������
			temp=malloc(n);	//�ٳ���
			if(temp) return temp;//�ɹ�
		}
	}
	//����handler��malloc�ľ�����
	void * _malloc_alloc::_handle_oom_realloc(void* p,size_t n)
	{
		void (*my_handler)();
		void *temp;
		while(1)
		{
			my_handler=_malloc_handler;
			if(my_handler==0)//�û�û�趨
			{
				fprintf(stderr, "out of memory\n"); 
				exit(1);
			}
			(*my_handler)();	//��������
			temp=realloc(p,n);	//�ٳ���
			if(temp) return temp;//�ɹ�
		}
	}
	typedef _malloc_alloc malloc_alloc;
	typedef _malloc_alloc Alloc;
}//end of namespcae numb
#endif