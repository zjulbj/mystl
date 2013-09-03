/*This is a internal header file.
 *It's a implement of vector
 */
#ifndef _MSTL_VECTOR_CPP
#define _MSTL_VECTOR_CPP 1
namespace numb
{	
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
				std::_Destroy(new_start,new_finish,get_Tp_allocator());
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
	//protected:M_insert_range_aux
	template<typename Tp,typename Alloc> 
		template<typename InputIter>
    void vector<Tp,Alloc>::
	M_insert_range_aux(iterator p,
					   InputIter first,InputIter last,
					   numb::input_iterator_tag)
	{
		for(;first!=last;++first)
		{
			insert(p,*first);
			++p;
		}
	}
	//Inserts elements from range [first, last) before 
	//the element pointed to by pos. The behavior is 
	//undefined if first and last are iterators into *this.
	template<typename Tp,typename Alloc> 		
		template<typename FwdIter>
	void vector<Tp,Alloc>::
	M_insert_range_aux(iterator p,
					   FwdIter first,FwdIter last,
					   numb::forward_iterator_tag)
	{
		if(first!=last)
		{
			const size_type n=numb::distance(first,last);
			if(size_type(this->impl.end_of_storage
						 -this->impl.finish)>=n)
			{//current memory is enough
				const size_type elem_after=end()-p;
				pointer old_finish(this->impl.finish);
				if(elem_after>n)
				{
					//uninit_copy [of-n,of) to [of,of+n)
					std::__uninitialized_copy_a(this->impl.finish-n,
									this->impl.finish,
									this->impl.finish,
									get_Tp_allocator());
					this->impl.finish+=n;
					//copy [p,of-n) to [of-n,of)
					std::copy_backward(p.base(),old_finish-n,old_finish);
					//insert [first,last) to [p,of-n)
					std::copy(first,last,p);
				}
				else
				{
					FwdIter mid=first;
					//divide [first,last) to [first,first+e_a)
					//and [first+e_a,last)
					numb::advance(mid,elem_after);
					//uninit_copy [mid last) to [of,of+n-e_a)
					std::__uninitialized_copy_a(mid,last,
												old_finish,
												get_Tp_allocator());
					this->impl.finish+=(n-elem_after);		
					//uninit_copy [p,of) to [of+n-e_a,of+n)
					std::__uninitialized_copy_a(p.base(),old_finish,
											   this->impl.finish,
											   get_Tp_allocator());
					this->impl.finish+=elem_after;
					//copy [first,mid) to [p,of)
					std::copy(first,mid,p.base());
				}
			}
			else
			{
				const size_type old_size=size();
				const size_type len=old_size+std::max(old_size,n);
				pointer new_start=M_allocate(len);
				pointer new_finish=new_start;
				try
				{
				  //copy[begin,p) to [nb,p)
				  new_finish=std::__uninitialized_copy_a(
								this->impl.start, p.base(),new_start,
								get_Tp_allocator());
				 //copy[first,last) to [p,p+(first-last))
				  new_finish=std::__uninitialized_copy_a(
								first,last,new_finish,
								get_Tp_allocator());
				 //copy[p,end) to [p+(first-last),ne)
                  new_finish
					=std::__uninitialized_copy_a(
								p.base, this->impl.finish, new_finish,
								get_Tp_allocator());
				}
				catch(...)
				{
					std::_Destroy(new_start,new_finish,get_Tp_allocator());
					M_deallocate(new_start,len);
					throw;
				}
				//destory the old items
				std::_Destroy(this->impl.start,this->impl.finish,
						  get_Tp_allocator());
				//release memory
				M_deallocate(this->impl.start,capacity());
				//update
				this->impl.start = new_start;
				this->impl.finish = new_finish;
				this->impl.end_of_storage = new_start + len;
			}
		}
	}
	//protected:M_fill_insert
	template<typename Tp,typename Alloc>
	void vector<Tp,Alloc>::
	M_fill_insert(iterator p,
				  size_type n, 
				  const value_type& v)
	{
		if(n!=0)
		{
			if(size_type(this->impl.end_of_storage
						-this->impl.finish)>=n)
			{
				value_type v_copy=v;
				const size_type elem_after=end()-p;
				pointer old_finish(this->impl.finish);
				if(elem_after>n)
				{
					std::__uninitialized_copy_a(
							this->impl.finish-n,
							this->impl.finish,
							this->impl.finish,
							get_Tp_allocator());
					this->impl.finish+=n;
					std::copy_backward(p.base(),
							old_finish-n,old_finish);
					std::fill(p.base(),p.base()+n,v_copy);
				}
				else
				{
				std::__uninitialized_fill_n_a(this->impl.finish,
								n - elem_after,
								v_copy,
								get_Tp_allocator());
				this->impl.finish += n - elem_after;
				std::__uninitialized_copy_a(p.base(), old_finish,
					      this->impl.finish,
					      get_Tp_allocator());
				this->impl.finish += elem_after;
				std::fill(p.base(),old_finish, v_copy);
				}
			}
			else//need more memory
			{
				const size_type old_size=size();        
				const size_type len=old_size + std::max(old_size, n);
				pointer new_start=M_allocate(len);
				pointer new_finish =new_start;
				try
				{
					new_finish=std::__uninitialized_copy_a(
								this->impl.start, p.base(),new_start,
								get_Tp_allocator());
					std::__uninitialized_fill_n_a(
								new_finish, n,v,
								get_Tp_allocator());
					new_finish+=n;
					new_finish
						=std::__uninitialized_copy_a(
								p.base(),this->impl.finish,new_finish,
								get_Tp_allocator());
				}
				catch(...)
				{
					std::_Destroy(new_start,new_finish,get_Tp_allocator());
					M_deallocate(new_start,len);
					throw;
				}
				//destory the old items
				std::_Destroy(this->impl.start,this->impl.finish,
						  get_Tp_allocator());
				//release memory
				M_deallocate(this->impl.start,capacity());
				//update
				this->impl.start = new_start;
				this->impl.finish = new_finish;
				this->impl.end_of_storage = new_start + len;
			}
		}
	}
}//end of namespace numb
#endif