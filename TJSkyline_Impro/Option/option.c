#include"Option.h"
#include"../route/CreateRoute.h"
#include"../IO/OutxqxFile.h"
#include"../xqx/transxqx.h"



void Readinformation(char *fn)
{
    FILE *fp=fopen(fn,"rb");
    int ri;
    int i=-1;
    char a[1000];
    RecordNum=0;
    while(!feof(fp))
    {
        fgets(a,1000,fp);
        if(!strncmp(a,"****",4)){
            i++;
            RecordNum++;
            fscanf(fp,"%d %d %d ",&ri,&routeRecord[i].PlanesType,&routeRecord[i].solvalue);
            routeRecord[i].PlanesNum=1;
            int j=0;
            if(ri!=i){
                printf("%d %d\n",i,ri);
                exit(-1);
            }
            char strline[1000];
            int LastEnd=0;
            int NowEnd;
            char cityOne[20];
            char cityTwo[20];
            char timeOne[20];
            char timeTwo[20];
            while(!feof(fp)){
                fgets(strline,1000,fp);
                if(!strncmp(strline,"****",4)){
                    fseek(fp,-6,SEEK_CUR);
                    break ;
                }
                if(strlen(strline)<=2){
                    continue;
                }
                sscanf(strline,"%s %s %s %s",cityOne,timeOne,timeTwo,cityTwo);
                if((NowEnd=TimetoIndex(timeTwo))<LastEnd){
                    j++;
                    routeRecord[i].PlanesNum++;
                }
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4]=CityToindex(cityOne);
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+1]=CityToindex(cityTwo);
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+2]=TimetoIndex(timeOne);
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+3]=TimetoIndex(timeTwo);
                LastEnd=NowEnd;
                routeRecord[i].LengthArray[j]++;
            }
        }

    }

    printf("%d\n",RecordNum);
    fclose(fp);
}


void ReadNowSettingTxt()
{
    FILE *fp;
    fp=fopen("nowSetting.txt","rb");
    char strline[1000];
    char a[1000];

	int i=0;

    int ri;
    fscanf(fp,"%d %d %d ",&ri,&routeRecord[i].PlanesType,&routeRecord[i].PlanesNum);
   // printf("%d information :%d %d %d\n",i,ri,routeRecord[i].PlanesType,routeRecord[i].PlanesNum);
    int j=0;
    int LastEnd=0;
    int NowEnd;
    char cityOne[20];
    char cityTwo[20];
    char timeOne[20];
    char timeTwo[20];
    while(!feof(fp))
    {
        fgets(a,1000,fp);
        if(!strncmp(a,"****",9)){
            i++;
            while(!feof(fp)){
                fgets(strline,1000,fp);
                if(!strncmp(strline,"****",4)){
                    fseek(fp,-6,SEEK_CUR);
                    return ;
                }

                sscanf(strline,"%s %s %s %s",cityOne,timeOne,timeTwo,cityTwo);
                if((NowEnd=TimetoIndex(timeTwo))<LastEnd)
                    j++;
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4]=CityToindex(cityOne);
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+1]=CityToindex(cityTwo);
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+2]=TimetoIndex(timeOne);
                routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+3]=TimetoIndex(timeTwo);
                LastEnd=NowEnd;
              //  printf("%d %d information: %d %d %d %d\n", i,j,routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4],routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+1],routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+2],routeRecord[i].AirlineArray[j][routeRecord[i].LengthArray[j]*4+3]);
                routeRecord[i].LengthArray[j]++;

            }
        }
        RecordNum++;
    }


}

void ReAddAirlineTxt()
{
    FILE *fp;
    int a,k;
    int i;
    fp=fopen("resetting.txt","rb");
    char strline[1000];
    fscanf(fp,"%s",strline);
    fscanf(fp,"%d",&a);
    if(a!=-1){
        a*=12;
    }
    int nowRecordNum=RecordNum;
    int begin_airports[2];
    for(k=0;k<AirplaneTypeNum-1;k++){
        for(i=0;i<AirportsNum;i++){
            if(APT[i].isBase==0)
                continue;
            if(TypeBaseN[k][i]!=0){
                begin_airports[0]=begin_airports[1]=i;
                CreateRoute(k,begin_airports,1,1,CityToindex(strline),a);
            }
        }
        for(i=0;i<AirportsNum;i++){
            free(AirportsTimeStart[i]);
            free(AirportsTimeEnd[i]);
        }
        free(AirportsTimeStart);
        free(AirportsTimeEnd);
    }
    for(i=nowRecordNum;i<RecordNum;i++){
        printfRecord(&routeRecord[i],i);
    }
    RecordNum=0;
    fclose(fp);
}


void ReadSettingTxt()
{
    FILE *fp;
    int *involeAirports=(int *)malloc(sizeof(int)*AirportsNum);
    memset(involeAirports,0,sizeof(int)*AirportsNum);
    int a;
    int Nrnum=RecordNum;
    fp=fopen("setting.txt","rb");
    int TypeK;
    int begin_airports[2];
    char strline[1000];
	char strinf[80];
	while (1) {
		fgets(strline, 1000, fp);
		if (strline[0] != '/' || strline[1] != '/')
			break;
	}
	sscanf(strline, "%s", strinf);
    TypeK=TypeToindex(strinf);

    int PortsNum;
	while (1) {
		fgets(strline, 1000, fp);
		if (strline[0] != '/' || strline[1] != '/')
			break;
	}
    sscanf(strline,"%d",&PortsNum);

    int i;
    for(i=0;i<PortsNum;i++){
		while (1) {
			fgets(strline, 1000, fp);
			if (strline[0] != '/' || strline[1] != '/')
				break;
		}
		sscanf(strline,"%s", strinf);
        involeAirports[CityToindex(strinf)]=1;
    }


	while (1) {
		fgets(strline, 1000, fp);
		if (strline[0] != '/' || strline[1] != '/')
			break;
	}
    int apnum;
    sscanf(strline,"%d",&apnum);

    for(i=0;i<apnum;i++){
		while (1) {
			fgets(strline, 1000, fp);
			if (strline[0] != '/' || strline[1] != '/')
				break;
		}
        sscanf(strline,"%s", strinf);
        begin_airports[i]=CityToindex(strinf);
    }

    int atime;
	while (1) {
		fgets(strline, 1000, fp);
		if (strline[0] != '/' || strline[1] != '/')
			break;
	}
    sscanf(strline,"%s",strinf);
    a=CityToindex(strinf);
	while (1) {
		fgets(strline, 1000, fp);
		if (strline[0] != '/' || strline[1] != '/')
			break;
	}
    sscanf(strline,"%d",&atime);
    if(atime>=0)
        atime*=12;

    Timeset(TypeK);
    if(FindBestmodel(TypeK,begin_airports,apnum,involeAirports,a,atime)<1){
        printf("no solution found\n");
    }
    else {
        printfRecord(&routeRecord[Nrnum],Nrnum);
    }
    int mmm=0;
    //int j,k;
//    for(j=0;j<routeRecord[Nrnum].PlanesNum;j++){
//            for(k=0;k<routeRecord[Nrnum].LengthArray[j];k++){
//                mmm+=ValueMatrix[routeRecord[Nrnum].PlanesType][routeRecord[Nrnum].AirlineArray[j][k*4]][routeRecord[Nrnum].AirlineArray[j][k*4+1]][routeRecord[Nrnum].AirlineArray[j][k*4+2]];
//                printf("%d %d %d %d %d\n",routeRecord[Nrnum].PlanesType,routeRecord[Nrnum].AirlineArray[j][k*4],routeRecord[Nrnum].AirlineArray[j][k*4+1],routeRecord[Nrnum].AirlineArray[j][k*4+2],ValueMatrix[routeRecord[Nrnum].PlanesType][routeRecord[Nrnum].AirlineArray[j][k*4]][routeRecord[Nrnum].AirlineArray[j][k*4+1]][routeRecord[Nrnum].AirlineArray[j][k*4+2]]);
//            }
//        }
//            printf("%d %d\n",mmm,routeRecord[Nrnum].solvalue);
    RecordNum=Nrnum;
    fclose(fp);
	free(involeAirports);
}



void CRoute()
{
    int i,j,k;
    int BeAirports[2];
    int LastRecordNum=RecordNum;
    int Timelong=-1;
        k=2;
//    for(k=1;k<AirplaneTypeNum-1;k++){
        Timeset(k);


        for(i=0;i<AirportsNum;i++){
            if(APT[i].isBase==0)
                continue;
            if(TypeBaseN[k][i]!=0){
                BeAirports[0]=BeAirports[1]=i;
                CreateRoute1(k,BeAirports,1,TypeBaseN[k][i]*10,-1,Timelong);
                write_log(LogFile,1,"1route %d %d success\n",k,i);
                //printf("1route %d %d success\n",k,i);
                LastRecordNum=outFILEIn("airline.txt",RecordNum,LastRecordNum);
            }
        }

        for(i=0;i<AirportsNum;i++){
            if(APT[i].isBase==0)
                continue;
            if(TypeBaseN[k][i]==0){
                continue;
            }
            for(j=i+1;j<AirportsNum;j++){
                if(APT[j].isBase==0)
                    continue;
                if(TypeBaseN[k][j]!=0){
                    BeAirports[0]=i;
                    BeAirports[1]=j;
                    CreateRoute1(k,BeAirports,2,Minner(TypeBaseN[k][i],TypeBaseN[k][j])*10,-1,Timelong);
                   // printf("3route %d %d %dsuccess\n",k,i,j);
                    write_log(LogFile,1,"3route %d %d %d success\n",k,i,j);
                    LastRecordNum=outFILEIn("airline.txt",RecordNum,LastRecordNum);
        //            CreateRoute(j,BeAirports,2,BaseTypeN[i][j]);
        //            printf("2route %d %d success\n",i,j);
                }
            }
        }
		//for (i = 0; i<AirportsNum; i++) {
		//	if (APT[i].isBase == 0)
		//		continue;
		//	if (BaseTypeN[i][k] != 0) {
		//		BeAirports[0] = BeAirports[1] = i;
		//		CreateRoute1(k, BeAirports, 2, BaseTypeN[i][k] * 20, -1, Timelong);
		//		write_log(LogFile, "2route %d %d success\n", k, i);
		//		// printf("2route %d %d success\n",k,i);
		//		LastRecordNum = outFILEIn("airline.txt", RecordNum, LastRecordNum);
		//	}
		//}
//    }
    printf("success\n");

}

int supplement()
{
	ChangeTimeWindows();
	CRoute();
	recoverTimewindow();
}

