#include "HTTPApplicationManager.h"
#include "activity.h"
HTTPApplicationManager *HTTPApplicationManager::instance = 0;
HTTPResponse *HTTPApplicationManager::exec(HTTPRequest*r)
{
	if (r == 0)
		return 0;
	HTTPResponse* resp = new HTTPResponse(r);
	//if match a processor->remapAll->send
	//else if match a static file->remapfile->get from cached file/read from disk->send 
	//else ->send 404;
	handleResponse(resp,r, instance->files);
	delete r;
	return resp;
	debug("HTTPApplicationManager", "exec");
}
HTTPApplicationManager*HTTPApplicationManager::getInstance(ArgumentHandle* ah)
{
	if (instance == 0)
		instance = new HTTPApplicationManager(ah);
	return instance;
}
void HTTPApplicationManager::handleResponse(HTTPResponse *respones,HTTPRequest*request, StaticFileManager& files)
{
	string url = request->URL;
	CachedStaticFile* f=files.getFile(files.remapFile(url));
	debug("HTTPApplicationManager", url);
	if (f)
	{
		debug("find static file", url);
		*respones << f;
		respones->complish();
		return;
	}
	pair<runPointer, activity*> act = files.getActivity(request);
	if (act.second!=0&&act.first!=0)
	{
		try
		{
			debug("find app", url);
			act.first(request, respones,act.second);
			respones->complish();
			return;
		}
		catch (...)
		{
			debug("get exception when running activity function:", url);
		}	
	}
	respones->complish();
}