#include "malloc_alloc.h"
#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <cstdio>
#include <cstdlib>
typedef numb::malloc_alloc<int> alloc;
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
	//int *p;
	std::cout<<"my allocator:\n";
	std::cout<<"list:\n";
	start=clock();
		for(int i=0;i!=testcase;i++)
		{
			IL.push_back(i);
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
	start=clock();
		for(int i=0;i!=testcase;i++)
		{
			L.push_back(i);
		}
	finish=clock();
	std::cout<<testcase<<"int in:"<<(double)(finish-start)<<"\n";
  
}
void p()
{
	std::cout<<"sorry,I've try my best!";
	exit(1);
}
int main ()
{
	std::cin>>testcase;
	//alloc::set_malloc_handler (p);
	//test1();
	//test2();
	//test3();
	//test4();
	return 0;
}