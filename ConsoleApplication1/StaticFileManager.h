#pragma once

#include "tool"
#include "ArgumentHandle.h"
#include "CachedStaticFile.h"
#include <boost/function.hpp>
#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include "application.h"
#include "activity.h"
#include "loadedActivity.h"
class StaticFileManager
{
	//mainapp
	application* mainApplication = 0;
	//url,application object
	std::vector<loadedActivity*> activitys;
	boost::mutex activitysLocker;
	//ppp,file
	std::map<std::string, CachedStaticFile*>cachedFile;
	//url,ppp
	std::map<std::string, std::string> fileMap;
	std::vector<pair<std::string, std::string>> advanceFileMap;
	//ppp,path
	std::map<std::string, std::string> allFiles;
	//.html,text/html
	std::map<std::string, std::string> fileTypes;
public:
	StaticFileManager(ArgumentHandle*ah)
	{
		reloadXML(ah);
		reloadStaticFiles(ah); 
		reloadAppFiles(ah);
	}
	pair<runPointer,activity*> getActivity(HTTPRequest*r)
	{
		string url = r->URL;
		debug("getAPP", url);
		runPointer p = 0;
		activitysLocker.lock();
		for (auto i=activitys.begin();i!=activitys.end();++i)
		{
			if((*i)==0)
				continue;
			bool b = 0;
			try
			{
				if((*i)->act!=0)
				if((*i)->act->hasRouter())
				{
					p = (*i)->act->getFunction(url);
					if (p != 0)
					{
						activitysLocker.unlock();
						return pair<runPointer, activity*>(p, (*i)->act);
					}
				}
				debug("getAPP 1" , b);
			}
			catch (...)
			{
				debug("get exception when getting activity function:", r->URL);
				b = 0;
			}
		}
		activitysLocker.unlock();
		return pair<runPointer, activity*>(0,0);
	}
	void unloadALLDll()
	{
		for (auto i = activitys.begin(); i != activitys.end(); ++i)
		{
			(*i)->act->onUnload();
			(*i)->dll->unload();
			delete (*i)->dll;
		}
		mainApplication->onUnload();

	}
	application* loadMainApplicationDLL(std::string path)
	{
		boost::dll::shared_library *dll = new boost::dll::shared_library(path);
		if (!dll->is_loaded())
			return 0;
		if (!dll->has("createApplication"))
			return 0;
		debug("dll has loaded", 1);
		debug("dll has create", 1);
		debug("dll has location", dll->location());

		boost::function<application*()> creator = dll->get_alias<application*()>(                                        // path to library
			"createApplication");
		application* b = creator();
		return b;
	}
	activity* loadActivityDLL(boost::dll::shared_library *dll)
	{
		if (dll == 0)
			return 0;
		if (!dll->is_loaded())
			return 0;
		if (!dll->has("createActivity"))
			return 0;
		debug("dll has loaded", 1);
		debug("dll has create", 1);
		debug("dll has location", dll->location());

		boost::function<activity*()> creator = dll->get_alias<activity*()>(                                        // path to library
			"createActivity");
		activity* b = creator();
		return b;
	}
	void reloadXML(ArgumentHandle*ah)
	{
		boost::filesystem::path xmlPath = (boost::filesystem::current_path() / ah->xmlPath);
		fileMap.clear();
		advanceFileMap.clear();
		if (!boost::filesystem::exists(xmlPath))
		{	
		
			fileMap.clear();
			debug("Setting file not exists ", xmlPath.string());
		useDefault:
			ah->useDefault = 1;
			fileMap.insert(strstrPair("/", "/baidu.html"));
			fileMap.insert(strstrPair("/favicon.ico", "/baidu_files/baidu_jgylogo3.gif"));
			fileTypes.insert(strstrPair(".html", "text/html"));
			fileTypes.insert(strstrPair(".gif", "image/gif"));
			fileTypes.insert(strstrPair(".css", "text/css"));
			return;
		}
		if (boost::filesystem::is_directory(xmlPath))
		{
			goto useDefault;
		}
		debug("Found setting xml..." , xmlPath.c_str());
		boost::property_tree::ptree xml;
		boost::property_tree::read_xml(xmlPath.string(), xml);
		try {
			ah->staticPath = xml.get<std::string>("web-app.static-file-root");
		}
		catch (...)
		{
			debug("web-app.static-file-root","get error use default");
			goto useDefault;
		}
		try {
			ah->mainAppPath = xml.get<std::string>("web-app.application");
			mainApplication=loadMainApplicationDLL(ah->mainAppPath);
			mainApplication->onLoad();
		}
		catch (...)
		{
			debug("web-app.main-app", "get error use default");
			goto useDefault;
		}
		try {
			auto maps=xml.get_child("web-app.file-remapping");
			for (auto i = maps.begin(); i != maps.end(); ++i)
			{
				std::string url = i->second.get<string>("<xmlattr>.url");
				if (url.find('*') < url.size())
				{
					advanceFileMap.push_back(strstrPair(url, i->second.data()));
				}
				else 
				{
					fileMap.insert(strstrPair(url, i->second.data()));	
				}
				
			}
		}
		catch (...)
		{
			debug("web-app.file-remapping", "get error use default");
			goto useDefault;
		}
		try {
			
			auto maps = xml.get_child("web-app.activity");
			debug("try load activity", maps.size());
			for (auto i = maps.begin(); i != maps.end(); ++i)
			{
				std::string name = i->second.get<string>("<xmlattr>.name");
				boost::dll::shared_library *dll = new boost::dll::shared_library(i->second.data());
				activity *app = loadActivityDLL(dll);
				if (app)
				{	
					debug("Dll "<< i->second.data()<<" loaded", "error code:"<<app->onLoad(mainApplication));
					activitysLocker.lock();
					activitys.push_back(new loadedActivity(name,dll->location().string(),app,dll));
					activitysLocker.unlock();
				}
			}
		}
		catch (...)
		{
			debug("web-app.applications", "get error on loading dll");
			goto useDefault;
		}
	}
	const std::string& remapToType(std::string &ppp)
	{
		std::string p=ppp.substr(ppp.find('.'));
		auto i = fileTypes.find(p);
		if (i == fileTypes.end())
		{
			return "*/*";
		}
		return i->second;
	}
	const std::string &remapFile(std::string &URL)
	{
		auto i = fileMap.find(URL);
		if (i == fileMap.end())
		{
			for (auto i = advanceFileMap.begin(); i != advanceFileMap.end(); ++i)
			{
				if (Tool::compareStarUrl(i->first, URL))
					return i->second;
			}
			return URL;
		}
		return i->second;
	}
	size_t loadDirectory(boost::filesystem::path p, size_t counter=0)
	{
		boost::filesystem::recursive_directory_iterator begin(p);
		boost::filesystem::recursive_directory_iterator end;
		size_t fileCount = counter;
		for (; begin != end; ++begin)
		{
			
			if (boost::filesystem::is_directory(*begin))
			{
				loadDirectory((*begin).path(), fileCount);
				continue;
			}
			else
			{
				std::string strPath = begin->path().string();
				std::string ppp = strPath.substr(p.string().length());//遍历出来的文件名称
				
				for (int ii = 0; ii < ppp.size(); ++ii)
				{
					if (ppp.at(ii) == '\\')
						ppp[ii] = '/';
				}
				allFiles.insert(strstrPair(ppp,strPath));
// 				std::string fileType = remapToType(ppp);
// 				std::ifstream file;
// 				bool isCompress = 1;
// 				std::string *data = new std::string();
// 				file.open(strPath.c_str(), std::ios::in|std::ios::binary);
// 				Tool::getGZIP(data, file);
// 				file.close();
// 				file.clear();
// 
// 				debug("get file ppp", ppp);
// 				debug("get file path", strPath);
// 				debug("datalenth", data->length());
// 				CachedStaticFile *csf= new CachedStaticFile(data, ppp, data->size(), isCompress);
// 				csf->setType(fileType);
// 				cachedFile.insert(FilePair(ppp, csf));
// 				fileCount += 1;
			}
		}
	}

	size_t reloadAppFiles(ArgumentHandle*ah)
	{
		//load dlls here
		return 0;
	}

	size_t reloadStaticFiles(ArgumentHandle*ah)
	{
		boost::filesystem::path sourcePath = (boost::filesystem::current_path() / ah->staticPath);
		allFiles.clear();
		if (!boost::filesystem::exists(sourcePath))
		{
			printl("Can not find static path");
			boost::filesystem::create_directory(sourcePath);
			return 0;
		}	
		return loadDirectory(sourcePath, 0);
	}
	CachedStaticFile* cacheFile(std::string &path)
	{
		if (!boost::filesystem::exists(path))
		{
			return 0;
		}
		debug("=====start", "======");
		std::string ppp = path.substr(boost::filesystem::current_path().string().length());//遍历出来的文件名称
		allFiles.insert(strstrPair(ppp, path));
		for (int ii = 0; ii < ppp.size(); ++ii)
		{
			if (ppp.at(ii) == '\\')
				ppp[ii] = '/';
		}
		std::string fileType = remapToType(ppp);
		std::ifstream file;
		bool isCompress = 1;
		std::string *data = new std::string();
		file.open(path.c_str(), std::ios::in | std::ios::binary);
		Tool::getGZIP(data, file);
		file.close();
		file.clear();

		debug("get file ppp", ppp);
		debug("get file path", path);
		debug("datalenth", data->length());
		CachedStaticFile *csf = new CachedStaticFile(data, ppp, data->size(), isCompress);
		csf->setType(fileType);
		cachedFile.insert(FilePair(ppp, csf));
		return csf;
	}
	~StaticFileManager() 
	{
		for (auto i= cachedFile.begin();i!= cachedFile.end();++i)
		{
			delete i->second;
		}
	}
	CachedStaticFile* getFile(const std::string& ppp)//after remap file
	{
		auto i = cachedFile.find(ppp);//try load from cached file
		if (i== cachedFile.end())
		{
			auto j = allFiles.find(ppp);//try load from all file
			if (j == allFiles.end())//no such file defined in all file
				return 0;
			CachedStaticFile* f = cacheFile(j->second);// load from all file
			if (f == 0)//load from all file failed
				return 0;
			cachedFile.insert(FilePair(j->first,f));
			return f;
		}
		return i->second;
	}
	
};

