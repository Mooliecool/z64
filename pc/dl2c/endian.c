#include <stdio.h>
int main(){char buff[4]="\x00\x01\x02\x03";int*val=(int*)buff;printf("%02X%02X%02X%02X->%08X\n",buff[0],buff[1],buff[2],buff[3],*val);return 0;}
