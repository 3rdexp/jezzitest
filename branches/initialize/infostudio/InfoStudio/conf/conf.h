#pragma once

#include <string>
#include <vector>
#include <list>


class Conf
{
public:

public:
	CString appPath;
	

	Conf();
	~Conf();

	void Load();
	void Save();
	
	BOOL	isRegister();


protected:	
};

bool  initConf();
void  closeConf();
Conf& getConf();


