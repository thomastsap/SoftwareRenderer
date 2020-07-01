#include "helper_functions.h"

void
swap(int32 *value1, int32 *value2)
{
	int32 temp = *value1;
	*value1 = *value2;
	*value2 = temp;
}

uint32
RoundReal32ToUInt32(real32 Real32)
{
	uint32 Result = (uint32)(Real32 + 0.5f);
	return(Result);
}