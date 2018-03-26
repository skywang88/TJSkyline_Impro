#include"OutxqxFile.h"
#include"../xqx/transxqx.h"
#include<string.h>
#include<stdlib.h>
#include<time.h>
FILE *LogFile;
char CityIndex[56][20]=
{
    "上海","广州","成都","昆明","深圳","西安","重庆","杭州","厦门","南京",
    "长沙","武汉","郑州","青岛","乌鲁木齐","海口","三亚","天津","哈尔滨",
    "大连","贵阳","沈阳","济南","福州","南宁","兰州","太原","长春",
    "温州","呼和浩特","南昌","宁波","合肥","石家庄","丽江","桂林","银川","珠海",
    "无锡","烟台","西宁","西双版纳","揭阳","泉州","拉萨","绵阳","晋江","大同","阿克苏",
	"伊宁","遵义","和田","喀什","惠州","安庆","包头"
};

char TypeIndex[4][5]=
{
    "E145","E190","A320","A330"
};


int TypeToindex(char *Te)
{
    int i;
    for(i=0;i<46;i++){
        if(!strcmp(Te,TypeIndex[i])){
            return i;
        }
    }
    return -1;
}

int CityToindex(char *Cy)
{
    int i;
    for(i=0;i<46;i++){
        if(!strcmp(Cy,CityIndex[i])){
            return i;
        }
    }
    return -1;
}

int TimetoIndex(char *Tx)
{
    int tsum=atoi(Tx);
    int houri=tsum/100;
    int mini=tsum%100;
    return (houri*60+mini)/5;
}



int write_log (FILE* pFile, int timet, const char *format, ...)
{
   va_list arg;
   int done;
    va_start (arg, format);
    //done = vfprintf (stdout, format, arg);

	if (timet == 1) {
		time_t time_log = time(NULL);
		struct tm* tm_log = localtime(&time_log);
		fprintf(pFile, "%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
	}
    done = vfprintf (pFile, format, arg);
    va_end (arg);

    fflush(pFile);
    return done;
}

void OutxqxFile(char *filename)
{
    int i,j;
    FILE *fp;
//    for(i=0;i<RecordNum;i++){
//        found=0;
//        for(xCi=0;xCi<xCsum;xCi++){
//            if(xqxConstraitindex[xCi*2]==routeRecord[i].PlanesType&&xqxConstraitindex[xCi*2+1]==routeRecord[i].AirlineArray[0][0]){
//                found=1;
//                break;
//            }
//        }
//        if(found==0){
//            xqxConstraitindex[xCsum*2]=routeRecord[i].PlanesType;
//            xqxConstraitindex[xCsum*2+1]=routeRecord[i].AirlineArray[0][0];
//       // printf("%d %d %d\n",xCsum,routeRecord[i].PlanesType,routeRecord[i].AirlineArray[0][0]);
//            xCsum++;
//        }
//    }

    fp=fopen(filename,"wb");
    fprintf(fp,"p %d %d\n",RecordNum,RecordNum*RecordNum);
    for(i=0;i<RecordNum;i++){
        for(j=0;j<RecordNum;j++){
            fprintf(fp,"%d %d %d\n",i+1,j+1,xqxMatrix[i][j]);
            //printf("%d %d %d\n",i+1,j+1,xqxMatrix[i][j]);
        }
    }

//    fprintf(fp,"%d\n",AirplaneTypeNum*baseNum);
//    for(i=0;i<RecordNum;i++){
//        fprintf(fp,"%d %d %d\n",i+1,j+1,xqxMatrix[i][j]);
//    }

//    for(i=0;i<RecordNum;i++){
//        fprintf(fp,"%d %d\n",routeRecord[i].PlanesType ,routeRecord[i],routeRecord[i].PlanesNum);
//    }
//    fprintf(fp,"%d\n",xCsum);
//    for(i=0;i<xCsum;i++){
//        fprintf(fp,"%d\t",BaseTypeN[xqxConstraitindex[i*2+1]][xqxConstraitindex[i*2]]);
//    }
//    fprintf(fp,"\n");
//
//    for(i=0;i<RecordNum;i++){
//        for(xCi=0;xCi<xCsum;xCi++){
//            if(xqxConstraitindex[xCi*2]==routeRecord[i].PlanesType&&xqxConstraitindex[xCi*2+1]==routeRecord[i].AirlineArray[0][0]){
//                break;
//            }
//        }
//        fprintf(fp,"%d %d\n",  xCi,routeRecord[i].PlanesNum);
//    }
    fclose(fp);
}

char *MyItoa(int timeint, char *timestring)
{
    timeint=timeint*5;
    int hour=timeint/60;
    int mini=timeint%60;
    if(hour<10){
        if(mini<10){
            sprintf(timestring,"0%d0%d",hour,mini);
        }
        else sprintf(timestring,"0%d%d",hour,mini);
    }
    else {
        if(mini<10){
            sprintf(timestring,"%d0%d",hour,mini);
        }
        else sprintf(timestring,"%d%d",hour,mini);
    }

    return timestring;
}

void printfRecord(struct solution *Nsol, int i)
{
	int j, k;
	char timestring[20];
	printf("****\n");
	//printf("%d\t %s\t %d\n",i,AirPlaneIndex[routeRecord[i].PlanesType],xqxMatrix[i][i]);
	if (Nsol->PlanesType > AirplaneTypeNum || Nsol->PlanesType < 0) {
		printf("\n");
	}
	printf("%d\t %d\t %d\t %d\n", i, Nsol->PlanesType, Nsol->PlanesNum, Nsol->solvalue);
	for (j = 0; j<Nsol->PlanesNum; j++) {
		printf("\n");
		for (k = 0; k<Nsol->LengthArray[j]; k++) {
			printf("%-20s\t", CityIndex[Nsol->AirlineArray[j][k * 4]]);
			printf("%-10s\t", MyItoa(Nsol->AirlineArray[j][k * 4 + 2], timestring));
			printf("%-20s\t", MyItoa(Nsol->AirlineArray[j][k * 4 + 3], timestring));
			printf("%s\n", CityIndex[Nsol->AirlineArray[j][k * 4 + 1]]);
			// printf("%s\t",AirportsIndex[Nsol->AirlineArray[j][k*4]]);
			//  printf("%s\t",AirportsIndex[Nsol->AirlineArray[j][k*4+1]]);
		}
		//  printf("\n");
	}
	printf("%llx\t %llx\t\n", Nsol->record, Nsol->solrecord);

	return;
}

int outFILEIn(char *filename, int NowrecordNum, int LastrecordNum)
{
    int i,j,k;
    FILE *fp;
    char timestring[20];
    fp=fopen(filename,"a+");
    //fprintf(fp,"%s\n",AirPlaneIndex[routeRecord[0].PlanesType]);
    for(i=LastrecordNum;i<NowrecordNum;i++){
        fprintf(fp,"****\n");
        //printf("%d\t %s\t %d\n",i,AirPlaneIndex[routeRecord[i].PlanesType],xqxMatrix[i][i]);
        fprintf(fp,"%d\t %d\t %d\n",i,routeRecord[i].PlanesType,routeRecord[i].solvalue);
        for(j=0;j<routeRecord[i].PlanesNum;j++){
                fprintf(fp,"\n");
                for(k=0;k<routeRecord[i].LengthArray[j];k++){
                   fprintf(fp,"%-20s\t",CityIndex[routeRecord[i].AirlineArray[j][k*4]]);
                   fprintf(fp,"%-10s\t",MyItoa(routeRecord[i].AirlineArray[j][k*4+2],timestring));
                   fprintf(fp,"%-20s\t",MyItoa(routeRecord[i].AirlineArray[j][k*4+3],timestring));
                   fprintf(fp,"%s\n",CityIndex[routeRecord[i].AirlineArray[j][k*4+1]]);
                  // printf("%s\t",AirportsIndex[routeRecord[i].AirlineArray[j][k*4]]);
                 //  printf("%s\t",AirportsIndex[routeRecord[i].AirlineArray[j][k*4+1]]);
                }
              //  printf("\n");
        }
    }
    fclose(fp);
    return NowrecordNum;
}



void outFile(char *filename)
{
    //输出机场名称 时间信息 10:00
    int i,j,k;
    FILE *fp;
    char timestring[20];
    fp=fopen(filename,"wb");
    for(i=0;i<RecordNum;i++){
        fprintf(fp,"****\n");
        //printf("%d\t %s\t %d\n",i,AirPlaneIndex[routeRecord[i].PlanesType],xqxMatrix[i][i]);
        fprintf(fp,"%d\t %d\t %d\n",i,routeRecord[i].PlanesType,routeRecord[i].solvalue);
        for(j=0;j<routeRecord[i].PlanesNum;j++){
                fprintf(fp,"\n");
                for(k=0;k<routeRecord[i].LengthArray[j];k++){
                   fprintf(fp,"%-20s\t",CityIndex[routeRecord[i].AirlineArray[j][k*4]]);
                   fprintf(fp,"%-10s\t",MyItoa(routeRecord[i].AirlineArray[j][k*4+2],timestring));
                   fprintf(fp,"%-20s\t",MyItoa(routeRecord[i].AirlineArray[j][k*4+3],timestring));
                   fprintf(fp,"%s\n",CityIndex[routeRecord[i].AirlineArray[j][k*4+1]]);
                  // printf("%s\t",AirportsIndex[routeRecord[i].AirlineArray[j][k*4]]);
                 //  printf("%s\t",AirportsIndex[routeRecord[i].AirlineArray[j][k*4+1]]);
                }
              //  printf("\n");
        }
    }
    fclose(fp);
}
