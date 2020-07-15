#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "header.h"

inline void
swap(int32 *value1, int32 *value2)
{
	Assert(value1 && value2);
	int32 temp = *value1;
	*value1 = *value2;
	*value2 = temp;
}

inline int32
RoundReal32ToInt32(real32 Real32)
{
	Assert(Real32 <= INT32_MAX);
	int32 Result = (int32)(Real32 + 0.5f);
	return(Result);
}

inline uint32
RoundReal32ToUInt32(real32 Real32)
{
	Assert(Real32 <= UINT32_MAX);
	uint32 Result = (uint32)(Real32 + 0.5f);
	return(Result);
}

inline uint32
SafeTruncateUInt64(uint64 Value)
{
	Assert(Value <= UINT32_MAX);
	uint32 Result = (uint32)Value;
	return(Result);
}
#endif 

