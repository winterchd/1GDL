#include <stdio.h>
#define N 128
void main(){
	FILE *fp;
	char word[16]="0123456789ABCDEF";
	int i,j,c,n;
	if((fp=fopen("test.txt","w+"))==NULL){
		printf("ERROR!\n");
		exit(1);
	}
	for(c=0;c<N;c++){
		for(i=0;i<1024*64;i++){
			for(j=0;j<16;j++){
				fputc(word[j],fp);
			}
		}
		printf("%dM done!\n",(c+1));
	}
	fclose(fp);
}
