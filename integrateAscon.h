#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/md5.h>

class integrateAscon
{
	integrateAscon(std::string path);

public:
	std::string Path;
	void synchroniseWithChildNodes();

private:
	bool isFilesAscon();
};

