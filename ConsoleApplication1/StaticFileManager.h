#pragma once
#include "ArgumentHandle.h"
#include "CachedStaticFile.h"
class StaticFileManager
{
	std::map<std::string, std::string> files;
public:
	StaticFileManager(ArgumentHandle*ah)
	{
		flushFile();
	}
	void reloadXML()
	{

	}
	size_t loadDirectory(boost::filesystem::path p, size_t counter)
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
				debug("find file", ppp);
				std::ifstream file;
				std::string *data = new std::string();
				file.open(strPath.c_str(), std::ios::in);
				Tool::getGZIP(data, file);
				file.close();
				file.clear();
				debug("datalenth", data->length());
				fileCount += 1;
			}
		}
	}
	size_t flushFile()
	{
		boost::filesystem::path sourcePath = (boost::filesystem::current_path() / "source");
		if (!boost::filesystem::exists(sourcePath))
		{
			printl("!boost::filesystem::exists(sourcePath)");
			boost::filesystem::create_directory(sourcePath);
			return 0;
		}	
		return loadDirectory(sourcePath, 0);
	}
	
	~StaticFileManager();
	std::vector<CachedStaticFile*>staticFiles;
};

