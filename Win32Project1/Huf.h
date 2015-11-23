#include <stdio.h>
#include <stdlib.h>

namespace HuF
{
	#define VERBOSE // If defined, prints verbose program progress when it's running...

	class HufAlg
	{
	public:
		HufAlg(void);
		void Compress(int count, char *args[]);
		void Decompress(int count, char *args[]);
	};
}
