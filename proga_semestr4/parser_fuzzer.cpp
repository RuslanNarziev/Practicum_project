#include "parserSQL.h"

extern "C" int LLVMFuzzerTestOneInput (const uint8_t *Data, size_t Size)
{
	FILE *f = fopen ("fuzz", "w+");
	if (!f)
	{
		perror ("fopen");
		return 0;
	}
	fwrite (Data,1, Size, f);
	fclose (f);
	std::ifstream input ("fuzz");
	
	int c = 0;
	c = input.get ();
    std::string str;
	while (c != EOF)
	{
		str.clear ();
		while (c != '\n')
		{
			str += c;
			c = input.get ();
		}
    	str += '\n';
    	Parser A (str.data ());
    	std::cout << A.parse ();
		c = input.get ();
	}
    input.close ();
    return 0;
}
