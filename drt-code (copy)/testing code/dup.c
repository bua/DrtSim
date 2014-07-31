#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
const char  *ipTable[100],ip[10];
ip[0]="0x402de8";
ip[1]="0x14af010";
ip[2]="0x4011f9";
ip[3]="0x14af070";
ip[4]="0x4011f9";  
ip[5]="0x14af070";
ip[6]="0x4011f9";
int main(int argc, char **argv){

 
    fileRead();
    return 0;
}
fileRead(void)
{
int i=6,j=0,c=0;
while (c!= 0) /* read a line */
{
printf("Value of i is = %d \n", i);
ipTable[j]=ip[i];
printf("ipTable[i] %s  = ip %s \n", ipTable[i],ip);
i++;
printf("[i]= %d", i);
for(j=0;j<27;j++){
printf("Value is = %s ", ip[j]);
}
}
return 0;
}

