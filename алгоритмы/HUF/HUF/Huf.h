#ifdef HUF_EXPORTS
#define HUF_API __declspec(dllexport) 
#else
#define HUF_API __declspec(dllimport) 
#endif

#include <stdio.h>
#include <stdlib.h>

namespace Huf
{
	#define VERBOSE // If defined, prints verbose program progress when it's running...

	short father[512], decomp_tree[512];
	unsigned short code[256], heap_length;
	unsigned long compress_charcount, file_size, heap[257];
	unsigned char code_length[256];
	long frequency_count[512];
	FILE *ifile, *ofile;

	class HufAlg
	{
	private:
		unsigned short generate_code_table();
		void build_code_tree();
		void build_initial_heap();
		void compress_image();
		void compression_report();
		void get_frequency_count();
		void reheap();

		void build_decomp_tree();
		void decompress_image();
	public:
		HufAlg(void);
		void Compress(int count, char *args[]);
		void Decompress(int count, char *args[]);
	};
}
