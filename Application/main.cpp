#include "..\ConsoleApplication1\application.h"
#include "..\ConsoleApplication1\activity.h"
#include "..\ConsoleApplication1\tool"
#include "jdbc\mysql_driver.h"
#include "jdbc\mysql_connection.h"
#include "jdbc\mysql_error.h"
#include "jdbc\cppconn\statement.h"
class myApplication : public application {
	
	myApplication() : counter(10) {}
	sql::Driver *d = 0;
	sql::Connection*c = 0;
	
public:
	sql::Statement *s = 0;
	int counter;
	int onLoad()
	{
		try
		{
			d = get_driver_instance();
			c = d->connect("tcp://127.0.0.1:3306", "244143443", "qqgk1105");
			c->setSchema("lvke");
			s = c->createStatement();
			printl("isValid:%d", c->isValid());
			printl("isClosed:%d\n", c->isClosed());
			printl("s isValid:%d", s==0?0:1);
		}
		catch (sql::SQLException& e)
		{
			printf("Get sql connect error");
		}
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
	addRouter(router("/hello", &hello), router("/hello2", &hello2));
	int onLoad(application*app)
	{
		this->app = (myApplication*)app;
		return ((myApplication*)app)->counter;
	}

	void onUnload()
	{
		delete this;
	}
	static bool hello(HTTPRequest*r, HTTPResponse* result, activity*myself)
	{
		((myActivity*)myself)->app->counter--;
		result->needCompress = 0;
		result->addHead("hello", "http");
		if (r->hasParm)
			for (auto i = r->parms->begin(); i != r->parms->end(); ++i)
			{
				result->addBody(i->first + ":" + i->second + "," + std::to_string(((myActivity*)myself)->app->counter) + "\n");
			}
		return 1;
	}
	static bool hello2(HTTPRequest*r, HTTPResponse* result,activity*myself)
	{
		((myActivity*)myself)->app->counter++;
		sql::Statement *s = ((myActivity*)myself)->app->s;
		sql::ResultSet *rset = 0;
		result->needCompress = 0;
		result->addHead("hello2", "http");
		try
		{
			if (r->hasParm)
				rset = s->executeQuery("SELECT * FROM lvke.guid;");
			while (rset->next())
			{
				cout << rset->getString(1);
				cout << rset->getString(2);
				cout << rset->getString(3);
				result->addBody(rset->getString(1));
				result->addBody(rset->getString(2));
				result->addBody(rset->getString(3));
			}
		}
		catch (sql::SQLException& e)
		{
			result->addBody("Get sql connect error");
			if(rset)
				delete rset;
			return 0;
		}
		if (rset)
			delete rset;
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
