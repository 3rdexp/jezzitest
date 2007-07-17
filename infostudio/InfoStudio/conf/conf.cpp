#include "stdafx.h"
#include "conf.h"
#include <fstream>
#include <sstream>


#define CONFFILENAME _T("conf.dat")
#include "../util.h"

Conf::Conf()
{
	appPath = GetExePath();
	
	Load();
}

Conf::~Conf()
{


}

BOOL Conf::isRegister()
{
	return TRUE;
	
}


void Conf::Load()
{
	
}



//////////////////////////////////////////////
static Conf* g_conf = NULL;

bool initConf()
{
	g_conf = new Conf();
	return true;
}

void closeConf()
{
	delete g_conf;
	g_conf = NULL;
}

Conf& getConf()
{
	return *g_conf;
}
