#ifdef AR002_EXPORTS
#define AR002_API __declspec(dllexport) 
#else
#define AR002_API __declspec(dllimport) 
#endif

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
	#define filename ((char *)&header[20])
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

	FILE *arcfile, *infile, *outfile;
	uint crc, bitbuf;	
	int unpackable;            
	ulong compsize, origsize;  
	ushort left[2 * NC - 1], right[2 * NC - 1];

	static int n, heapsize, remainder, matchlen, bitcount;
	static short heap[NC + 1];
	static char *temp_name;

	static ushort *freq, *sortptr, len_cnt[17], crctable[UCHAR_MAX + 1], c_freq[2 * NC - 1], 
		c_table[4096], c_code[NC], p_freq[2 * NP - 1], pt_table[256], pt_code[NPT], t_freq[2 * NT - 1];

	static uchar *len, *buf, c_len[NC], pt_len[NPT], buffer[DICSIZ], header[255], 
		headersize, headersum,  *text, *childcount;
	
	static uint subbitbuf, bufsiz = 0, blocksize, file_crc, output_pos, output_mask;

	static node pos, matchpos, avail, *position, *parent, *prev, *next = NULL;

	class AR002Alg
	{
	private:
		void error(char *fmt, ...);
		void make_crctable(void);
		void fillbuf(int n);
		uint getbits(int n);
		void putbits(int n, uint x);
		int fread_crc(uchar *p, int n, FILE *f);
		void fwrite_crc(uchar *p, int n, FILE *f);
		void init_getbits(void);
		void init_putbits(void);

		void huf_encode_start(void);
		void huf_decode_start(void);
		uint decode_c(void);
		uint decode_p(void);
		void output(uint c, uint p);
		void huf_encode_end(void);

		void make_table(int nchar, uchar bitlen[], int tablebits, ushort table[]);

		int make_tree(int nparm, ushort freqparm[],	uchar lenparm[], ushort codeparm[]);
	public:
		AR002Alg(void);
		AR002_API void encode(void);
		AR002_API void decode_start(void);
		AR002_API void decode(uint count, uchar text[]);
	};
}
