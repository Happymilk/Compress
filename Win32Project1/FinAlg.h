#include <stdio.h>
#include <string.h>

namespace Fin
{
	// macro to calculate index in pctable from previous 2 characters
	#define INDEX(p1,p2) (((unsigned)(unsigned char)p1<<7)^(unsigned char)p2)

	class FinAlg
	{
	public:
		FinAlg(void);
		void Compress(FILE *pfIn, FILE *pfOut);
		void Decompress(FILE *pfin, FILE *pfout);
	};
}
