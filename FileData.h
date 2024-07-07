#include <iostream>
#include <string>
#include "PathList.h"

using namespace std;

class FileData
{
public:
	long long key;
	string filePath;
	PathList pathList;

	FileData()
	{
		filePath = "";
		pathList.insert(filePath);
		key = -1;
	}

	FileData(string p)
	{
		filePath = p;
		pathList.insert(p);
		key = -1;
	}
};