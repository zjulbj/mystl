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
#include <cstddef>	//for ptrdiff_t,size_t
#include <cstdlib>	//for exit()
#include <climits>	//for UNIT_MAX
#include <iostream>	//for cerr
namespace numb
{
	//�����ڴ�
	template <class T>
	inline T* _allocate(ptrdiff_t size,T*)
	{
		std::set_new_handler (0);//�趨new_handleΪ0
		T* temp=(T*)(::operator new((size_t)(size*sizeof(T))));//����size��T���͵��ڴ�
		if(temp==0)
		{
			std::cerr<<"Out of Memory!"<<std::endl;
			exit(0);
		}
		return temp;
	}
	//�ͷ��ڴ�
	template <class T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);
	}
	//���ù��캯����placement new
	template <class T1,class T2>
	inline void _construct(T1* p,const T2& value)
	{
		new(p) T1(value);	//ֻ���ù��캯�����������ڴ棬p�ڴ��ѷ����
	}	
	//��������
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
			//7���������ڲ�����
			typedef T 			value_type;
			typedef	T* 			pointer;
			typedef const T* 	const_pointer;
			typedef T&			reference;
			typedef const T&	const_reference;
			typedef size_t		size_type;
			typedef ptrdiff_t	difference_type;
			
			//Ĭ�Ϲ��캯��
			allocator()throw(){}
			//�������캯��
			allocator(const allocator &)throw() {} 
			//��ͬ���͵Ŀ���
			template<class Other>  
			allocator(const allocator<Other> &)throw() {} 
			//��������
			
			~allocator()throw(){}
			//Ϊ����U���°�allocator
			template<class U>
			struct rebind
			{
				typedef allocator<U> other;
			};
			//�����ڴ�
			pointer	allocate(size_type s,void* hint=0)
			{
				return _allocate((difference_type)s,(pointer)0);
			}
			//�ͷ��ڴ�
			void deallocate(pointer p,size_type n)
			{
				_deallocate(p);
			}
			//�������
			void construct(pointer p,const T& value)
			{
				_construct(p,value);
			}
			//��������
			void destroy(pointer p)
			{
				_destroy(p);
			}
			//�����ַ
			pointer address(reference x)
			{
				return (pointer)&x;
			}
			const_pointer address(const_reference x)
			{
				return (const_pointer)&x;
			}
			//�����ܹ����������ڴ�
			size_type max_size()const
			{
				return size_type(UINT_MAX/sizeof(T));
			}
	};	//end of class allocator
}//end of namespcae numb
#endif