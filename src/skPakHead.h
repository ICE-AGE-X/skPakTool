#ifndef __SKPAKHEAD_H__
#define __SKPAKHEAD_H__
#include<stdio.h>
#include<iostream>
#ifdef WIN32
#include<direct.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#endif
#include <string.h>
#include <fstream>
#include <list>

class skPak {
public:
	bool skPakUnpack(char *path);
	void mkdirByfileName(std::string fileName);
	bool skPakPack(char * outFileName,char * orgFile,char * txtFile);
private:
	struct DataHead
	{
		int length;
		int zero=0;
		int zero2=0;
		int zero3=0;
		int unknown=0;
		int unknown2=0;
		int unknown3=0;
		int unknown4=0;
	};
	struct DataTablePC
	{
		int offset=0;
		int zero=0;
	};

	struct DataTable3DS
	{
		int offset;
	};

	struct NameTablePC
	{
		int offset=0;
		int zero=0;
	};

	struct NameTable3DS
	{
		int offset;
	};

	struct HeadPC
	{
		int Zero=0;//0x00 00 00 00
		int fileNum=0;
		int dataTableOffset=0;
		int Zero2=0;//0x00 00 00 00
		int fileNameTableOffset=0;
		int zero3=0;
	};

	struct Head3DS
	{
		int Zero=0;//0x00 00 00 00
		int fileNum;
		int dataTableOffset;
		int fileNameTableOffset;
	};


};





#endif // !__SKPAKHEAD__

