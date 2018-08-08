#pragma once
#include "tool"
#include <boost/dll.hpp>
#include "activity.h"
class loadedActivity
{
public:
	loadedActivity(std::string name, std::string path, activity * act, boost::dll::shared_library* dll)
	{
		this->name=name;
		this->path= path;
		this->dll=dll;
		this->act=act;
	}
	std::string name;
	std::string path;
	boost::dll::shared_library* dll=0;
	activity *act=0;
};