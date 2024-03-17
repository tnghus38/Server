#include "CTextParser.h"
#include <Windows.h>
CTextParser::CTextParser()
{
}

CTextParser::~CTextParser()
{
}
bool CTextParser::LoadText(const char* TextName)
{
	FILE* pFile; //�д� ����

	pFile = fopen(TextName, "rb");

	if (pFile == NULL)
	{
		printf("������(%s) �� �� �����ϴ�",TextName);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	iFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pFileBuffer = (char*)malloc(iFileSize);
	fseek(pFile, 0, SEEK_SET);
	//	rewind(pFile);
	fread(pFileBuffer, iFileSize, 1, pFile);
	fclose(pFile);
	CheckNameSpace();
	return true;

}

void CTextParser::CheckNameSpace()
{
	curPos = 0; //ó������ �˻� 
	MaxNameSpace = iFileSize;//������ �˻�
	int NsCount = 0; //���� ���� ã�� ���ӽ����̽� ����
	char chWord[256];

	char* chpBuffer = &pFileBuffer[curPos];
	int len;
	for (int i = 0; i < MaxNameSpaceCount; i++)
	{
		NS_MaxPos[i] = iFileSize;  // Ȥ�� ���� 
		NS_Pos[i] = 0;
	}
	while (GetNextWord(&chpBuffer, &len))
	{

		if (chpBuffer[0]==':')
		{
			memset(chWord, 0, 256);
			memcpy(chWord, chpBuffer+1, len-1);

			NS_Pos[NsCount] = curPos;//������ġ ���� 
			strcpy(NS_Str[NsCount], chWord);// ���� ���ӽ����̽� ���� 

			while (GetNextWord(&chpBuffer, &len))
			{
				
					memset(chWord, 0, 256);
					memcpy(chWord, chpBuffer, len);
					if (0 == strcmp(chWord, "}"))
					{
						NS_MaxPos[NsCount] = curPos;
						break;
					}
			}
			NsCount++;
			
		}
	}
	/*for (int i = 0; i < MaxNameSpaceCount; i++)
	{
		printf("���� ��ġ %d \n�� ��ġ %d \n�̸� %s!\n", NS_Pos[i],NS_MaxPos[i],NS_Str[i]);
	}*/
	curPos = 0;
}
bool CTextParser::SkipNoneCommand()
{
	char* chpBuffer = &pFileBuffer[curPos];

	while (1)
	{
		if (curPos  >= MaxNameSpace)
			return false;

		if (*chpBuffer == '/' && *(chpBuffer + 1) == '/')  //�ּ� ������ �Ʒ��ٷ� ���� ���ö����� �̵� 
		{
			while (*chpBuffer != '\n')
			{
				if (curPos >= MaxNameSpace)
					return false;
				chpBuffer++;
				curPos++;
			}
		}

		   //    �޸�                  ��ħǥ                  �����̽�            �齺���̽�?            ��                   �����ǵ�               ĳ���� ���� 
		if (*chpBuffer == ',' || *chpBuffer == '.' || *chpBuffer == 0x20 || *chpBuffer == 0x08 || *chpBuffer == 0x09 || *chpBuffer == 0x0a || *chpBuffer == 0x0d)
		{
			chpBuffer++;
			curPos++;
		}
		else
		{
			break;
		}
	}

	return true;
}
bool CTextParser::GetNextWord(char** chppBuffer, int* ipLength)
{
	
	if (SkipNoneCommand())
	{
		char* chpBuffer = &pFileBuffer[curPos];
		int startPos = curPos;
		while ( 1)
		{
			if (*chpBuffer == '/' && *(chpBuffer + 1) == '/') //�ּ� ������ 
			{
				break;
			}
			//    �޸�                       �����̽�            �齺���̽�?            ��                   �����ǵ�               ĳ���� ���� 
			if (*chpBuffer == ','  || *chpBuffer == 0x20 || *chpBuffer == 0x08 || *chpBuffer == 0x09 || *chpBuffer == 0x0a || *chpBuffer == 0x0d)
			{
				break;
			}
			chpBuffer++;
			curPos++;
		}
		chpBuffer = &pFileBuffer[startPos];
		*chppBuffer = chpBuffer;
		*ipLength = curPos- startPos;
		return true;
	}
	

	return false;
}
bool CTextParser::GetStringWord(char** chppBuffer, int* ipLength)
{
	if (SkipNoneCommand())
	{
		char* chpBuffer = &pFileBuffer[curPos];

		if (*chpBuffer != '"') // ������ ���ڿ��� " �ν������������� ���� 
			return false;
		
		// " �ں��� ���ڿ��޾ƿ� 
		chpBuffer++;
		curPos++;
		int startPos = curPos;
		while (1)
		{
			//  " ���������� ���ڿ� ���� 
			if (*chpBuffer == '"')
			{
				break;
			}
			chpBuffer++;
			curPos++;
		}
		chpBuffer = &pFileBuffer[startPos];
		*chppBuffer = chpBuffer;
		*ipLength = curPos - startPos;
		return true;
	}
	return true;
}

bool CTextParser::GetValue_String(const char* szName, char* ipValue, const char* NameSpace )
{
	char* chpBuff;
	char chWord[256];
	int iLenth;
	if (0 == strcmp(NameSpace, ""))
	{
		// ���ӽ����̽��� �ȳ����� ó������ ������ �������� 
		curPos = 0;
		MaxNameSpace = iFileSize;
	}
	else
	{
		bool isFind = false;
		for (int i = 0; i < MaxNameSpaceCount; i++)
		{
			if (0 == strcmp(NameSpace, NS_Str[i]))
			{
				curPos = NS_Pos[i];
				MaxNameSpace = NS_MaxPos[i];
				isFind = true;
			}
		}
		if (isFind == false) //�ش� ���ӽ����̽� ��ã���� ���� 
			return false;
	}
	while (GetNextWord(&chpBuff, &iLenth))
	{
		memset(chWord, 0, 256);
		memcpy(chWord, chpBuff, iLenth);

		if (0 == strcmp(szName, chWord))
		{
			if (GetNextWord(&chpBuff, &iLenth))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, chpBuff, iLenth);
				if (0 == strcmp(chWord, "="))
				{
					if (GetStringWord(&chpBuff, &iLenth))
					{
						memset(chWord, 0, 256);
						memcpy(chWord, chpBuff, iLenth);
						strcpy(ipValue, chWord);
						return true;
					}
					return false;
				}
			}
			
			return false;
		}

	}
	return true;
}

bool CTextParser::GetValue_WString(const char* szName, wchar_t* ipValue, const char* NameSpace)
{
	char* chpBuff;
	char chWord[256];
	int iLenth;
	if (0 == strcmp(NameSpace, ""))
	{
		// ���ӽ����̽��� �ȳ����� ó������ ������ �������� 
		curPos = 0;
		MaxNameSpace = iFileSize;
	}
	else
	{
		bool isFind = false;
		for (int i = 0; i < MaxNameSpaceCount; i++)
		{
			if (0 == strcmp(NameSpace, NS_Str[i]))
			{
				curPos = NS_Pos[i];
				MaxNameSpace = NS_MaxPos[i];
				isFind = true;
			}
		}
		if (isFind == false) //�ش� ���ӽ����̽� ��ã���� ���� 
			return false;
	}
	while (GetNextWord(&chpBuff, &iLenth))
	{
		memset(chWord, 0, 256);
		memcpy(chWord, chpBuff, iLenth);

		if (0 == strcmp(szName, chWord))
		{
			if (GetNextWord(&chpBuff, &iLenth))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, chpBuff, iLenth);
				if (0 == strcmp(chWord, "="))
				{
					if (GetStringWord(&chpBuff, &iLenth))
					{
						memset(chWord, 0, 256);
						memcpy(chWord, chpBuff, iLenth);
						mbstowcs(ipValue, chWord, 256);
						return true;
					}
					return false;
				}
			}

			return false;
		}

	}
	return true;
}
bool CTextParser::GetValue_Int(const char* szName, int* ipValue, const char* NameSpace )
{
	char* chpBuff;
	char chWord[256];
	int iLenth;
	if (0 == strcmp(NameSpace, ""))
	{
		// ���ӽ����̽��� �ȳ����� ó������ ������ �������� 
		curPos = 0;
		MaxNameSpace = iFileSize;
	}
	else
	{
		bool isFind = false;
		for (int i = 0; i < MaxNameSpaceCount; i++)
		{
			if (0 == strcmp(NameSpace, NS_Str[i]))
			{
				curPos = NS_Pos[i];
				MaxNameSpace = NS_MaxPos[i];
				isFind = true;
			}
		}
		if (isFind == false) //�ش� ���ӽ����̽� ��ã���� ���� 
			return false;
	}
	while (GetNextWord(&chpBuff,&iLenth))
	{
		memset(chWord, 0, 256);
		memcpy(chWord, chpBuff, iLenth);

		if (0 == strcmp(szName, chWord))
		{
			if (GetNextWord(&chpBuff, &iLenth))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, chpBuff, iLenth);
				if (0 == strcmp(chWord,"="))
				{
					if (GetNextWord(&chpBuff, &iLenth))
					{
						memset(chWord, 0, 256);
						memcpy(chWord, chpBuff, iLenth);
						*ipValue = atoi(chWord);
						return true;
					}
					return false;
				}
			}

			return false;
		}

	}
	return true;
}

bool CTextParser::GetValue_Double(const char* szName, double* ipValue, const char* NameSpace)
{
	char* chpBuff;
	char chWord[256];
	int iLenth;
	if (0 == strcmp(NameSpace, ""))
	{
		// ���ӽ����̽��� �ȳ����� ó������ ������ �������� 
		curPos = 0;
		MaxNameSpace = iFileSize;
	}
	else
	{
		bool isFind = false;
		for (int i = 0; i < MaxNameSpaceCount; i++)
		{
			if (0 == strcmp(NameSpace, NS_Str[i]))
			{
				curPos = NS_Pos[i];
				MaxNameSpace = NS_MaxPos[i];
				isFind = true;
			}
		}
		if (isFind == false) //�ش� ���ӽ����̽� ��ã���� ���� 
			return false;
	}
	while (GetNextWord(&chpBuff, &iLenth))
	{
		memset(chWord, 0, 256);
		memcpy(chWord, chpBuff, iLenth);

		if (0 == strcmp(szName, chWord))
		{
			if (GetNextWord(&chpBuff, &iLenth))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, chpBuff, iLenth);
				if (0 == strcmp(chWord, "="))
				{
					if (GetNextWord(&chpBuff, &iLenth))
					{
						memset(chWord, 0, 256);
						memcpy(chWord, chpBuff, iLenth);
						*ipValue = atof(chWord);
						return true;
					}
					return false;
				}
			}

			return false;
		}

	}
	return true;
}