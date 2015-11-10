#ifdef SPLAY_EXPORTS
#define SPLAY_API __declspec(dllexport) 
#else
#define SPLAY_API __declspec(dllimport) 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace Splay
{
	typedef unsigned int word;
	typedef unsigned char byte;

	#define BufSize 16384 //размер входного и выходного буфера
	#define Sig 0xff02aa55L //произвольная сигнатура обозначающая сжатый файл
	#define MaxChar 256 //порядок высшего символа
	#define EofChar 256 //используется для обозначения конца сжатого файла
	#define PredMax 255 //MaxChar-1 
	#define TwiceMax 512 //2*MaxChar 
	#define Root 0 //индекс корня записи

	typedef struct
	{
		unsigned long Signature;
		//можно поместить различную инфу, как имя файла и т.д.
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
	char CompStr[4]; //ответ от Expand?
	FILE *InF; //input file
	FILE *OutF; //output file 

	TreeDownArray Left, Right; //дочерние ветви дерева 
	TreeUpArray Up; //родительская ветвь
	bool CompressFlag;  
	byte BitPos; //текущий бит в байте 
	CodeType InByte; //текущий входной байт 
	CodeType OutByte; //текущий выходной байт 
	word InSize; //текущий символ в входном буфере 
	word OutSize; //текущий символ в выходном буфере 
	word Index; //индекс общего назначения

	char *Usage = {"Usage: splay [x] infile outfile\n\n"
					"Where 'x' denotes expand infile to outfile\n"
					"Normally compress infile to outfile\n"};

	byte BitMask[8]={1, 2, 4, 8, 16, 32, 64, 128};

	class SplayAlg
	{
	private:
		void InitializeSplay(void);

		/* rearrange the splay tree for each succeeding character */
		void Splay(CodeType Plain)
		{
			DownIndex A, B;
			UpIndex C, D;
    
			A = Plain + MaxChar;
    
			do
			{
				/* walk up the tree semi-rotating pairs */
				C = Up[A];
				if (C != Root)
				{
					/* a pair remains */
					D = Up[C];
            
					/* exchange children of pair */
					B = Left[D];
					if (C == B)
					{
						B = Right[D];
						Right[D] = A;
					}
					else
						Left[D] = A;
            
					if (A == Left[C])
						Left[C] = B;
					else
						Right[C] = B;
            
					Up[A] = D;
					Up[B] = C;
					A = D;
				}
				else
					A = C;
			} while (A != Root);
		}

		/* flush output buffer and reset */
		void FlushOutBuffer(void)
		{
			if (OutSize > 0)
			{
				fwrite(OutBuffer+1, sizeof(byte), OutSize, OutF);
				OutSize = 0;
			}
		}

		/* output byte in OutByte */
		void WriteByte(void)
		{
			if (OutSize == BufSize)
				FlushOutBuffer();
			OutSize++;
			OutBuffer[OutSize] = OutByte;
		}

		/* compress a single char */
		void Compress(CodeType Plain)
		{
			DownIndex A;
			UpIndex U;
			word Sp;
			bool Stack[PredMax+1];
    
			A = Plain + MaxChar;
			Sp = 0;
    
			/* walk up the tree pushing bits onto stack */
			do
			{
				U = Up[A];
				Stack[Sp] = (Right[U] == A);
				Sp++;
				A = U;
			} while (A != Root);
    
			/* unstack to transmit bits in correct order */
			do
			{
				Sp--;
				if (Stack[Sp])
					OutByte |= BitMask[BitPos];
				if (BitPos == 7)
				{
					/* byte filled with bits, write it out */
					WriteByte();
					BitPos = 0;
					OutByte = 0;
				}
				else
					BitPos++;
			} while (Sp != 0);
    
			/* update the tree */
			Splay(Plain);
		}

		/* compress input file, writing to outfile */
		void CompressFile(void)
		{
			FileHeader Header;
    
			/* write header to output */
			Header.Signature = Sig;
			fwrite(&Header, sizeof(FileHeader), 1, OutF);
    
			/* compress file */
			OutSize = 0;
			BitPos = 0;
			OutByte = 0;
			do
			{
				InSize = fread(InBuffer+1, sizeof(byte), BufSize, InF);
				for (Index = 1; Index <= InSize; Index++)
					Compress(InBuffer[Index]);
			} while (InSize >= BufSize);
    
			/* Mark end of file */
			Compress(EofChar);
    
			/* Flush buffers */
			if (BitPos != 0)
				WriteByte();
			FlushOutBuffer();
		}

		void ReadHeader(void);
		byte GetByte(void);
		CodeType Expand(void);
		void ExpandFile(void);
	public:
		SplayAlg(void);
		void mainSplay(int count, char *args[]);
	};
}
