MySTL--A simple version of STL
=====

For details: [http://zjulbj.github.io/categories.html#stl-ref](http://zjulbj.github.io/categories.html#stl-ref "http://zjulbj.github.io/categories.html#stl-ref")

##Goals

- Simple version of STL.
    
	- Referring from SGI STL and GNU C++ STL
	
	- Support main functions of ISO STL

- A simple library,not a framework

- Just for learning,not for using

- Attend to add some c++11 characteristic

##Interface design

- In strict accordance with ISO C++

- Reffer to GNU C++ STL

##Categories

- [mystl](../tree/master/mystl "../tree/master/mystl")
	- [memory](mystl/memory "mystl/memory")		(使用memory需要包含文件)
	- [iterator](mystl/iterator "mystl/iterator")	(使用iterator的实现及需要包含文件)
	- [vector](mystl/vector "mystl/vector")		(使用vector需要包含文件)
	- [stl_impl](../tree/master/mystl/stl_impl "../tree/master/mystl/stl_impl")	(各种实现文件)
		- [allocator.h](mystl/stl_impl/allocator.h "mystl/stl_impl/allocator.h")
		- [malloc_alloc.h](mystl/stl_impl/malloc_alloc.h "/mystl/stl_impl/malloc_alloc.h")
		- [pool_alloc.h]( /mystl/stl_impl/pool_alloc.h "/mystl/stl_impl/pool_alloc.h")
		- [iterator_base.h](mystl/stl_impl/iterator_base.h "mystl/stl_impl/iterator_base.h")
		- [stl_iterator.h](mystl/stl_impl/stl_iterator.h "mystl/stl_impl/stl_iterator.h")
		- [sti_vector.h](/mystl/stl_impl/sti_vector.h "mystl/stl_impl/sti_vector.h")
	

##项目简介
主要参考SGI,gnu g++版的STL,重构一个自己的STL。
##项目信息
详细的说明可以浏览我的博客:[http://zjulbj.github.io](http://zjulbj.github.io "http://zjulbj.github.com")

如果有什么意见或者建议：

欢迎联系我

email:`numbcode[at]gmail[dot]com`

或者直接到[我的博客](http://zjulbj.github.io "http://zjulbj.github.io")留言
