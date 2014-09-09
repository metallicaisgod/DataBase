#pragma once
namespace CDICommon
{
void key17_create_table(void);
void key17_calc(unsigned char *sz, unsigned long len, unsigned long *crc32);
unsigned long key17_str(char *sz, bool key);

} //end of namespace CDICommon
