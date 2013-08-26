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
		typedef Vec_base<Tp,Alloc>			Base;
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
	
	//	Construct/copy/destory	
		/*	default ctor
		 *	creates no element;
		 */
		vector():Base(){}
		/*ctor with a @param a An allocator
		 *creates no element
		 */
		explicit vector(const allocator_type& a):Base(a){}
				
		/**
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
			//to do:M_fill_initialize
			M_fill_initialize(n,value); 
		}
		/**
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
		
		vector&
		operator=(const vector& __x);
		//to do:assign
		
		//vector::get_allocator
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
		//to do:shrink_to_fit
		
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
		//to do:data()
		
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
				;
			}
		}
		//Remove last element
		void pop_back()
		{
			--this->impl.finish;
			this->impl.destroy(this->impl.finish);
		}
		//to do:insert
		//to do:erase
		//to do:swap
		//to do:clear
		//to do:resize 
	protected:
		//internal functions
		//call by vector(n,v,a) ctor
		void
		M_fill_initialize(size_type n, const value_type& value)
		{
			std::__uninitialized_fill_n_a(this->impl.start, n,value, 
				      get_Tp_allocator());
			this->impl.finish = this->impl.end_of_storage;
		}
	};//end of vector
	
	//to do:Vector equality comparisons.
 }//end of namespace
 #endif