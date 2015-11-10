#include "AR002Alg.h"

using namespace AR002;

AR002Alg::AR002Alg(void)
{
//	int main(int argc, char *argv[])
//{
//	int i, j, cmd, count, nfiles, found, done;
//
//	/* Check command line arguments. */
//	if (argc < 3
//	 || argv[1][1] != '\0'
//	 || ! strchr("AXRDPL", cmd = toupper(argv[1][0]))
//	 || (argc == 3 && strchr("AD", cmd)))
//		error(usage);
//
//	/* Wildcards used? */
//	for (i = 3; i < argc; i++)
//		if (strpbrk(argv[i], "*?")) break;
//	if (cmd == 'A' && i < argc)
//		error("Filenames may not contain '*' and '?'");
//	if (i < argc) nfiles = -1;  /* contains wildcards */
//	else nfiles = argc - 3;     /* number of files to process */
//
//	/* Open archive. */
//	arcfile = fopen(argv[2], "rb");
//	if (arcfile == NULL && cmd != 'A')
//		error("Can't open archive '%s'", argv[2]);
//
//	/* Open temporary file. */
//	if (strchr("ARD", cmd)) {
//		temp_name = tmpnam(NULL);
//		outfile = fopen(temp_name, "wb");
//		if (outfile == NULL)
//			error("Can't open temporary file");
//		atexit(exitfunc);
//	} else temp_name = NULL;
//
//	make_crctable();  count = done = 0;
//
//	if (cmd == 'A') {
//		for (i = 3; i < argc; i++) {
//			for (j = 3; j < i; j++)
//				if (strcmp(argv[j], argv[i]) == 0) break;
//			if (j == i) {
//				strcpy(filename, argv[i]);
//				if (add(0)) count++;  else argv[i][0] = 0;
//			} else nfiles--;
//		}
//		if (count == 0 || arcfile == NULL) done = 1;
//	}
//
//	while (! done && read_header()) {
//		found = search(argc, argv);
//		switch (cmd) {
//		case 'R':
//			if (found) {
//				if (add(1)) count++;  else copy();
//			} else copy();
//			break;
//		case 'A':  case 'D':
//			if (found) {
//				count += (cmd == 'D');  skip();
//			} else copy();
//			break;
//		case 'X':  case 'P':
//			if (found) {
//				extract(cmd == 'X');
//				if (++count == nfiles) done = 1;
//			} else skip();
//			break;
//		case 'L':
//			if (found) {
//				if (count == 0) list_start();
//				list();
//				if (++count == nfiles) done = 1;
//			}
//			skip();  break;
//		}
//	}
//
//	if (temp_name != NULL && count != 0) {
//		fputc(0, outfile);  /* end of archive */
//		if (ferror(outfile) || fclose(outfile) == EOF)
//			error("Can't write");
//		remove(argv[2]);  rename(temp_name, argv[2]);
//	}
//
//	printf("  %d files\n", count);
//	return EXIT_SUCCESS;
//}
}

//io
void error(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	putc('\n', stderr);
	vfprintf(stderr, fmt, args);
	putc('\n', stderr);
	va_end(args);
}

void make_crctable(void)
{
	uint i, j, r;

	for (i = 0; i <= UCHAR_MAX; i++) 
	{
		r = i;
		for (j = 0; j < CHAR_BIT; j++)
			if (r & 1) 
				r = (r >> 1) ^ CRCPOLY;
			else
				r >>= 1;
		crctable[i] = r;
	}
}

void fillbuf(int n) //Shift bitbuf n bits left, read n bits 
{
	bitbuf <<= n;
	while (n > bitcount) 
	{
		bitbuf |= subbitbuf << (n -= bitcount);
		if (compsize != 0) 
		{
			compsize--;  
			subbitbuf = (uchar)getc(arcfile);
		} 
		else 
			subbitbuf = 0;
		bitcount = CHAR_BIT;
	}
	bitbuf |= subbitbuf >> (bitcount -= n);
}

uint getbits(int n)
{
	uint x;

	x = bitbuf >> (BITBUFSIZ - n);  
	fillbuf(n);
	return x;
}

void putbits(int n, uint x) //Write rightmost n bits of x 
{
	if (n < bitcount) 
		subbitbuf |= x << (bitcount -= n); 
	else 
	{
		if (compsize < origsize) 
		{
			putc(subbitbuf | (x >> (n -= bitcount)), outfile);
			compsize++;
		} 
		else 
			unpackable = 1;
		if (n < CHAR_BIT) 
			subbitbuf = x << (bitcount = CHAR_BIT - n); 
		else 
		{
			if (compsize < origsize) 
			{
				putc(x >> (n - CHAR_BIT), outfile);
				compsize++;
			} 
			else 
				unpackable = 1;
			subbitbuf = x << (bitcount = 2 * CHAR_BIT - n);
		}
	}
}

int fread_crc(uchar *p, int n, FILE *f)
{
	int i;

	i = n = fread(p, 1, n, f);  
	origsize += n;
	while (--i >= 0) 
		UPDATE_CRC(*p++);
	return n;
}

void fwrite_crc(uchar *p, int n, FILE *f)
{
	if (fwrite(p, 1, n, f) < n) 
		error("Unable to write");
	while (--n >= 0) 
		UPDATE_CRC(*p++);
}

void init_getbits(void)
{
	bitbuf = 0;  
	subbitbuf = 0;  
	bitcount = 0;
	fillbuf(BITBUFSIZ);
}

void init_putbits(void)
{
	bitcount = CHAR_BIT;  
	subbitbuf = 0;
}
//

//make tree
static void count_len(int i)  /* call with i = root */
{
	static int depth = 0;

	if (i < n) 
		len_cnt[(depth < 16) ? depth : 16]++;
	else 
	{
		depth++;
		count_len(left[i]);
		count_len(right[i]);
		depth--;
	}
}

static void make_len(int root)
{
	int i, k;
	uint cum;

	for (i = 0; i <= 16; i++) 
		len_cnt[i] = 0;
	count_len(root);
	cum = 0;
	for (i = 16; i > 0; i--)
		cum += len_cnt[i] << (16 - i);
	while (cum != (1U << 16)) 
	{
		fprintf(stderr, "17");
		len_cnt[16]--;
		for (i = 15; i > 0; i--) 
		{
			if (len_cnt[i] != 0) 
			{
				len_cnt[i]--;  
				len_cnt[i+1] += 2;  
				break;
			}
		}
		cum--;
	}
	for (i = 16; i > 0; i--) 
	{
		k = len_cnt[i];
		while (--k >= 0) 
			len[*sortptr++] = i;
	}
}

static void downheap(int i) //priority queue; send i-th entry down heap 
{
	int j, k;

	k = heap[i];
	while ((j = 2 * i) <= heapsize) 
	{
		if (j < heapsize && freq[heap[j]] > freq[heap[j + 1]])
		 	j++;
		if (freq[k] <= freq[heap[j]]) 
			break;
		heap[i] = heap[j];  
		i = j;
	}
	heap[i] = k;
}

static void make_code(int n, uchar len[], ushort code[])
{
	int i;
	ushort start[18];

	start[1] = 0;
	for (i = 1; i <= 16; i++)
		start[i + 1] = (start[i] + len_cnt[i]) << 1;
	for (i = 0; i < n; i++) 
		code[i] = start[len[i]]++;
}

int make_tree(int nparm, ushort freqparm[], uchar lenparm[], ushort codeparm[])
	//make tree, calculate len[], return root 
{
	int i, j, k, avail;

	n = nparm;  
	freq = freqparm;  
	len = lenparm;
	avail = n;  
	heapsize = 0;  
	heap[1] = 0;
	for (i = 0; i < n; i++) 
	{
		len[i] = 0;
		if (freq[i]) 
			heap[++heapsize] = i;
	}
	if (heapsize < 2) 
	{
		codeparm[heap[1]] = 0;  
		return heap[1];
	}
	for (i = heapsize / 2; i >= 1; i--)
		downheap(i);  //make priority queue 
	sortptr = codeparm;
	do //while queue has at least two entries
	{  
		i = heap[1];  //take out least-freq entry 
		if (i < n) 
			*sortptr++ = i;
		heap[1] = heap[heapsize--];
		downheap(1);
		j = heap[1];  //next least-freq entry 
		if (j < n) 
			*sortptr++ = j;
		k = avail++;  //generate new node
		freq[k] = freq[i] + freq[j];
		heap[1] = k;  
		downheap(1);  //put into queue 
		left[k] = i;  
		right[k] = j;
	} 
	while (heapsize > 1);
	sortptr = codeparm;
	make_len(k);
	make_code(nparm, lenparm, codeparm);
	return k;  //return root 
}
//

//make table
void make_table(int nchar, uchar bitlen[], int tablebits, ushort table[])
{
	ushort count[17], weight[17], start[18], *p;
	uint i, k, len, ch, jutbits, avail, nextcode, mask;

	for (i = 1; i <= 16; i++) 
		count[i] = 0;
	for (i = 0; i < nchar; i++) 
		count[bitlen[i]]++;

	start[1] = 0;
	for (i = 1; i <= 16; i++)
		start[i + 1] = start[i] + (count[i] << (16 - i));
	if (start[17] != (ushort)(1U << 16)) 
		error("Bad table");

	jutbits = 16 - tablebits;
	for (i = 1; i <= tablebits; i++) 
	{
		start[i] >>= jutbits;
		weight[i] = 1U << (tablebits - i);
	}
	while (i <= 16) 
		weight[i++] = 1U << (16 - i);

	i = start[tablebits + 1] >> jutbits;
	if (i != (ushort)(1U << 16)) 
	{
		k = 1U << tablebits;
		while (i != k) 
			table[i++] = 0;
	}

	avail = nchar;
	mask = 1U << (15 - tablebits);
	for (ch = 0; ch < nchar; ch++) 
	{
		if ((len = bitlen[ch]) == 0) 
			continue;
		nextcode = start[len] + weight[len];
		if (len <= tablebits) 
		{
			for (i = start[len]; i < nextcode; i++) 
				table[i] = ch;
		} 
		else 
		{
			k = start[len];
			p = &table[k >> jutbits];
			i = len - tablebits;
			while (i != 0) 
			{
				if (*p == 0) 
				{
					right[avail] = left[avail] = 0;
					*p = avail++;
				}
				if (k & mask) 
					p = &right[*p];
				else
					p = &left[*p];
				k <<= 1;  
				i--;
			}
			*p = ch;
		}
		start[len] = nextcode;
	}
}
//

//huf

//***!encoding!***
static void count_t_freq(void)
{
	int i, k, n, count;

	for (i = 0; i < NT; i++) 
		t_freq[i] = 0;
	n = NC;
	while (n > 0 && c_len[n - 1] == 0) 
		n--;
	i = 0;
	while (i < n) 
	{
		k = c_len[i++];
		if (k == 0) 
		{
			count = 1;
			while (i < n && c_len[i] == 0) 
			{  
				i++;  
				count++;  
			}
			if (count <= 2) 
				t_freq[0] += count;
			else 
				if (count <= 18) 
					t_freq[1]++;
				else 
					if (count == 19) 
					{  
						t_freq[0]++;  
						t_freq[1]++; 
					}
					else 
						t_freq[2]++;
		} 
		else 
			t_freq[k + 2]++;
	}
}

static void write_pt_len(int n, int nbit, int i_special)
{
	int i, k;

	while (n > 0 && pt_len[n - 1] == 0) 
		n--;
	putbits(nbit, n);
	i = 0;
	while (i < n) 
	{
		k = pt_len[i++];
		if (k <= 6) 
			putbits(3, k);
		else 
			putbits(k - 3, (1U << (k - 3)) - 2);
		if (i == i_special) 
		{
			while (i < 6 && pt_len[i] == 0) 
				i++;
			putbits(2, (i - 3) & 3);
		}
	}
}

static void write_c_len(void)
{
	int i, k, n, count;

	n = NC;
	while (n > 0 && c_len[n - 1] == 0) 
		n--;
	putbits(CBIT, n);
	i = 0;
	while (i < n) 
	{
		k = c_len[i++];
		if (k == 0) 
		{
			count = 1;
			while (i < n && c_len[i] == 0) 
			{  
				i++; 
				count++;
			}
			if (count <= 2)
			{
				for (k = 0; k < count; k++)
					putbits(pt_len[0], pt_code[0]);
			} 
			else 
				if (count <= 18) 
				{
					putbits(pt_len[1], pt_code[1]);
					putbits(4, count - 3);
				} 
				else 
					if (count == 19) 
					{
						putbits(pt_len[0], pt_code[0]);
						putbits(pt_len[1], pt_code[1]);
						putbits(4, 15);
					} 
					else 
					{
						putbits(pt_len[2], pt_code[2]);
						putbits(CBIT, count - 20);
					}
		} 
		else 
			putbits(pt_len[k + 2], pt_code[k + 2]);
	}
}

static void encode_c(int c)
{
	putbits(c_len[c], c_code[c]);
}

static void encode_p(uint p)
{
	uint c, q;

	c = 0;  
	q = p;  
	while (q) 
	{  
		q >>= 1; 
		c++; 
	}
	putbits(pt_len[c], pt_code[c]);
	if (c > 1) 
		putbits(c - 1, p & (0xFFFFU >> (17 - c)));
}

static void send_block(void)
{
	uint i, k, flags, root, pos, size;

	root = make_tree(NC, c_freq, c_len, c_code);
	size = c_freq[root]; 
	putbits(16, size);
	if (root >= NC) 
	{
		count_t_freq();
		root = make_tree(NT, t_freq, pt_len, pt_code);
		if (root >= NT) 
			write_pt_len(NT, TBIT, 3); 
		else 
			putbits(TBIT, 0);  putbits(TBIT, root);
		write_c_len();
	} 
	else 
	{
        putbits(TBIT, 0);  
		putbits(TBIT, 0);
		putbits(CBIT, 0);  
		putbits(CBIT, root);
	}
	root = make_tree(NP, p_freq, pt_len, pt_code);
	if (root >= NP) 
		write_pt_len(NP, PBIT, -1);
	else 
		putbits(PBIT, 0);  putbits(PBIT, root);
	pos = 0;
	for (i = 0; i < size; i++) 
	{
		if (i % CHAR_BIT == 0) 
			flags = buf[pos++];  
		else 
			flags <<= 1;
		if (flags & (1U << (CHAR_BIT - 1))) 
		{
			encode_c(buf[pos++] + (1U << CHAR_BIT));
			k = buf[pos++] << CHAR_BIT;  
			k += buf[pos++];
			encode_p(k);
		} 
		else 
			encode_c(buf[pos++]);
		if (unpackable) 
			return;
	}
	for (i = 0; i < NC; i++) 
		c_freq[i] = 0;
	for (i = 0; i < NP; i++) 
		p_freq[i] = 0;
}

void output(uint c, uint p)
{
	static uint cpos;

	if ((output_mask >>= 1) == 0) 
	{
		output_mask = 1U << (CHAR_BIT - 1);
		if (output_pos >= bufsiz - 3 * CHAR_BIT) 
		{
			send_block();
			if (unpackable) 
				return;
			output_pos = 0;
		}
		cpos = output_pos++;  
		buf[cpos] = 0;
	}
	buf[output_pos++] = (uchar) c;  
	c_freq[c]++;
	if (c >= (1U << CHAR_BIT)) 
	{
		buf[cpos] |= output_mask;
		buf[output_pos++] = (uchar)(p >> CHAR_BIT);
		buf[output_pos++] = (uchar) p;
		c = 0; 
		while (p) 
		{  
			p >>= 1; 
			c++; 
		}
		p_freq[c]++;
	}
}

void huf_encode_start(void)
{
	int i;

	if (bufsiz == 0) 
	{
        bufsiz = 16 * 1024U;
		while ((buf = (uchar*)malloc(bufsiz)) == NULL)
		{
			bufsiz = (bufsiz / 10U) * 9U;
			if (bufsiz < 4 * 1024U) 
				error("Out of memory.");
		}
	}
	buf[0] = 0;
	for (i = 0; i < NC; i++) 
		c_freq[i] = 0;
	for (i = 0; i < NP; i++) 
		p_freq[i] = 0;
	output_pos = output_mask = 0;
	init_putbits();
}

void huf_encode_end(void)
{
	if (!unpackable) 
	{
		send_block();
		putbits(CHAR_BIT - 1, 0);  //flush remaining bits 
	}
}

//***!decoding!***
static void read_pt_len(int nn, int nbit, int i_special)
{
	int i, c, n;
	uint mask;

	n = getbits(nbit);
	if (n == 0) 
	{
		c = getbits(nbit);
		for (i = 0; i < nn; i++) 
			pt_len[i] = 0;
		for (i = 0; i < 256; i++) 
			pt_table[i] = c;
	} 
	else 
	{
		i = 0;
		while (i < n) 
		{
			c = bitbuf >> (BITBUFSIZ - 3);
			if (c == 7) 
			{
				mask = 1U << (BITBUFSIZ - 1 - 3);
				while (mask & bitbuf) 
				{  
					mask >>= 1;  
					c++;  
				}
			}
			fillbuf((c < 7) ? 3 : c - 3);
			pt_len[i++] = c;
			if (i == i_special) 
			{
				c = getbits(2);
				while (--c >= 0) 
					pt_len[i++] = 0;
			}
		}
		while (i < nn) 
			pt_len[i++] = 0;
		make_table(nn, pt_len, 8, pt_table);
	}
}

static void read_c_len(void)
{
	int i, c, n;
	uint mask;

	n = getbits(CBIT);
	if (n == 0) 
	{
		c = getbits(CBIT);
		for (i = 0; i < NC; i++) 
			c_len[i] = 0;
		for (i = 0; i < 4096; i++) 
			c_table[i] = c;
	} 
	else 
	{
		i = 0;
		while (i < n) 
		{
			c = pt_table[bitbuf >> (BITBUFSIZ - 8)];
			if (c >= NT) 
			{
				mask = 1U << (BITBUFSIZ - 1 - 8);
				do 
				{
					if (bitbuf & mask) 
						c = right[c];
					else
						c = left [c];
					mask >>= 1;
				} 
				while (c >= NT);
			}
			fillbuf(pt_len[c]);
			if (c <= 2) 
			{
				if (c == 0) 
					c = 1;
				else 
					if (c == 1) 
						c = getbits(4) + 3;
					else 
						c = getbits(CBIT) + 20;
				while (--c >= 0) 
					c_len[i++] = 0;
			} 
			else 
				c_len[i++] = c - 2;
		}
		while (i < NC) 
			c_len[i++] = 0;
		make_table(NC, c_len, 12, c_table);
	}
}

uint decode_c(void)
{
	uint j, mask;

	if (blocksize == 0) 
	{
		blocksize = getbits(16);
		read_pt_len(NT, TBIT, 3);
		read_c_len();
		read_pt_len(NP, PBIT, -1);
	}
	blocksize--;
	j = c_table[bitbuf >> (BITBUFSIZ - 12)];
	if (j >= NC) 
	{
		mask = 1U << (BITBUFSIZ - 1 - 12);
		do 
		{
			if (bitbuf & mask) 
				j = right[j];
			else
				j = left [j];
			mask >>= 1;
		} 
		while (j >= NC);
	}
	fillbuf(c_len[j]);
	return j;
}

uint decode_p(void)
{
	uint j, mask;

	j = pt_table[bitbuf >> (BITBUFSIZ - 8)];
	if (j >= NP) 
	{
		mask = 1U << (BITBUFSIZ - 1 - 8);
		do 
		{
			if (bitbuf & mask) 
				j = right[j];
			else
				j = left [j];
			mask >>= 1;
		} 
		while (j >= NP);
	}
	fillbuf(pt_len[j]);
	if (j != 0) 
		j = (1U << (j - 1)) + getbits(j - 1);
	return j;
}

void huf_decode_start(void)
{
	init_getbits();  
	blocksize = 0;
}
//

//decode
static int j;  //remaining bytes to copy 

void decode_start(void)
{
	huf_decode_start();
	j = 0;
}

void decode(uint count, uchar buffer[])
/* The calling function must keep the number of bytes to be processed.  
   This function decodes either 'count' bytes or 'DICSIZ' bytes, whichever
   is smaller, into the array 'buffer[]' of size 'DICSIZ' or more. Call 
   decode_start() once for each new file before calling this function.*/
{
	static uint i;
	uint r, c;

	r = 0;
	while (--j >= 0) 
	{
		buffer[r] = buffer[i];
		i = (i + 1) & (DICSIZ - 1);
		if (++r == count) 
			return;
	}
	for ( ; ; ) 
	{
		c = decode_c();
		if (c <= UCHAR_MAX) 
		{
			buffer[r] = c;
			if (++r == count) 
				return;
		} 
		else 
		{
			j = c - (UCHAR_MAX + 1 - THRESHOLD);
			i = (r - decode_p() - 1) & (DICSIZ - 1);
			while (--j >= 0) 
			{
				buffer[r] = buffer[i];
				i = (i + 1) & (DICSIZ - 1);
				if (++r == count) 
					return;
			}
		}
	}
}
//

//encode
static void allocate_memory(void)
{
    if (next != NULL) 
		return;
    text = (uchar*)malloc(DICSIZ * 2 + MAXMATCH);
    level = (uchar*)malloc((DICSIZ + UCHAR_MAX + 1) * sizeof(*level));
    childcount = (uchar*)malloc((DICSIZ + UCHAR_MAX + 1) * sizeof(*childcount));
    #if PERCOLATE
        position = (node*)malloc((DICSIZ + UCHAR_MAX + 1) * sizeof(*position));
    #else
        position = (node*)malloc(DICSIZ * sizeof(*position));
    #endif
    parent = (node*)malloc(DICSIZ * 2 * sizeof(*parent));
    prev = (node*)malloc(DICSIZ * 2 * sizeof(*prev));
    next = (node*)malloc((MAX_HASH_VAL + 1) * sizeof(*next));
    if (next == NULL) 
		error("Out of memory.");
}

static void init_slide(void)
{
    node i;

    for (i = DICSIZ; i <= DICSIZ + UCHAR_MAX; i++) 
	{
        level[i] = 1;
        #if PERCOLATE
            position[i] = NULL;  //sentinel
		#endif
    }
    for (i = DICSIZ; i < DICSIZ * 2; i++) 
		parent[i] = NULL;
    avail = 1;
    for (i = 1; i < DICSIZ - 1; i++) 
		next[i] = i + 1;
    next[DICSIZ - 1] = NULL;
    for (i = DICSIZ * 2; i <= MAX_HASH_VAL; i++) 
		next[i] = NULL;
}

static node child(node q, uchar c) //q's child for character c (NULL if not found) 
{
    node r;

    r = next[HASH(q, c)];
    parent[NULL] = q;  //sentinel 
    while (parent[r] != q)
		r = next[r];
    return r;
}

static void makechild(node q, uchar c, node r) //Let r be q's child for character c. 
{
    node h, t;

    h = HASH(q, c);
    t = next[h];  
	next[h] = r;  
	next[r] = t;
    prev[t] = r;  
	prev[r] = h;
    parent[r] = q;  
	childcount[q]++;
}

void split(node old)
{
    node New, t;

    New = avail;  
	avail = next[New];  
	childcount[New] = 0;
    t = prev[old];  
	prev[New] = t;  
	next[t] = New;
    t = next[old];  
	next[New] = t;  
	prev[t] = New;
    parent[New] = parent[old];
    level[New] = matchlen;
    position[New] = pos;
    makechild(New, text[matchpos + matchlen], old);
    makechild(New, text[pos + matchlen], pos);
}

static void insert_node(void)
{
    node q, r, j, t;
    uchar c, *t1, *t2;

    if (matchlen >= 4) 
	{
        matchlen--;
        r = (matchpos + 1) | DICSIZ;
        while ((q = parent[r]) == NULL) 
			r = next[r];
        while (level[q] >= matchlen) 
		{
                r = q;  
				q = parent[q];
        }
        #if PERCOLATE
			t = q;
            while (position[t] < 0) 
			{
				position[t] = pos;  
				t = parent[t];
            }
            if (t < DICSIZ) 
				position[t] = pos | PERC_FLAG;
        #else
            t = q;
            while (t < DICSIZ) 
			{
				position[t] = pos;  
				t = parent[t];
            }
        #endif
    } 
	else 
	{
        q = text[pos] + DICSIZ;  
		c = text[pos + 1];
        if ((r = child(q, c)) == NULL) 
		{
            makechild(q, c, pos);  
			matchlen = 1;
            return;
        }
        matchlen = 2;
    }
    for ( ; ; ) 
	{
        if (r >= DICSIZ) 
		{
			j = MAXMATCH;  
			matchpos = r;
        } 
		else 
		{
            j = level[r];
            matchpos = position[r] & ~PERC_FLAG;
        }
        if (matchpos >= pos) 
			matchpos -= DICSIZ;
        t1 = &text[pos + matchlen];  
		t2 = &text[matchpos + matchlen];
        while (matchlen < j) 
		{
            if (*t1 != *t2) 
			{  
				split(r);  
				return;
			}
            matchlen++;  
			t1++;  
			t2++;
        }
        if (matchlen >= MAXMATCH) 
			break;
        position[r] = pos;
        q = r;
        if ((r = child(q, *t1)) == NULL) 
		{
            makechild(q, *t1, pos); 
			return;
        }
        matchlen++;
    }
    t = prev[r];
	prev[pos] = t;  
	next[t] = pos;
    t = next[r];  
	next[pos] = t;  
	prev[t] = pos;
    parent[pos] = q; 
	parent[r] = NULL;
    next[r] = pos;  //special use of next[] 
}

static void delete_node(void)
{
    #if PERCOLATE
        node q, r, s, t, u;
    #else
        node r, s, t, u;
    #endif

    if (parent[pos] == NULL) 
		return;
    r = prev[pos];  
	s = next[pos];
    next[r] = s;  
	prev[s] = r;
    r = parent[pos];
	parent[pos] = NULL;
    if (r >= DICSIZ || --childcount[r] > 1) 
		return;
    #if PERCOLATE
        t = position[r] & ~PERC_FLAG;
    #else
        t = position[r];
    #endif
    if (t >= pos) 
		t -= DICSIZ;
    #if PERCOLATE
        s = t;  
		q = parent[r];
        while ((u = position[q]) & PERC_FLAG)
		{
            u &= ~PERC_FLAG;  
			if (u >= pos) 
				u -= DICSIZ;
            if (u > s) 
				s = u;
            position[q] = (s | DICSIZ); 
			q = parent[q];
        }
        if (q < DICSIZ) 
		{
            if (u >= pos) 
				u -= DICSIZ;
            if (u > s) s = u;
                position[q] = s | DICSIZ | PERC_FLAG;
        }
    #endif
    s = child(r, text[t + level[r]]);
    t = prev[s]; 
	u = next[s];
    next[t] = u; 
	prev[u] = t;
    t = prev[r];  
	next[t] = s; 
	prev[s] = t;
    t = next[r]; 
	prev[t] = s;  
	next[s] = t;
    parent[s] = parent[r]; 
	parent[r] = NULL;
    next[r] = avail;  
	avail = r;
}

static void get_next_match(void)
{
    int n;

    remainder--;
    if (++pos == DICSIZ * 2) 
	{
        memmove(&text[0], &text[DICSIZ], DICSIZ + MAXMATCH);
        n = fread_crc(&text[DICSIZ + MAXMATCH], DICSIZ, infile);
        remainder += n;  pos = DICSIZ;  putc('.', stderr);
    }
    delete_node();  
	insert_node();
}

void encode(void)
{
    int lastmatchlen;
    node lastmatchpos;

    allocate_memory(); 
	init_slide(); 
	huf_encode_start();
    remainder = fread_crc(&text[DICSIZ], DICSIZ + MAXMATCH, infile);
    putc('.', stderr);
    matchlen = 0;
    pos = DICSIZ;  
	insert_node();
    if (matchlen > remainder) 
		matchlen = remainder;
    while (remainder > 0 && ! unpackable) 
	{
        lastmatchlen = matchlen; 
		lastmatchpos = matchpos;
        get_next_match();
        if (matchlen > remainder) 
			matchlen = remainder;
        if (matchlen > lastmatchlen || lastmatchlen < THRESHOLD)
            output(text[pos - 1], 0);
        else 
		{
            output(lastmatchlen + (UCHAR_MAX + 1 - THRESHOLD),
			   (pos - lastmatchpos - 2) & (DICSIZ - 1));
            while (--lastmatchlen > 0) 
				get_next_match();
            if (matchlen > remainder)
				matchlen = remainder;
        }
    }
    huf_encode_end();
}
//

//ar
static char *usage =
	"Usage: ar command archive [file ...]\n"
	"Commands:\n"
	"   a: Add files to archive (replace if present)\n"
	"   x: Extract files from archive\n"
	"   r: Replace files in archive\n"
	"   d: Delete files from archive\n"
	"   p: Print files on standard output\n"
	"   l: List contents of archive\n"
	"If no files are named, all files in archive are processed,\n"
	"   except for commands 'a' and 'd'.\n"
	"You may copy, distribute, and rewrite this program freely.\n";

static uint ratio(ulong a, ulong b) //[(1000a + [b/2]) / b] 
{
	int i;

	for (i = 0; i < 3; i++)
		if (a <= ULONG_MAX / 10) 
			a *= 10;  
		else 
			b /= 10;
	if ((ulong)(a + (b >> 1)) < a) 
	{ 
		a >>= 1;
		b >>= 1; 
	}
	if (b == 0) 
		return 0;
	return (uint)((a + (b >> 1)) / b);
}

static void put_to_header(int i, int n, ulong x)
{
	while (--n >= 0) 
	{
		header[i++] = (uchar)((uint)x & 0xFF);  x >>= 8;
	}
}

static ulong get_from_header(int i, int n)
{
	ulong s;

	s = 0;
	while (--n >= 0) 
		s = (s << 8) + header[i + n];  //little endian 
	return s;
}

static uint calc_headersum(void)
{
	int i;
	uint s;

	s = 0;
	for (i = 0; i < headersize; i++) 
		s += header[i];
	return s & 0xFF;
}

static int read_header(void)
{
	headersize = (uchar) fgetc(arcfile);
	if (headersize == 0) 
		return 0;  //end of archive 
	headersum  = (uchar) fgetc(arcfile);
	fread_crc(header, headersize, arcfile);  //CRC not used 
	if (calc_headersum() != headersum) 
		error("Header sum error");
	compsize = get_from_header(5, 4);
	origsize = get_from_header(9, 4);
	file_crc = (uint)get_from_header(headersize - 5, 2);
	filename[namelen] = '\0';
	return 1;  //success 
}

static void write_header(void)
{
	fputc(headersize, outfile);
	//We've destroyed file_crc by null-terminating filename. 
	put_to_header(headersize - 5, 2, (ulong)file_crc);
	fputc(calc_headersum(), outfile);
	fwrite_crc(header, headersize, outfile);  //CRC not used 
}

static void skip(void)
{
	fseek(arcfile, compsize, SEEK_CUR);
}

static void copy(void)
{
	uint n;

	write_header();
	while (compsize != 0) 
	{
		n = (uint)((compsize > DICSIZ) ? DICSIZ : compsize);
		if (fread ((char *)buffer, 1, n, arcfile) != n)
			error("Can't read");
		if (fwrite((char *)buffer, 1, n, outfile) != n)
			error("Can't write");
		compsize -= n;
	}
}

static void store(void)
{
	uint n;

	origsize = 0;
	crc = INIT_CRC;
	while ((n = fread((char *)buffer, 1, DICSIZ, infile)) != 0) 
	{
		fwrite_crc(buffer, n, outfile);  
		origsize += n;
	}
	compsize = origsize;
}

static int add(int replace_flag)
{
	long headerpos, arcpos;
	uint r;

	if ((infile = fopen(filename, "rb")) == NULL) 
	{
		fprintf(stderr, "Can't open %s\n", filename);
		return 0;  /* failure */
	}
	if (replace_flag) 
	{
		printf("Replacing %s ", filename);  
		skip();
	} 
	else
		printf("Adding %s ", filename);
	headerpos = ftell(outfile);
	namelen = strlen(filename);
	headersize = 25 + namelen;
	memcpy(header, "-lh5-", 5);  /* compress */
	write_header();  /* temporarily */
	arcpos = ftell(outfile);
	origsize = compsize = 0; 
	unpackable = 0;
	crc = INIT_CRC; 
	encode();
	if (unpackable) 
	{
		header[3] = '0';  /* store */
		rewind(infile);
		fseek(outfile, arcpos, SEEK_SET);
		store();
	}
	file_crc = crc ^ INIT_CRC;
	fclose(infile);
	put_to_header(5, 4, compsize);
	put_to_header(9, 4, origsize);
	memcpy(header + 13, "\0\0\0\0\x20\x01", 6);
	memcpy(header + headersize - 3, "\x20\0\0", 3);
	fseek(outfile, headerpos, SEEK_SET);
	write_header();  /* true header */
	fseek(outfile, 0L, SEEK_END);
	r = ratio(compsize, origsize);
	printf(" %d.%d%%\n", r / 10, r % 10);
	return 1;  /* success */
}

int get_line(char *s, int n)
{
	int i, c;

	i = 0;
	while ((c = getchar()) != EOF && c != '\n')
		if (i < n) s[i++] = (char)c;
	s[i] = '\0';
	return i;
}

static void extract(int to_file)
{
	int n, method;
	uint ext_headersize;

	if (to_file) 
	{
		while ((outfile = fopen(filename, "wb")) == NULL) 
		{
			fprintf(stderr, "Can't open %s\nNew filename: ", filename);
			if (get_line(filename, FNAME_MAX) == 0) 
			{
				fprintf(stderr, "Not extracted\n");
				skip();  
				return;
			}
			namelen = strlen(filename);
		}
		printf("Extracting %s ", filename);
	} 
	else
	{
		outfile = stdout;
		printf("===== %s =====\n", filename);
	}
	crc = INIT_CRC;
	method = header[3];  
	header[3] = ' ';
	if (! strchr("045", method) || memcmp("-lh -", header, 5)) 
	{
		fprintf(stderr, "Unknown method: %u\n", method);
		skip();
	} 
	else 
	{
		ext_headersize = (uint)get_from_header(headersize - 2, 2);
		while (ext_headersize != 0) 
		{
			fprintf(stderr, "There's an extended header of size %u.\n", ext_headersize);
			compsize -= ext_headersize;
			if (fseek(arcfile, ext_headersize - 2, SEEK_CUR))
				error("Can't read");
			ext_headersize = fgetc(arcfile);
			ext_headersize += (uint)fgetc(arcfile) << 8;
		}
		crc = INIT_CRC;
		if (method != '0') 
			decode_start();
		while (origsize != 0) 
		{
			n = (uint)((origsize > DICSIZ) ? DICSIZ : origsize);
			if (method != '0') 
				decode(n, buffer);
			else 
				if (fread((char *)buffer, 1, n, arcfile) != n)
					error("Can't read");
			fwrite_crc(buffer, n, outfile);
			if (outfile != stdout) 
				putc('.', stderr);
			origsize -= n;
		}
	}
	if (to_file)
		fclose(outfile);  
	else 
		outfile = NULL;
	printf("\n");
	if ((crc ^ INIT_CRC) != file_crc)
		fprintf(stderr, "CRC error\n");
}

static void list_start(void)
{
	printf("Filename         Original Compressed Ratio CRC Method\n");
}

static void list(void)
{
	uint r;

	printf("%-14s", filename);
	if (namelen > 14) 
		printf("\n              ");
	r = ratio(compsize, origsize);
	printf(" %10lu %10lu %u.%03u %04X %5.5s\n",
		origsize, compsize, r / 1000, r % 1000, file_crc, header);
}

static int match(char *s1, char *s2)
{
	for ( ; ; ) 
	{
		while (*s2 == '*' || *s2 == '?') 
		{
			if (*s2++ == '*')
				while (*s1 && *s1 != *s2) s1++;
			else 
				if (*s1 == 0)
					return 0;
				else 
					s1++;
		}
		if (*s1 != *s2) 
			return 0;
		if (*s1 == 0) 
			return 1;
		s1++; 
		s2++;
	}
}

static int search(int argc, char *argv[])
{
	int i;

	if (argc == 3) 
		return 1;
	for (i = 3; i < argc; i++)
		if (match(filename, argv[i])) 
			return 1;
	return 0;
}

static void exitfunc(void)
{
	fclose(outfile); 
	remove(temp_name);
}
//
