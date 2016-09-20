//============================================================================
// Name        : skPakTool.cpp
// Author      : ICE AGE
// Version     :
// Copyright   : PRIVATE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "skPakHead.h"
using namespace std;

void showHelp()
{
	cout<<"skpakTool u [pak file]"<<endl;
	cout<<"skpakTool p [out name] [org file] [txt file]"<<endl;
	cout<<"by ICE AGE"<<endl;
}

int main(int arg,char **argv) {

	if(arg!=3&&arg!=5)
	{
		showHelp();
		return 1;
	}


	char flag=**(argv+1);
	skPak * sp=new skPak();
	switch (flag) {
		case 'u':
		{
			if(sp->skPakUnpack(*(argv+2)))
				cout<<"success"<<endl;
			else
				cout<<"error"<<endl;
		}
			break;
		case 'p':
		{
			if(sp->skPakPack(*(argv+2),*(argv+3),*(argv+4)))
				cout<<"success"<<endl;
		}
			break;
		default:
			showHelp();
			break;
	}
	return 0;
}


