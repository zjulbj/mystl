#include "allocator.h"
#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <cstdio>
#include <cstdlib>
//typedef __gnu_cxx::malloc_allocator<int> alloc;
typedef numb::allocator<int> alloc;
clock_t start, finish;
int testcase;
void test1()
{
	std::vector<int,alloc> IV;
	std::cout<<"my allocator:\n";
	//for vector
	std::cout<<"vector:\n";
	start=clock();
		for(int i=0;i!=testcase;i++)
		{
			IV.push_back(i);
		}
	finish=clock();
	std::cout<<testcase<<" int in:"<<(double)(finish-start)<<"\n";
}
void test2()
{
	//for list
	std::list<int,alloc> IL;
	int *p;
	std::cout<<"my allocator:\n";
	std::cout<<"list:\n";
	start=clock();
		// for(int i=0;i!=testcase;i++)
		// {
			// IL.push_back(i);
		// }
		p=IL.get_allocator().allocate(testcase);
		for(int i=0;i!=testcase;i++)
		{
			p[i]=i;
		}
	finish=clock();
	std::cout<<testcase<<" int in:"<<(double)(finish-start)<<"\n";

}
void test3()
{
	std::vector<int> I;
	std::cout<<"std allocator:\n";
	std::cout<<"vector:\n";
	start=clock();
		for(int i=0;i!=testcase;i++)
		{
			I.push_back(i);
		}
	finish=clock();
	std::cout<<testcase<<" int in:"<<(double)(finish-start)<<"\n";
}
void test4()
{

	//for list
	std::list<int> L;
	std::cout<<"std allocator:\n";
	std::cout<<"list:\n";
	int *p;
	start=clock();
		// for(int i=0;i!=testcase;i++)
		// {
			// L.push_back(i);
		// }
		p=L.get_allocator().allocate(testcase);
		for(int i=0;i!=testcase;i++)
		{
			p[i]=i;
		}
	finish=clock();
	std::cout<<testcase<<"int in:"<<(double)(finish-start)<<"\n";
  
}
int main ()
{
	std::cin>>testcase;
	
	//test1();
	//test2();
	//test3();
	//test4();
 return 0;
}