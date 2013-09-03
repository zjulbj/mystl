//============================================================================
// Name        : stl_vector.h
// Author      : Bingjie Lv
// Version     : 
// Copyright   : Your copyright notice
// Description : standard interface to  STL Standards
//============================================================================

/*This is a internal header file.
 *It's a implement of vector
 */
#ifndef _MSTL_VECTOR_H
#define _MSTL_VECTOR_H
#include<type_traits>
namespace numb
{
	//a base class to allocate/deallocate memory,without initializing. 
	template<typename Tp,typename Alloc>
	struct Vec_base
	{
		typedef typename Alloc::template rebind<Tp>::other  Tp_alloc_type;	
		typedef Alloc											 allocator_type;
		//A class derived from Tp_alloc_type to manage the memory
		struct Vec_impl:public Tp_alloc_type
		{
			typename Tp_alloc_type::pointer start;
			typename Tp_alloc_type::pointer finish;
			typename Tp_alloc_type::pointer end_of_storage;
			//default ctor
			Vec_impl():
			Tp_alloc_type(),start(0),finish(0),end_of_storage(0)
			{}
			//ctor with a @param Tp_alloc_type
			Vec_impl(const Tp_alloc_type& a):
			Tp_alloc_type(a),start(0),finish(0),end_of_storage(0)
			{}			
		};
		//get the allocator type
		Tp_alloc_type&
		get_Tp_allocator()
		{
			return *static_cast<Tp_alloc_type*> (&this->impl);
		}
		const Tp_alloc_type&
		get_Tp_allocator()const
		{
			return *static_cast<const Tp_alloc_type*> (&this->impl);
		}
		allocator_type&
		get_allocator()const
		{
			allocator_type(get_Tp_allocator());
		}
		public:
			Vec_impl impl;
			//ctors and dctor
			Vec_base():impl(){}
			Vec_base(const allocator_type& a):impl(a){}
			Vec_base(size_t n):impl()
			{
				create_n(n);
			}
			Vec_base(size_t n,const allocator_type&a):impl(a)
			{
				create_n(n);
			}
			/*To-do:
			 *  move ctors
			 */
			~Vec_base()
			{
				M_deallocate(this->impl.start,
						this->impl.end_of_storage-this->impl.start);
			}
			
			
		public:
			typename Tp_alloc_type::pointer
			M_allocate(size_t n)
			{
				return n!=0?this->impl.allocate(n):0;
			}
			void M_deallocate(typename Tp_alloc_type::pointer p, size_t n)
			{
				if(p)
					this->impl.deallocate(p,n);
			}
		private:
			void create_n(size_t n)
			{
				this->impl.start=this->M_allocate(n);
				this->impl.finish=this->impl.start;
				this->impl.end_of_storage=this->impl.start+n;
			}
			
	};//end of Vec_base
	/*
	 *	A stanard vector implement
	 *	@tparam _Tp  Type of element.
	 *	@tparam _Alloc  Allocator type, defaults to allocator<_Tp>.
	 *
	 */
	template<typename Tp,typename Alloc=numb::allocator<Tp> >
	class vector:protected Vec_base<Tp,Alloc>
	{
	 //nested typedefs
		//internal typedefs
		typedef	 typename Alloc::value_type 	Alloc_value_type;
		typedef  Vec_base<Tp,Alloc>			    Base;
		typedef	 typename Base::Tp_alloc_type	 Tp_alloc_type;
		/*
		 *To-do:Concept requirements
		 *	1.requires(Tp,SGIAssignableConcept)
		 *	2.requires(Tp, Alloc_value_type, SameTypeConcept)
		 */
		//public typedefs
		public:
			typedef Tp 											value_type;
			typedef typename Tp_alloc_type::pointer				pointer;
			typedef typename Tp_alloc_type::const_pointer		const_pointer;
			typedef typename Tp_alloc_type::reference			reference;
			typedef typename Tp_alloc_type::const_reference		const_reference;
			typedef ptrdiff_t								difference_type;
			typedef size_t									size_type;
			typedef  Alloc 								allocator_type;
			typedef typename numb::_normal_iterator<pointer,vector> iterator;
			typedef typename numb::_normal_iterator<const_pointer,vector> 
													const_iterator;
			/*To do:
			 *	1.reverse_iterator
			 *	2.const_reverse_iterator	
			 */
		protected:
			using Base::M_allocate;
			using Base::M_deallocate;
			using Base::get_Tp_allocator;
			using Base::impl;
		public:
	
	//Construct/copy/destory	
		/*	
		 *  default ctor
		 *	creates no element;
		 */
		vector():Base(){}
		/* 
		 * ctor with a @param a An allocator
		 * creates no element
		 */
		explicit vector(const allocator_type& a):Base(a){}
		/* 
		 * ctor with a @param n Number of elements
		 * creates n default constructed elemets 
		 */	
		 explicit vector(size_type n):Base(n)
		 {	M_default_initialize(n);}
		/*
         *  Creates a vector with n copies of value
         *  @param  n  The number of elements to initially create.
         *  @param  value  An element to copy.
         *  @param  a  An allocator.
         */
		explicit
		vector(size_type n, 
				const value_type& value = value_type(),
				const allocator_type& a = allocator_type())
		: Base(n, a)
		{ 
			M_fill_initialize(n,value); 
		}
		/*
         *  Creates a vector from a range
         *  @param  first  Input iterator.
         *  @param  last   Input iterator.
         *  @param  a  An allocator.
         */
		 template<typename InputIter>
		 vector(InputIter first,InputIter last,const allocator_type a=allocator_type())
		 :Base(a)
		 {
			//avoid ambiguity from vector(int,int)
			typedef typename std::is_integral<InputIter>::type Integral;
			M_fill_range(first,last,Integral());	
		 }
		/*
         *  Copy constructor.
         *  @param  x  A vector of identical element and allocator types.
         */
		vector(const vector& x)
		: Base(x.size(), x.get_Tp_allocator())
        { 
			//To do:__uninitialized_copy_a
			this->impl.finish =
				std::__uninitialized_copy_a(x.begin(), x.end(),
				      this->_M_impl._M_start,
				      get_Tp_allocator());
		}
		~vector()
		{
			//To do:_Destroy
			std::_Destroy(this->impl.start, this->impl.finish,
		      get_Tp_allocator()); 
		}
		//Copy assignment operator.
		//Replaces the contents with a copy of the contents of other.
		vector&
		operator=(const vector& x);
	//assign
		/*
		 * Replaces the contents with n copies of v
		 * Unlike copy ctor,extra memory and allocator are not copied
		 */
		void assign(size_type n,const value_type& v)
		{	M_fill_assign(n,v);	}
		
		template<class InputIter>
		void assign(InputIter first,InputIter last)
		{
			//avoid ambiguity from assign(int,int);
			typedef typename std::is_integral<InputIter>::type Integral;
			M_assign_range(first,last,Integral());
		}
	//get_allocator
		using Base::get_allocator;
		
	// Iterators:	
		// return a iterator of first element
		iterator begin()
		{	return iterator(this->impl.start);}
		const_iterator begin()const
		{	return const_iterator(this->impl.start);}
		
		// return a iterator of one past the last element
		iterator end()
		{ return iterator(this->impl.finish);}
		const_iterator end()const
		{ return const_iterator(this->impl.finish);}
		
		//to do: rbegin()
		//to do: rend()
	//Capacity	:
		//number of elements in the vector
		size_type size()const
		{	return size_type(this->impl.finish-this->impl.start);}
		//largest possible size()
		size_type max_size()const
		{
			return get_Tp_allocator().max_size();
		}
		//capacity now
		size_type capacity() const
		{ return size_type(this->impl.end_of_storage
			 - this->impl.start); }
		//empty or not
		bool empty() const
		{ return begin() == end(); }
		
		//to do:reverse
		//to do:shrink_to_fit  c++11
		
	// Element access:	
		//return n-th element's reference
		//without checking range
		reference operator[](size_type n)
		{ return *(this->impl.start + n); }
		const_reference operator[](size_type n)const
		{ return *(this->impl.start + n); }
		
		//similar with operator []
		//but do range checking
		protected:
		void M_range_check(size_type n)
		{
			if(n>=this->size())
			{
			  throw std::out_of_range("vector:M_range_check");
			}
		}
		public:
		reference at(size_type n)
		{
			M_range_check(n);
			return (*this)[n]; 
		}
		const_reference at(size_type n)const
		{
			M_range_check(n);
			return (*this)[n]; 
		}
		reference front()
		{	return *begin();}
		const_reference front() const
		{ return *begin();}
		reference back()
		{ return *(end()-1);}
		const_reference back()const
		{ return *(end()-1);}
		//to do:data() c++11
		
	// Modifiers
		//Add a element at the end of vector
		void push_back(const value_type& x)
		{
			if(this->impl.finish!=this->impl.end_of_storage)
			{
				this->impl.construct(this->impl.finish,x);
				++this->impl.finish;
			}
			else
			{
				//to do
				M_insert_aux(end(),x);
			}
		}
		//Remove last element
		void pop_back()
		{
			--this->impl.finish;
			this->impl.destroy(this->impl.finish);
		}
		
		//Insert
		//insert(p,v)
		iterator insert(iterator p, const value_type& v);
		//insert(p,n,v)
		void insert(iterator p, size_type n, const value_type& x)
		{ M_fill_insert(p, n,x); }
		//insert(p,first,last)
		template<typename InputIter>
        void insert(iterator p, InputIter first,InputIter last)
        {
			// Check whether it's an integral type.  If so, it's not an iterator.
			typedef typename std::is_integral<InputIter>::type Integral;
			M_insert_range(p, first, last, Integral());
		}
		//to do:erase
		//erase(p);
		iterator erase(iterator p);
		//erase(first,last)
		iterator erase(iterator first, iterator last);
		//swap with vector x
		void swap(vector& x)
		{
			std::swap(this->impl.start, x.impl.start);
			std::swap(this->impl.finish, x.impl.finish);
			std::swap(this->impl.end_of_storage,
				x.impl.end_of_storage);
			//to do:swap allocator
		}
		//clear all the element
		void clear()
		{	M_erase_at_end(this->impl.start);}
		//resize the vector with new_size element v
		void resize(size_type new_size,value_type v)
		{
			if(new_size>size())//insert before the end
			{
				insert(end(), new_size-size(), v);
			}
			else if(new_size<size())//erase the redundant
			{
				M_erase_at_end(this->impl.start + new_size);
			}
			//do nothing if new_size==size()
		}
	protected:
	//internal functions
		/*
		 *  Memory expansion handler. To obtain @a n bytes of memory, 
		 *  and then copies [first,last) into it.
         */
		template<class ForwardIter>
		pointer 
		M_allocate_and_copy(size_type n,
							ForwardIter first,
							ForwardIter last)
		{
			//allocate new memory
			pointer tmp=M_allocate(n);
			try
			{
				std::__uninitialized_copy_a(first,last,tmp,get_Tp_allocator());
				return tmp;
			}
			catch(...)
			{
				//if failed,release the memory
				M_deallocate(tmp,n);
				throw;
			}
		}
		/*
		 * Internal constructor functions follow.
		 * Called by the range constructor to implement
		 */
		//call by vector(n) ctor
		void M_default_initialize(size_type n)
		{
			//create n default elements
			std::__uninitialized_default_n_a(this->impl.start, n, 
					 get_Tp_allocator());
			this->impl.finish = this->impl.end_of_storage;
		}
		//call by vector(n,v,a) ctor
		void M_fill_initialize(size_type n, const value_type& value)
		{
			std::__uninitialized_fill_n_a(this->impl.start, n,value, 
				      get_Tp_allocator());
			this->impl.finish = this->impl.end_of_storage;
		}
		//call by vector(first,last) ctor
		//in case of vector(int,int)
		template<class Integer>
		void M_fill_range(Integer n,
						  Integer v,
						  std::true_type)
		{
			this->impl.start=M_allocate(static_cast<size_type>(n));
			this->impl.end_of_storage=this->impl.start+static_cast<size_type>(n);
			M_fill_initialize(static_cast<size_type>(n),v);
		}
		//construct from a range
		template<class InputIter>
		void M_fill_range(InputIter first,
						  InputIter last,
						  std::false_type)
		{
			//depend on iterator type
			M_fill_range_aux(first,last,numb::_iterator_category(first));
		}
		//if it's input iterator
		template<class InputIter>
		void M_fill_range_aux(
				InputIter first,
				InputIter last,
				numb::input_iterator_tag)
		{
			for(;first!=last;++first)
				push_back(*first);
		}
		//or it's forward iterator
		template<class ForwardIter>
		void M_fill_range_aux(
				ForwardIter first,
				ForwardIter last,
				numb::forward_iterator_tag)
		{
			const size_type n=numb::distance(first,last);
			this->impl.start=M_allocate(n);//allocate memory 
			this->impl.end_of_storage=this->impl.start+n;
			this->impl.finish=std::__uninitialized_copy_a(
								first,last,this->impl.start,get_Tp_allocator());
		}
		/*
		 * Internal assign functions follow.  The *_aux functions do the actual
		 * assignment work for the range versions.
		 * Called by the range assign to implement 
		 */
		 //for assign(int,int)
		 template<typename Integer>
		 void M_assign_range(Integer n,Integer v,std::true_type)
		 {	M_fill_assign(n,v);	 }
		 //for assign(iterator,iterator)
		 template<typename InputIter>
		 void M_assign_range(InputIter first,InputIter last,std::false_type)
		 {	
			M_assign_range_aux(first,last,
							   numb::_iterator_category(first));
         }
		 //declarations for some other assign function
		 //they aren't defined as inline functions
		 template<typename InputIter>
		 void M_assign_range_aux(InputIter first,InputIter last,
								 numb::input_iterator_tag);
		 template<typename ForwardIter>
		 void M_assign_range_aux(ForwardIter first,ForwardIter last,
								 numb::forward_iterator_tag);
		 void M_fill_assign(size_type n,const value_type& v);
		/*
		 *Internal insert functions follow.
		 */
		 void M_insert_aux(iterator p,const value_type& v);
		/*
		 * Internal erase functions follow.
		 * Called by erase(q1,q2), clear(), resize(), M_fill_assign,
		 * M_assign_aux.
		 */
		void
		M_erase_at_end(pointer p)
		{
			std::_Destroy(p, 
						this->impl.finish,
						get_Tp_allocator());
			this->impl.finish = p;
		}
	};//end of vector
	//public:operator=
	template<typename Tp,typename Alloc>
	vector<Tp,Alloc>&
	vector<Tp,Alloc>::operator=(const vector<Tp,Alloc>& x)
	{
		if(&x!=this)//avoid self-assign
		{
			const size_type xlen=x.size();
			if(xlen>capacity())	//need more memory	
			{
				//allocate and copy to buffer
				pointer tmp=M_allocate_and_copy(xlen,x.begin(),x.end());
				//destroy the all the old items
				std::_Destroy(this->impl.start, this->impl.finish,
								get_Tp_allocator()); 
				//release the old memory
				M_deallocate(this->impl.start,capacity());
				//update current vector
				this->impl.start=tmp;
				this->impl.end_of_storage=this->impl.start+xlen;
			}
			else if(xlen>size())
			{
				//the [start,size()) part,use std::copy
				std::copy(x.impl.start,
						  x.impl.start+size(),
						  this->impl.start);
				//the [size(),xlen) part,use std::__uninitialized_copy_a
				std::__uninitialized_copy_a(x.impl.start+size(),
							x.impl.finish,
							this->impl.finish,
							get_Tp_allocator());
			}
			else
			{
				//copy all items of x to *this
				//destroy the rest 
				std::_Destroy(
					std::copy(x.begin(),
							  x.end(),
							  begin()),
					end(),
					get_Tp_allocator());
			}
			this->impl.finish=this->impl.start+xlen;
		}
		return *this;
	}
	//protected:M_fill_assign(n,v)
	template<typename Tp,typename Alloc>
	void
	vector<Tp,Alloc>::M_fill_assign(size_type n,const value_type& v)
	{
		if(n>capacity())//need more memory
		{
			vector<Tp,Alloc> tmp(n,v,get_Tp_allocator());
			tmp.swap(*this);
			//tmp will be destroyed once out of this scope
		}
		else if(n>size())
		{
			//For [begin,size) part,use std::fill
			std::fill(begin(),end(),v);
			//For [size,n) part,use std::__uninitialized_fill
			std::__uninitialized_fill_n_a(end(),n-size(),v,
							get_Tp_allocator());
			this->impl.finish=this->impl.start+n;
		}
		else
		{
			//fill [begin,begin+n) with v,erase the rest[begin+n,end)
			M_erase_at_end(std::fill_n(this->impl.start,n,v));
		}
	}
	//protected:M_assign_range_aux(first,last,iterator_tag)
	template<typename Tp,typename Alloc> template<typename InputIter> 
	void vector<Tp,Alloc>::
	M_assign_range_aux(InputIter first,
					   InputIter last,
					   numb::input_iterator_tag)
	{
		pointer cur(this->impl.start);
		for(;cur!=this->impl.finish&&first!=last;++first,++cur)
				*cur=*first;
		if(first==last)
			M_erase_at_end(cur);
		else
			insert(end(),first,last);		
	}
	template<typename Tp,typename Alloc> template<typename FwdIter> 
	void vector<Tp,Alloc>::
	M_assign_range_aux(FwdIter first,
					   FwdIter last,
					   numb::forward_iterator_tag)
	{
		const size_type len=numb::distance(first,last);
		if(len>capacity())//need more memory
		{
			//allocate and copy to buffer
			pointer tmp(M_allocate_and_copy(len,first,last));
			//destroy the old items
			std::_Destroy(this->impl.start,this->impl.finish,get_Tp_allocator());
			//release the old memory
			M_deallocate(this->impl.start,capacity());
			//update
			this->impl.start=tmp;
			this->impl.end_of_storage=this->impl.start+len;
		}
		else if(len>size())
		{
			//Note:Unlike operator =
			//Forward iterator doesn't support first+size()
			FwdIter mid=first;
			numb::advance(mid,size());	//instead of first+size()
			//For [begin,end),use std::copy
			std::copy(first,mid,this->impl.start);
			//For the rest
			std::__uninitialized_copy_a(mid,last,this->impl.finish,
											get_Tp_allocator());				
		}
		else
		{
			std::_Destroy(
					std::copy(first,
							  last,
							  begin()),
					end(),
					get_Tp_allocator());
		}
		this->impl.finish=this->impl.start+len;
	}	
	//public:insert(p,v)
	template<typename Tp,typename Alloc>
	typename vector<Tp,Alloc>::iterator
    vector<Tp,Alloc>::insert(iterator p, const value_type& v)
	{
		const size_type n=p-begin();
		if(this->impl.finish!=this->impl.end_of_storage
			&&p==end())		//insert to the back
		{
			this->impl.construct(this->impl.finish,v);
			++this->impl.finish;
		}
		else
		{
			M_insert_aux(p,v);
		}
		return iterator(this->impl.start + n);
	}
	//protected:M_insert_aux(p,v)
	template<typename _Tp, typename _Alloc>
    void
    vector<_Tp, _Alloc>::
    M_insert_aux(iterator p, const value_type& v)
	{
		if(this->impl.finish!=this->impl.end_of_storage)
		{
			//initialize the item this->impl.finish with last item
			this->impl.construct(this->impl.finish,*(this->impl.finish-1));
			++this->impl.finish;
			value_type v_copy=v;
			//copy [p,end-2) to [p+1,end-1)
			std::copy_backward(p.base(),this->impl.finish-2,this->impl.finish-1);	
			*p=v_copy;	//insert to p;
		}
		else
		{
			const size_type old_size=size();	//size()==capacity()
			const size_type new_size=old_size!=0?old_size<<1:1;
			pointer new_start=M_allocate(new_size);
			pointer new_finish=new_start;
			try
			{
				//copy [begin,p) to [new_start,new_p)
				new_finish=std::__uninitialized_copy_a(
							this->impl.start,p.base(),new_start,get_Tp_allocator());
				this->impl.construct(new_finish,v); //insert v to p
				++new_finish;
				//copy [p+1,end) to [new_p+1,new_finish)
				new_finish=std::__uninitialized_copy_a(
							p.base(),this->impl.finish,new_finish,get_Tp_allocator());
			}
			catch(...)
			{
				M_deallocate(new_start,new_size);
				throw;
			}
			//destory the old items
			std::_Destroy(this->impl.start,this->impl.finish,get_Tp_allocator());
			//release memory
			M_deallocate(this->impl.start,capacity());
			//update
			this->impl.start = new_start;
			this->impl.finish = new_finish;
			this->impl.end_of_storage = new_start + new_size;
		}
	}
	//to do: M_fill_insert
	
	//to do: M_erase_at_end
	//to do: vector equality comparisons.
 }//end of namespace
 #endif