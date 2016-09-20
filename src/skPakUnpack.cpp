#include "skPakHead.h"


using namespace std;

void  skPak::mkdirByfileName(string fileName)
{
	const char * tmpN;
	tmpN = fileName.c_str();
	int countFloderNum = 0;
	int countPos = 0;
	while (*tmpN != '\0')
	{
		if (*tmpN == '/')
		{
			countFloderNum++;
			//cout<<countFloderNum<<" "<<countPos<<endl;
			char * dirName = new char[countPos + 1];
			fileName.copy(dirName, countPos, 0);
			dirName[countPos] = '\0';
			mkdir(dirName);
			delete[] dirName;
		}
		countPos++;
		tmpN++;
	}
}

//3ds japan or  pc
bool skPak::skPakUnpack(char * path)
{
	FILE * pakFile;
	pakFile = fopen(path, "rb");

	if (!pakFile)
	{
		cout << "open error !" << endl;
		return false;
	}
	HeadPC pcPakHead;

	long int fileLength;
	fseek(pakFile, 0, SEEK_END);

	fileLength = ftell(pakFile);

	//cout << fileLength << endl;
	fseek(pakFile, 0, SEEK_SET);

	fread(&pcPakHead, sizeof(pcPakHead), 1, pakFile);

	//cout << pcPakHead.fileNum << " " << pcPakHead.dataTableOffset << " " << pcPakHead.fileNameTableOffset << endl;

	DataTablePC  dtArray[pcPakHead.fileNum + 1];
	NameTablePC  ntArray[pcPakHead.fileNum + 1];

	fread(&dtArray, sizeof(dtArray), 1, pakFile);

	fseek(pakFile, pcPakHead.fileNameTableOffset, SEEK_SET);
	fread(&ntArray, sizeof(ntArray), 1, pakFile);

	//cout<<dtArray[1].offset<<" "<<dtArray[2].offset<<endl;
	//cout<<ntArray[1].offset<<" "<<ntArray[2].offset<<endl;
	dtArray[pcPakHead.fileNum].offset = fileLength;
	ntArray[pcPakHead.fileNum].offset = fileLength;
	string fileName[pcPakHead.fileNum];

	FILE * outFile;
	FILE * txtFile=fopen("filename.txt","wb");
	for (int i = 0; i<pcPakHead.fileNum; i++)
	{
		fseek(pakFile, ntArray[i].offset, SEEK_SET);
		char * x;
		x = new char[ntArray[i + 1].offset - ntArray[i].offset];
		fread(x, ntArray[i + 1].offset - ntArray[i].offset, 1, pakFile);
		fileName[i] = x;
		fwrite((fileName[i]+"\n").c_str(),strlen((fileName[i]+"\n").c_str()),1,txtFile);
		mkdirByfileName(x);
		fseek(pakFile, dtArray[i].offset, SEEK_SET);
		DataHead dh;
		fread(&dh, sizeof(DataHead), 1, pakFile);
		char * outChr;
		outChr = new char[dh.length];
		fread(outChr, dh.length, 1, pakFile);
		outFile = fopen(fileName[i].c_str(), "wb");
		fwrite(outChr, dh.length, 1, outFile);
		fclose(outFile);
		delete[] outChr;
	}

	fclose(outFile);
	fclose(txtFile);
	fclose(pakFile);
	return true;
}

bool skPak::skPakPack(char * outFileName,char * orgFilePath,char * txtFilePath)
{
	FILE * outFile=fopen(outFileName,"wb");
	FILE * orgFile=fopen(orgFilePath,"rb");

	//��ȡԭ�ļ� �Ĳ�����Ϣ  ��������д��
	HeadPC orgHead;
	fread(&orgHead,sizeof(orgHead),1,orgFile);
	DataTablePC  odtArray[orgHead.fileNum];
	fread(&odtArray,sizeof(odtArray),1,orgFile);
	DataHead odhArray[orgHead.fileNum];
	for (int i = 0; i < orgHead.fileNum; ++i)
	{
		fseek(orgFile,odtArray[i].offset,SEEK_SET);
		fread(&odhArray[i],sizeof(DataHead),1,orgFile);
	}
	fclose(orgFile);


	HeadPC pakHead;
	//д�ļ�ͷ ���� ��Ҫ��д
	fwrite(&pakHead,sizeof(pakHead),1,outFile);
	pakHead.dataTableOffset= ftell(outFile);

	//���ı��ļ� ����ļ���
	filebuf fb;
	fb.open(txtFilePath,ios::in);
	string tst;
	istream fin(&fb);

	list<string> nameList;
	list<string> ::iterator nameIter;
	//�������ļ�
	pakHead.fileNum=0;
	while(getline(fin,tst))
	{
		nameList.push_back(tst);
	}
	fb.close();
	//����ļ�����
	pakHead.fileNum=nameList.size();
	cout<<"file count:"<<pakHead.fileNum<<endl;
	//��ȡ�ļ���ʼƫ��
	pakHead.dataTableOffset=ftell(outFile);
	//д������
	DataTablePC dtArray[pakHead.fileNum];
	fwrite(&dtArray,sizeof(dtArray),1,outFile);

	FILE * subFile;
	int index=0;
	char fix=0;
	//д�����ļ�
	for(nameIter=nameList.begin();nameIter!=nameList.end();++nameIter)
	{
		string a =*nameIter;
		subFile= fopen(a.c_str(),"rb");
		fseek(subFile,0,SEEK_END);
		DataHead dh;
		dh.length=ftell(subFile);
		dh.unknown=odhArray[index].unknown;
		dh.unknown2=odhArray[index].unknown2;
		dh.unknown3=odhArray[index].unknown3;
		dh.unknown4=odhArray[index].unknown4;
		dtArray[index].offset=ftell(outFile);
		//д�ļ�ͷ
		fwrite(&dh,sizeof(dh),1,outFile);
		char * buffer=new char [dh.length];
		fseek(subFile,0,SEEK_SET);
		fread(buffer,dh.length,1,subFile);
		fwrite(buffer,dh.length,1,outFile);
		//8�ֽڶ���
		if(dh.length%8>0)
			for(int i=0;i<(8-dh.length%8);i++)
				fwrite(&fix,sizeof(fix),1,outFile);
		delete [] buffer;
		fclose(subFile);
		index++;
	}

	//��ȡ�ļ�����ʼ ƫ��
	pakHead.fileNameTableOffset=ftell(outFile);
	//д�ļ�����
	NameTablePC ntArray [pakHead.fileNum];
	fwrite(&ntArray,sizeof(ntArray),1,outFile);
	index=0;
	for(nameIter=nameList.begin();nameIter!=nameList.end();++nameIter)
	{
		string a =*nameIter;
		ntArray[index].offset= ftell(outFile);
		fwrite(a.c_str(),strlen(a.c_str()),1,outFile);
		//8�ֽڶ���

		for(int i=0;i<(8-strlen(a.c_str())%8);i++)
			fwrite(&fix,sizeof(fix),1,outFile);
		index++;
	}
	//��д�ļ�ͷ
	fseek(outFile,0,SEEK_SET);
	fwrite(&pakHead,sizeof(pakHead),1,outFile);
	//��д�ļ�ƫ�Ʊ�
	fseek(outFile,pakHead.dataTableOffset,SEEK_SET);
	fwrite(&dtArray,sizeof(dtArray),1,outFile);
	//��д�ļ���ƫ�Ʊ�
	fseek(outFile,pakHead.fileNameTableOffset,SEEK_SET);
	fwrite(&ntArray,sizeof(ntArray),1,outFile);

	//fclose(orgFile);
	fclose(outFile);

	return true;
}
