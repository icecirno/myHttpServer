#pragma once
#include "tool"
template<class C>
class Queue
{
public:
	list<C*> data;
	boost::mutex locker;

	Queue()
	{};
	~Queue()
	{};
	size_t size()
	{
		size_t re;
		locker.lock();
		re = data.size();
		locker.unlock();
		return re;
	}
	Queue& operator<<( C* c)
	{
		if (c == 0)
			return *this;
		locker.lock();
		data.push_back(c);
		locker.unlock();
	}
	C* get()
	{
		C* c=0;
		locker.lock();
		if (data.size())
		{
			c = *data.begin();
			data.pop_front();
			debug("queue", "gotone");
		}
		locker.unlock();
		return c;
	}
};
