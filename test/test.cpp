#include<iostream>
#include "../mystl/vector"
using numb::vector;
using std::cout;
using std::endl;

void display(vector<char>& v)
{
	for(auto x:v)
		cout<<x;
	cout<<endl;
}
class A
{
	public:
	A(){cout<<"A()\n";}
	A(const A&)
	{
		cout<<"A()\n";
	}
	~A(){cout<<"~A()\n";}
};
int main()
{
	vector<char> ch;
	char a[]="helloworld";
	ch.assign(a,a+10);
	ch.insert(ch.begin()+5,' ');
	ch.push_back('!');
	display(ch);
	cout<<ch.size()<<" "<<ch.capacity()<<endl;
	
	vector<char> ch2(ch.begin(),ch.end());
	display(ch2);
	cout<<"ch[5]: "<<ch.at(6)<<" "<<ch[6]<<endl;
	//cout<<ch.at(12)<<" "<<ch[12]<<endl;
	cout<<"front: "<<ch.front()<<" back:"<<ch.back()<<endl;
	cout<<"empty: "<<ch.empty()<<endl;
	ch2.resize(21,'!');
	display(ch2);
	
	cout<<ch2.size()<<" "<<ch2.capacity()<<endl;
	{
		vector<A> va(2,A());
	}
	return 0;
}