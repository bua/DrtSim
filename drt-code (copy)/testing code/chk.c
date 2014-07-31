

#include<stdio.h>
int main()
{
char *a[20],i,j,k,n=4;
a[0]= "0x402dc8";
a[1]= "0x402de8";
a[2]= "0x402de8";
a[3]= "0x4011f9";

printf("nUpdated array is  : ");
for(i=0;i<n;i++)
{
printf("  i = %d  \n",i);

   for(j=i+1;j<n;)
   {
printf(" j =%d \n",j);
       if(strcmp (a[j],a[i])== 0)
      {
         for(k=j;k<n;k++)
             a[k]=a[k+1];
          n--;
      }
      else
         j++;
   }
}

for(i=0;i<n;i++)
    printf("%s ",a[i]); 
return 0;
}




