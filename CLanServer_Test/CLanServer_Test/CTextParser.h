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
	int iFileSize; // ���� ������ 
	char* pFileBuffer; //  ���ϵ����͵� 
	int curPos;   // ���� ������ ��ġ 
	int MaxNameSpace;  /// ���ӽ����̽� ������ ����   ( ���ӽ����̽� ��ġ�� ���� ���� �˻����� �ʰ��Ϸ��� )
	int NS_Pos[MaxNameSpaceCount]; //���ӽ����̽� ���� ��ġ��
	int NS_MaxPos[MaxNameSpaceCount]; //���ӽ����̽� ������ ��ġ��
	char NS_Str[MaxNameSpaceCount][256]; //���ӽ����̽� �̸���   

	void CheckNameSpace();
	bool SkipNoneCommand();
	bool GetNextWord(char** chppBuffer, int* ipLength);
	bool GetStringWord(char** chppBuffer, int* ipLength);
};




#endif