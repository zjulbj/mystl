//============================================================================
// Name        : malloc_alloc.h
// Author      : Bingjie Lv
// Version     : 
// Copyright   : Your copyright notice
// Description : standard interface to  STL Standards
//============================================================================
#ifndef _mmalloc_alloc_H
#define	_mmalloc_alloc_H
#include <cstddef>	//for size_t
#include <cstdlib>	//for malloc,dealloc,etc.
#include <cstdio>	//for stderr
#include <new>		//for placement new
namespace numb
{
template <class T> class malloc_alloc
{
public:
  typedef T                 value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef std::size_t       size_type;
  typedef std::ptrdiff_t    difference_type;
  
  template <class U> 
  struct rebind { typedef malloc_alloc<U> other; };

  malloc_alloc() {}
  malloc_alloc(const malloc_alloc&) {}
  template <class U> 
  malloc_alloc(const malloc_alloc<U>&) {}
  ~malloc_alloc() {}

  static pointer address(reference x){ return &x; }
  static const_pointer address(const_reference x){ return &x;}
  //分配
  static pointer allocate(size_type n, const_pointer = 0) 
  {
    size_t _t=(n * sizeof(T));
	void* p = std::malloc(_t);
    if (!p)
       p=oom_malloc(_t);
    return static_cast<pointer>(p);
  }
  //释放
  static void deallocate(pointer p, size_type ) 
  { 
	free(p);
  }
  //最大容量
  static size_type max_size() 
  {  return static_cast<size_type>(-1) / sizeof(T);}
  //构造
  static void construct(pointer p, const value_type& x) 
  { 
    new (p) value_type(x); 
  }
  //析构
  static void destroy(pointer p) { p->~value_type(); }
   //重配置内存
  static void* reallocate(void* _p,size_t _n)
  {
	void *_temp=realloc(_p,_n);
	if(_temp==0)	//初次分配失败，调用handler
		_temp=oom_realloc(_p,_n);
	return _temp;
  }
  //设置处理句柄
  static void (* set_malloc_handler(void (*f)()))()
  {
    void (* _old)() = _malloc_handler;
	_malloc_handler = f;
	return(_old);
  }
  bool operator==(const malloc_alloc &) const
  {
	return true;
  }
  bool operator!=(const malloc_alloc &) const
  {
	return false;
  }
private:
  void operator=(const malloc_alloc&);
  static void * oom_malloc(size_t);
  static void * oom_realloc(void*,size_t);
  static void (* _malloc_handler)();
};
template<class T>
void (* malloc_alloc<T>::_malloc_handler)()= 0;//初始化为0
template<class T>
void * malloc_alloc<T>::oom_malloc(size_t n)
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
template<class T>
void * malloc_alloc<T>::oom_realloc(void* p,size_t n)
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


///void版的
template<> class malloc_alloc<void>
{
  typedef void        value_type;
  typedef void*       pointer;
  typedef const void* const_pointer;

  template <class U> 
  struct rebind { typedef malloc_alloc<U> other; };
};
template <class T>
inline bool operator==(const malloc_alloc<T>&, 
                       const malloc_alloc<T>&) {
  return true;
}

template <class T>
inline bool operator!=(const malloc_alloc<T>&, 
                       const malloc_alloc<T>&) {
  return false;
}
}
#endif