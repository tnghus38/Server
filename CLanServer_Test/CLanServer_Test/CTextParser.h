#pragma once
#ifndef __CTEXTPARSER__
#define __CTEXTPARSER__


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define MaxNameSpaceCount 20
class CTextParser
{
public:
	CTextParser();
	~CTextParser();
	bool LoadText(const char* TextName);
	bool GetValue_Int(const char* szName, int* ipValue ,const char* NameSpace ="");
	bool GetValue_Double(const char* szName, double* ipValue, const char* NameSpace = "");
	bool GetValue_String(const char* szName, char* ipValue, const char* NameSpace = "");

	bool GetValue_WString(const char* szName, wchar_t* ipValue, const char* NameSpace = "");
	
private: 
	int iFileSize; // 파일 사이즈 
	char* pFileBuffer; //  파일데이터들 
	int curPos;   // 현재 포인터 위치 
	int MaxNameSpace;  /// 네임스페이스 마지막 범위   ( 네임스페이스 위치를 벗어 나면 검색하지 않게하려고 )
	int NS_Pos[MaxNameSpaceCount]; //네임스페이스 시작 위치들
	int NS_MaxPos[MaxNameSpaceCount]; //네임스페이스 끝나는 위치들
	char NS_Str[MaxNameSpaceCount][256]; //네임스페이스 이름들   

	void CheckNameSpace();
	bool SkipNoneCommand();
	bool GetNextWord(char** chppBuffer, int* ipLength);
	bool GetStringWord(char** chppBuffer, int* ipLength);
};




#endif