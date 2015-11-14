#ifdef SPLAY_EXPORTS
#define SPLAY_API __declspec(dllexport) 
#else
#define SPLAY_API __declspec(dllimport) 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace SplaY
{
	typedef unsigned int word;
	typedef unsigned char byte;

	#define BufSize 16384 //������ �������� � ��������� ������
	#define Sig 0xff02aa55L //������������ ��������� ������������ ������ ����
	#define MaxChar 256 //������� ������� �������
	#define EofChar 256 //������������ ��� ����������� ����� ������� �����
	#define PredMax 255 //MaxChar-1 
	#define TwiceMax 512 //2*MaxChar 
	#define Root 0 //������ ����� ������

	typedef struct
	{
		unsigned long Signature;
		//����� ��������� ��������� ����, ��� ��� ����� � �.�.
	} FileHeader;

	typedef byte BufferArray[BufSize + 1];
	typedef word CodeType; //0..MaxChar 
	typedef byte UpIndex; //0..PredMax 
	typedef word DownIndex; //0..TwiceMax 
	typedef DownIndex TreeDownArray[PredMax + 1]; //UpIndex 
	typedef UpIndex TreeUpArray[TwiceMax + 1]; //DownIndex 

	BufferArray InBuffer; //input file buffer 
	BufferArray OutBuffer; //output file buffer 
	char InName[80]; //input file name 
	char OutName[80]; //output file name 
	char CompStr[4]; //����� �� Expand?
	FILE *InF; //input file
	FILE *OutF; //output file 

	TreeDownArray Left, Right; //�������� ����� ������ 
	TreeUpArray Up; //������������ �����
	bool CompressFlag;  
	byte BitPos; //������� ��� � ����� 
	CodeType InByte; //������� ������� ���� 
	CodeType OutByte; //������� �������� ���� 
	word InSize; //������� ������ � ������� ������ 
	word OutSize; //������� ������ � �������� ������ 
	word Index; //������ ������ ����������

	char *Usage = {"Usage: splay [x] infile outfile\n\n"
					"Where 'x' denotes expand infile to outfile\n"
					"Normally compress infile to outfile\n"};

	byte BitMask[8]={1, 2, 4, 8, 16, 32, 64, 128};

	class SplayAlg
	{
	private:
		void InitializeSplay(void);
		void ReadHeader(void);
		byte GetByte(void);
		CodeType Expand(void);
		void ExpandFile(void);
	public:
		SplayAlg(void);
		SPLAY_API void mainSplay(int count, char *args[]);
	};
}
