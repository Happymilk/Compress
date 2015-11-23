#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

namespace AR002
{
	typedef unsigned char uchar; //8 bits or more
	typedef unsigned int uint; //16 bits or more 
	typedef unsigned short ushort; //16 bits or more 
	typedef unsigned long ulong; //32 bits or more 
	typedef short node;

	#define FNAME_MAX (255 - 25) //max strlen(filename) 
	#define namelen  header[19]
	#define INIT_CRC 0 //CCITT: 0xFFFF 
	#define BITBUFSIZ (CHAR_BIT * sizeof bitbuf)
	#define DICBIT 13 //12(-lh4-) or 13(-lh5-) 
	#define DICSIZ (1U << DICBIT)
	#define MATCHBIT 8 //bits for MAXMATCH - THRESHOLD 
	#define MAXMATCH 256 //formerly F (not more than UCHAR_MAX + 1) 
	#define THRESHOLD 3 //choose optimal value 
	#define PERC_FLAG 0x8000U
	#define NC (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD) //alphabet = {0, 1, 2, ..., NC - 1} 
	#define CBIT 9 // $\lfloor \log_2 NC \rfloor + 1$ 
	#define CODE_BIT 16 //codeword length 
	#define PERCOLATE 1
	#define MAX_HASH_VAL (3 * DICSIZ + (DICSIZ / 512 + 1) * UCHAR_MAX)
	#if MAXMATCH <= (UCHAR_MAX + 1)
		static uchar *level;
	#else
		static ushort *level;
	#endif
	#define HASH(p, c) ((p) + ((c) << (DICBIT - 9)) + DICSIZ * 2)
	#define NP (DICBIT + 1)
	#define NT (CODE_BIT + 3)
	#define PBIT 4 //smallest integer such that (1U << PBIT) > NP 
	#define TBIT 5 //smallest integer such that (1U << TBIT) > NT 
	#if NT > NP
		#define NPT NT
	#else
		#define NPT NP
	#endif
	#define CRCPOLY 0xA001 //ANSI CRC-16 CCITT: 0x8408 
	#define UPDATE_CRC(c) crc=crctable[(crc^(c))&0xFF]^(crc>>CHAR_BIT)

	class AR002Alg
	{
	public:
		AR002Alg(void);
		void mainAr(int argsc, char *args1[], char *args2[], char *args3[]);
	};
}
