#include "..\..\..\utils\utils.h"
#include "utlbuffer.h"
#include "..\..\..\utils\hook\hooks.h"
#include "..\..\sdk\sdk.h"
CUtlBuffer::CUtlBuffer(int a1, int size, int a3)
{
	utils::construct_utl_buffer(this, a1, size, a3);
}

void CUtlBuffer::ensure(int size)
{
	utils::ensure_buffer_capacity(this, size);
}	
void CUtlBuffer::PutString(const char* szString)
{

	utils::put_utl_buffer_string(this, szString);
}

