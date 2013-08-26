//============================================================================
// Name        : iterator_base.h
// Author      : Bingjie Lv
// Version     : 
// Copyright   : Your copyright notice
// Description : standard interface to  STL Standards
//============================================================================

/*This is a internal header file.
 *It contains all of the general iterator-related utility types,such as
 * iterator_traits and struct iterator and functions, such as distance() 
 *and advance().
 */
#ifndef _MITERATOR_BASE_H
#define _MITERATOR_BASE_H 1
namespace numb
{

/*	Abstructions for uniform iterator,
 *	general iterator-related utility types
 */
	/* Iterator tags
	 * 5 kinds of iterators,such as input iterator,
	 * output iterator.
	 */
	///Input Iterator
	struct input_iterator_tag{};
	///Output Iterator
	struct output_iterator_tag{};
	///Forward Iterator
	struct forward_iterator_tag:public input_iterator_tag {};
	///Bidirectional Iterator
	struct bidirectional_iterator_tag:public forward_iterator_tag {};
	///Random Acess Iterator
	struct random_iterator_tag:public bidirectional_iterator_tag {};
	
	/* Common iterator class
	 * It contains nothing but some nested typedefs
	 * All iterator classes inherit from it.
	 */
	template<typename _Category,
			 typename _T,
			 typename _Distance=ptrdiff_t,
			 typename _Pointer=_T*,
			 typename _Reference=_T&>
	struct iterator
	{
		//5 associated types
		typedef _Category	iterator_category;
		typedef _T			value_type;
		typedef _Distance	difference_type;
		typedef _Pointer 	pointer;
		typedef _Reference  reference;
	};
	
	/* Traits classes for iterators
	 * It contains nothing but some nested typedefs
	 */
	//General version
	template<typename _Iterator>
	struct iterator_traits
	{
		typedef typename _Iterator::value_type 			value_type;
		typedef typename _Iterator::iterator_category 	iterator_category;
		typedef typename _Iterator::difference_type 	difference_type;
		typedef typename _Iterator::pointer 			pointer;
		typedef typename _Iterator::reference 			reference;
	};
	//Partial specialization for pointer types.
	//(such as int*,char*,etc.)
	template<typename _Tp>
	struct iterator_traits<_Tp*>
	{
		typedef _Tp 					value_type;
		typedef random_iterator_tag 	iterator_category;
		typedef ptrdiff_t			 	difference_type;
		typedef _Tp*		 			pointer;
		typedef _Tp&		 			reference;
	};
	//Partial specialization for pointer-to-const types.
	//(such as coonst int*,const char*,etc.)
	template<typename _Tp>
	struct iterator_traits<const _Tp*>
	{
		typedef _Tp 					value_type;
		typedef random_iterator_tag 	iterator_category;
		typedef ptrdiff_t			 	difference_type;
		typedef const _Tp*		 			pointer;
		typedef const _Tp&		 			reference;
	};
	/*
	 *Some general functionals
	 */
	 
	//A function that get the category of a iterator
	template<typename _Iter>
	inline typename iterator_traits<_Iter>::iterator_category
	_iterator_category(const _Iter)
	{
		return typename iterator_traits<_Iter>::iterator_category();
	}

	/* Distance between iterators .
	 * @param   first  An input iterator
	 * @param   last   An input iterator
	 * @return  distance between them.
	 */
	//For all iterators but random acces iterator
	template<typename InputIter>
	inline typename iterator_traits<InputIter>::difference_type
	_distance(InputIter first,InputIter last,input_iterator_tag)
	{
		typename iterator_traits<InputIter>::difference_type n=0;
		while(first!=last)
		{
			++first;
			++n;
		}
		return n;
	}
	//For random acces iterator,@n maybe negetive.
	template<typename RandomIter>
	inline typename iterator_traits<RandomIter>::difference_type
	_distance(RandomIter first,RandomIter last,random_iterator_tag)
	{
		return last-first;
	}
	//interface for client
	//InputIter stands for it is an input iterator at least.
	template<typename InputIter>
	inline typename iterator_traits<InputIter>::difference_type
	distance(InputIter first,InputIter last)
	{
		return _distance(first,last,
						 _iterator_category(first));
	}
	/* Increment/decrement iterators .
	 * @param   i   An input iterator
	 * @param   n   A delta by which to change
	 * @return  nothing.
	 */
	//For input iterator ,n>=0;
	template<typename InputIter,typename Distance>
	inline void _advance(InputIter i,Distance n,input_iterator_tag)
	{
		assert(n>=0);
		while(n--) ++i;
	}
	//for bidirectional iterator
	template <typename BidirecIter,typename Distance>
	inline void _advance(BidirecIter i,Distance n,bidirectional_iterator_tag)
	{
		if(n>0)
			while(n--)
				++i;
		else
			while(n++)
				--i;
	}
	//For random access iterator
	template <typename RandomIter,typename Distance>
	inline void _advance(RandomIter i,Distance n,random_iterator_tag)
	{	i+=n;	}
	//Interface to client
	template <typename InputIter,typename Distance>
	inline void advance(InputIter i,Distance n)
	{
		_advance(i,n,_iterator_category(i));
	}	
}//end of numb
#endif