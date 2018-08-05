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
	CachedStaticFile* f=files.getFile(files.remapFile(request->URL));
	debug("HTTPApplicationManager", request->URL);
	if (f)
	{
		debug("find static file", request->URL);
		*respones << f;
		respones->complish();
		return;
	}
	activity *act = files.getActivity(request->URL);
	if (act)
	{
		act->runActivity(request, respones);
		respones->complish();
		return;
	}
	*respones << f;
	respones->complish();
}