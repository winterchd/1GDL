#include "public.h"
#include "time.h"
pthread_mutex_t mtx;
FILE *fp;
void *recv_thr(int *sck)
{
	int realsize,i,sum,rws;
	long offset;
	char *rbuff,seq[11],cc;
	
	rbuff = (char *)malloc((DGRAM_SIZE)*sizeof(char));
	
	realsize = recv(*sck,rbuff,10,0);
	while(*rbuff != 0)
	{
		strncpy(seq,rbuff,10);
		strcat(seq,"\0");
		offset = atol(seq);
		sum = 0;
		while(sum<DGRAM_SIZE)
		{
			realsize = recv(*sck,rbuff+sum,DGRAM_SIZE-sum,0);
			sum += realsize;
		}
		pthread_mutex_lock(&mtx);
		fseek(fp,offset,0);
		cc = fgetc(fp);
		if(cc>=48) printf("%lu:(%s)%ld\n",pthread_self(),seq,offset);
		fseek(fp,offset,0);
		rws = fwrite(rbuff,1,DGRAM_SIZE,fp);
		pthread_mutex_unlock(&mtx);
		realsize = recv(*sck,rbuff,10,0);
	}
	free(rbuff);
}

void main()
{
	int i=0,n;
	struct sockaddr_in s_ad;
	pthread_t pid[MAX_THREAD];
	int len,sck[MAX_THREAD],thr_kill;
	long got=0;
	char ip[30]="127.0.0.1",fname[40],fn[40];
	time_t t_start,t_end;

	len = sizeof(s_ad);
	pthread_mutex_init(&mtx,NULL);
	for(i=0;i<MAX_THREAD;i++)
	{
		sck[i] = socket(PF_INET,SOCK_STREAM,0);
	}
	memset(&s_ad,0,sizeof(s_ad));
	s_ad.sin_family = AF_INET;
	inet_aton(ip,&s_ad.sin_addr);
	s_ad.sin_port = htons(SERVER_PORT);

	connect(sck[0],(struct sockaddr *)&s_ad,len);
	printf("Filename:");
	scanf("%s",fname);

	t_start=time(NULL);

	if(send(sck[0], fname, strlen(fname)+1,0) < 0)
	{
		perror("Send File Name Failed:");
		exit(1);
	}
	
	sprintf(fn,"%s","dl_");
	strcat(fn,fname);
	if((fp = fopen(fn,"w+")) == NULL)
	{
		printf("ERROR!\n");
		exit(1);
	}

	for(i=0;i<MAX_THREAD;i++)
	{
		if(i!=0)
		{
			connect(sck[i],(struct sockaddr *)&s_ad,len);
		}
		if(pthread_create(pid+i,NULL,(void *)recv_thr,(sck+i))!=0)
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

	t_end=time(NULL);
	printf("%.0fs\n",difftime(t_end,t_start));

	for(i=0;i<MAX_THREAD;i++) close(sck[i]);
	fseek(fp,0,2);
	fclose(fp);
}

