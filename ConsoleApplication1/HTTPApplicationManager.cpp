#include "HTTPApplicationManager.h"
HTTPApplicationManager *HTTPApplicationManager::instance = 0;
HTTPResponse *HTTPApplicationManager::exec(HTTPRequest*r)
{
	if (r == 0)
		return 0;
	HTTPResponse* resp = new HTTPResponse(r);
	//if match a processor->remapAll->send
	//else if match a static file->remapfile->get from cached file/read from disk->send 
	//else ->send 404;
	handleResponse(*resp,*r, instance->files);
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
void HTTPApplicationManager::handleResponse(HTTPResponse &respones,HTTPRequest&request, StaticFileManager& files)
{
	respones<< files.getFile(files.remapFile(request.URL));
	respones.complish();
}