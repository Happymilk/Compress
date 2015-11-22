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
