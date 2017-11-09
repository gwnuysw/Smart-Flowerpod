# include <stdio.h>
# include <stdlib.h>
int main()
{
	int i = 0;
	int j ;
	int cur, start, end;
		char stringtest[20];
	char filename[20] = "filetest0.txt";		//파일 이름 

	FILE* stream = fopen(filename, "w");		//파일 오픈 
	if(stream == NULL)							//오픈 확인 
	{
		printf("file open error \n");
		exit(1);
	}

	while(i < 10)								//반복 
	{
		if(ftell(stream) > 100)				//각 파일의 크기 검사 
		{
			printf("new file opened \n");	
			if(fclose(stream) == EOF)			//크기를 벗어나면 파일닫음 
			{
				printf("file close error \n");
				exit(1);
			}
			
			filename[8]++;						//새로운 파일을 열기위한 단계 
			
			stream = fopen(filename, "w");		//새 파일 오픈 
			
			if(stream ==NULL)		//오픈 확인 
			{
				printf("file open error \n");
				exit(1);
			}
			i++;			//프로그램 종료 조건 ----뺴도 됨 
		}
		
		scanf("%d", &j);			//데이터 입력 ------------------------
		
		fprintf(stream,"test : %d ", j);	//파일 입력 
		
		end = ftell(stream);//--------------------------------------------------------
		
		printf("file end pointer loc : %d \n",end);//-----------------------
		
		if(j == -1)				//프로그램 종료조건 -------------------------------------
		{
			if(fclose(stream) == EOF)
			{
				printf("file close error \n");
				exit(1);
			}
			exit(0);
		}
	}
	

	exit(0);
}
