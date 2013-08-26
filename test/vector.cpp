#include<iostream>
#include "../mystl/vector"
using std::cout;
using std::endl;
template<typename T>
void out(T t)
{
	cout<<t<<" ";
}
int main()
{
	typedef numb::vector<int> vector;
	typedef vector::iterator	iter;
	vector V(10);
	cout<<V.size()<<endl;
	for(int i=0;i!=10;i++)
	{
		V[i]=i;
	}
	iter begin=V.begin();
	iter end=V.end();
	for(iter j=begin;j!=end;++j)
	{
		out(*j);
	}
	cout<<endl;
	for(int i=0;i!=10;i++)
	{
		V.pop_back();
	}
	for(int i=0;i!=10;i++)
	{
		V.push_back(10-i);
	}
	for(iter j=begin;j!=end;++j)
	{
		out(*j);
	}
}