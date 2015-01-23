#include "public.h"
pthread_mutex_t mtx;
FILE *fp;
long offset=0;
void *send_thr(int *sck)
{
	int realsize;
	char *sbuff,seq[11];

	sbuff = (char *)malloc((DGRAM_SIZE+10)*sizeof(char));
	
	pthread_mutex_lock(&mtx);
	fseek(fp,offset,0);
	sprintf(sbuff,"%010ld",offset);
	realsize = fread(sbuff+10,sizeof(char),DGRAM_SIZE,fp);
	offset += DGRAM_SIZE;
	pthread_mutex_unlock(&mtx);

	while(realsize > 0)
	{
		send(*sck,sbuff,DGRAM_SIZE+10,0);

		pthread_mutex_lock(&mtx);
		fseek(fp,offset,0);
		sprintf(sbuff,"%010ld",offset);
		realsize = fread(sbuff+10,sizeof(char),DGRAM_SIZE,fp);
		offset += DGRAM_SIZE;
		pthread_mutex_unlock(&mtx);
	}
	memset(sbuff,0,10);
	send(*sck,sbuff,10,0);
	free(sbuff);
}

void main()
{
	struct sockaddr_in s_ad,c_ad;
	pthread_t pid[MAX_THREAD];
	int ser,cli[MAX_THREAD],len,i=0,realsize,thr_kill;
	char *buff,fname[30];
	
	len = sizeof(c_ad);
	pthread_mutex_init(&mtx,NULL);
	buff = (char *)malloc(DGRAM_SIZE*sizeof(char));
	
	ser = socket(PF_INET,SOCK_STREAM,0);
	
	memset(&s_ad,0,len);
	s_ad.sin_family = AF_INET;
	s_ad.sin_addr.s_addr = htonl(INADDR_ANY);
	s_ad.sin_port = htons(SERVER_PORT);
	
	bind(ser,(struct sockaddr *)&s_ad,sizeof(s_ad));
	listen(ser,10);
	printf("Waiting...\n");
	cli[0] = accept(ser,(struct sockaddr *)&c_ad,&len);
	
	realsize = recv(cli[0], buff, 40,0);
	if(realsize < 0)
	{
		perror("Receive File Name Failed:");
		exit(1);
	}
	strncpy(fname,buff,realsize);
	
	printf("%s\n",fname);
	if((fp = fopen(fname,"r")) == NULL)
	{
		printf("ERROR!\n");
		exit(1);
	}
	printf("File open:OK\n");
	
	for(i=0;i<MAX_THREAD;i++)
	{
		if(i!=0)
		{
			cli[i] = accept(ser,(struct sockaddr *)&c_ad,&len);
		}
		if(pthread_create(pid+i,NULL,(void *)send_thr,(int *)(cli+i))!=0)
		{
			perror("pthread_create");
		}
	}
	
	i = 0;
	while(i<MAX_THREAD)
	{
		thr_kill = pthread_kill(pid[i],0);
		if(thr_kill==ESRCH)
		{
			i++;
		}
	}
	printf("End!\n");

	fclose(fp);
	for(i=0;i<MAX_THREAD;i++) close(cli[i]);
	close(ser);
	free(buff);
}

