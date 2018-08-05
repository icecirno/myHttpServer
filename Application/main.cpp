#include "..\ConsoleApplication1\application.h"
#include "..\ConsoleApplication1\activity.h"
#include "..\ConsoleApplication1\tool"

class myApplication : public application {
	
	myApplication() : counter(10) {}

public:
	int counter;
	int onLoad()
	{
		counter += 1;
		return counter;
	}
	void onUnload()
	{
		delete this;
	}
	// Factory method
	static myApplication* create() {
		return new myApplication();
	}
	~myApplication()
	{
		debug("myApplication", "deleted");
	}
};
class myActivity : public activity {

	int counter;
	myApplication* app=0;
	myActivity() : counter(1) {}

public:
	
	int onLoad(application*app)
	{
		this->app = (myApplication*)app;
		return ((myApplication*)app)->counter;
	}

	void onUnload()
	{
		delete this;
	}
	bool runActivity(HTTPRequest*r, HTTPResponse* result)
	{
		app->counter++;
		result->needCompress = 0;
		result->addHead("hello", "http");
		if(r->hasParm)
		for (auto i=r->parms->begin();i!=r->parms->end();++i)
		{
			result->addBody(i->first + ":" + i->second + ","+std::to_string(app->counter)+"\n");
		}
		return 1;
	}
	// Factory method
	static myActivity* create() {
		return new myActivity();
	}
	~myActivity()
	{
		debug("myActivity", "deleted");
	}
};
BOOST_DLL_ALIAS(
	myActivity::create, // <-- this function is exported with...
	createActivity                              // <-- ...this alias name
)
BOOST_DLL_ALIAS(
	myApplication::create, // <-- this function is exported with...
	createApplication                              // <-- ...this alias name
)
