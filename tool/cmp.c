#include "public.h"

void main(){
	char c1,c2;
	long p;
	FILE *fp1,*fp2;
	if((fp1 = fopen("2.txt","r")) == NULL)
	{
		printf("ERROR!\n");
		exit(1);
	}
	if((fp2 = fopen("dl_2.txt","r")) == NULL)
	{
		printf("ERROR!\n");
		exit(1);
	}
	while(!feof(fp1))
	{
		p = ftell(fp1);
		c1 = fgetc(fp1);
		c2 = fgetc(fp2);
		if((c1!=c2)/*&&(c1==48)&&(p%DGRAM_SIZE==0)*/)
		{
			printf("%ld(%ld):%c(%d)-%c(%d)\n",p,p%DGRAM_SIZE,c1,c1,c2,c2);
		}
	}
	printf("%lu\n",sizeof(char));
	fclose(fp1);
	fclose(fp2);
}
