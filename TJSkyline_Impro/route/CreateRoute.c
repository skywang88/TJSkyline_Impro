#include"CreateRoute.h"
#include"..\IO\OutxqxFile.h"
#include<stdlib.h>

double bestsolval;
double lastsolval;
int RecordNum;
struct solution *routeRecord;

__int64 SupportRecord[10000];	
int NRecordNum;	
int supportRecordNum;
int *AirportsRecord;

double Timelimit;	
int arNum;
int ***AirportsTimeStart;	/*机场最早到达时间，AirportsTimeStart[k][i][j], 机型k机场i 机场j 由i出发最早到达j的时间*/
int ***AirportsTimeEnd;		/*机场最晚起飞时间,AirportsTimeEnd[k][i][j] 机型k 机场i 机场j   最晚由i出发到达j的时间*/
int MaxBeginTime;
int MaxEndTime;
int LongestFlightTime;
int **StaticAirports;
int **AirportsGroup;
int *StaticGroup;
int **AirportsNeighboor;
int	changerecord[10000];
int changerecordNum;
int UnfindNum;
const int GroupNum=3;
int AddAirportsNum;
struct randomStruct 
{
	int weightSum;
	int *WeightAirports;
};
struct randomStruct AllAirportsWeight;

//void MyfindSolution(int )
//{
//
//}

int timeWindowEnough(int airportsi, int Twnj, int direction, int Krange)
{
	if (APT[airportsi].TimeWindows_hour[Twnj * 4] <= 72 || APT[airportsi].TimeWindows_hour[Twnj * 4] >= 244)
		return 1;
	if (APT[airportsi].TimeWindows_hour[Twnj * 4 + 2 + direction] * Krange > StaticAirports[airportsi][Twnj*2+direction]) {
		return 0;
	}
	if (APT[airportsi].TimeWindows_hour[Twnj * 4 + 2 + direction] >0 
		&&APT[airportsi].TimeWindows_hour[Twnj * 4 + 2 + direction] * Krange < StaticAirports[airportsi][Twnj * 2 + direction]) {
		return 2;
	}
	return 1;
}

int BanTimeWindows(int airportsi, int Twnj, int direction)
{
	changerecord[changerecordNum * 4] = airportsi;
	changerecord[changerecordNum * 4 + 1] = Twnj;
	changerecord[changerecordNum * 4 + 2] = direction;
	changerecord[changerecordNum * 4 + 3] = APT[airportsi].TimeWindows_hour[Twnj * 4 + 2 + direction];
	APT[airportsi].TimeWindows_hour[Twnj * 4 + 2 + direction] = 0;
	changerecordNum++;
	return 1;
}

int recoverTimewindow()
{
	int i;
	int airportsi;
	int Twnj;
	int direction;
	for (i = 0; i < changerecordNum; i++) {
		airportsi = changerecord[i * 4];
		Twnj = changerecord[i * 4 + 1];
		direction = changerecord[i * 4 + 2];
		APT[airportsi].TimeWindows_hour[Twnj * 4 + 2 + direction] = changerecord[i * 4 + 3];
	}
	changerecordNum = 0;
	for (i = 0; i < AirportsNum; i++) {
		AllAirportsWeight.WeightAirports[i] = 1;
	}
	AllAirportsWeight.weightSum = AirportsNum;
}

void ChangeTimeWindows()
{
	int i,j;
	
    
	int krange = 2;
	int direction = 0;
	for (i = 0; i < AirportsNum; i++) {
		if (BanAirports(i))
			continue;
		if (APT[i].Twn_hour <= 0)
			continue;
		for (j = 0; j < APT[i].Twn_hour; j++) {
			if (APT[i].TimeWindows_hour[j * 4] <= 72 || APT[i].TimeWindows_hour[j * 4]>=252) {
				continue;
			}
			for (direction = 0; direction < 2; direction++) {
				if (!timeWindowEnough(i, j, direction, krange))
				{
					AllAirportsWeight.WeightAirports[i]++;
					AllAirportsWeight.weightSum++;
					if (j > 0 && APT[i].TimeWindows_hour[j * 4-4] >= 84 && APT[i].TimeWindows_hour[j * 4-4] <= 240) {
						if (timeWindowEnough(i, j - 1, direction, krange)==2) {
							BanTimeWindows(i, j - 1, direction);
						}
					}
					if (j < APT[i].Twn_hour-1 && APT[i].TimeWindows_hour[j * 4 + 4] >= 84 && APT[i].TimeWindows_hour[j * 4 + 4] <= 240) {
						if (timeWindowEnough(i, j + 1, direction, krange)==2) {
							BanTimeWindows(i, j + 1, direction);
						}
					}
				}
			}
		}
	}
}
int TransIntergerToAirports(__int64 x, int *involveAirports)
{
	__int64 a = 0;
	__int64 k = 1;
	int i;

	for (i = 0; i < AirportsNum; i++,k<<=1) {
		if (x&k) {
			involveAirports[i] = 1;
			a++;
		}
	}
	return a;
}

__int64 TransAirportsToInterger(int *involveAirports)
{
	__int64 x = 0;
	__int64	k = 1;
	int i;

	for (i = 0; i < AirportsNum; i++,k<<=1) {
		if (involveAirports[i] == 1) {
			x = (x | k);
		}
	}
	return x;
}
void simpleOptimizeS(int AirPlaneTypei, int Airportsi)
{
	int i, j;
	int *visit;
	int *pre;
	visit = (int *)malloc(sizeof(int)*AirportsNum);
	pre = (int *)malloc(sizeof(int) *AirportsNum);
	for (i = 0; i<AirportsNum; i++) {
		visit[i] = 0;
		if (i == Airportsi) {
			AirportsTimeStart[AirPlaneTypei][Airportsi][i] = 0;
			visit[i] = 1;
		}
		else AirportsTimeStart[AirPlaneTypei][Airportsi][i] = RTime[AirPlaneTypei][Airportsi][i] - 1;
		pre[i] = Airportsi;

		//printf("%d %d\n",i,AirportsTimeStart[i]);
	}
	//for (i = 0; i < AirportsNum; i++) {
	//	printf("%d %d\n", i, APT[i].PassTime[AirPlaneTypei]);
	//}
	//    for(i=0;i<AirportsNum;i++){
	//        printf("s %d:%d %d\n",i,AirportsTimeStart[Airportsi][i],pre[i]);
	//    }

	for (i = 1; i<AirportsNum; i++) {
		int mintime = MaxTime, pos = Airportsi;
		for (j = 0; j<AirportsNum; j++) {
			if (!visit[j] && AirportsTimeStart[AirPlaneTypei][Airportsi][j]<mintime) {
				pos = j;
				mintime = AirportsTimeStart[AirPlaneTypei][Airportsi][j];
			}
		}
		visit[pos] = 1;
		for (j = 0; j<AirportsNum; j++)
		{
			if (!visit[j] && AirportsTimeStart[AirPlaneTypei][Airportsi][j]>AirportsTimeStart[AirPlaneTypei][Airportsi][pos] + APT[pos].PassTime[AirPlaneTypei] + RTime[AirPlaneTypei][pos][j] - 1)
			{
				AirportsTimeStart[AirPlaneTypei][Airportsi][j] = AirportsTimeStart[AirPlaneTypei][Airportsi][pos] + APT[pos].PassTime[AirPlaneTypei] + RTime[AirPlaneTypei][pos][j] - 1;
				pre[j] = pos;//记录其前驱
			}
		}

		
		//        for(i=0;i<AirportsNum;i++){
		//            printf("s %d:%d %d",i,AirportsTimeStart[Airportsi][i],pre[i]);
		//        }
	}
	//for (i = 0; i<AirportsNum; i++) {
	//	printf("%d %d %d %d\n", i, pre[i], AirportsTimeStart[AirPlaneTypei][Airportsi][i], RTime[AirPlaneTypei][Airportsi][i] - 1);
	//}
	//printf("\n");
	free(pre);
	free(visit);

}

void simpleOptimizeE(int AirPlaneTypei, int Airportsj)
{
	int i, j;
	int *visit;
	int *pre;
	visit = (int *)malloc(sizeof(int)*AirportsNum);
	pre = (int *)malloc(sizeof(int) *AirportsNum);
	for (i = 0; i<AirportsNum; i++) {
		visit[i] = 0;
		if (i == Airportsj) {
			AirportsTimeEnd[AirPlaneTypei][Airportsj][i] = 0;
			visit[i] = 1;
		}
		else AirportsTimeEnd[AirPlaneTypei][Airportsj][i] = RTime[AirPlaneTypei][i][Airportsj] - 1;
		pre[i] = Airportsj;


		// printf("%d %d\n",i,AirportsTimeEnd[i]);
	}
	for (i = 1; i<AirportsNum; i++) {
		int mintime = MaxTime, pos = Airportsj;
		for (j = 0; j<AirportsNum; j++) {
			if (!visit[j] && AirportsTimeEnd[AirPlaneTypei][Airportsj][j]<mintime) {
				pos = j;
				mintime = AirportsTimeEnd[AirPlaneTypei][Airportsj][j];
			}
		}
		visit[pos] = 1;
		for (j = 0; j<AirportsNum; j++)
		{
			if (!visit[j] && AirportsTimeEnd[AirPlaneTypei][Airportsj][j]>AirportsTimeEnd[AirPlaneTypei][Airportsj][pos] + APT[pos].PassTime[AirPlaneTypei] + RTime[AirPlaneTypei][j][pos] - 1)
			{
				AirportsTimeEnd[AirPlaneTypei][Airportsj][j] = AirportsTimeEnd[AirPlaneTypei][Airportsj][pos] + APT[pos].PassTime[AirPlaneTypei] + RTime[AirPlaneTypei][pos][j] - 1;
				pre[j] = pos;//记录其前驱
			}
		}
	}
	free(visit);
	free(pre);

	//    for(i=0;i<AirportsNum;i++){
	//        printf("%d %d %d %d\n",i,pre[i], AirportsTimeEnd[i],RTime[AirPlaneTypei][i][Airportsj]-1);
	//    }
	//    printf("\n");
}

void simpleOptimize()
{
	int i, j, k;
	AirportsTimeStart = (int ***)malloc(sizeof(int**)*AirplaneTypeNum);
	AirportsTimeEnd = (int ***)malloc(sizeof(int**)*AirplaneTypeNum);
	for (i = 0; i<AirplaneTypeNum; i++) {
		AirportsTimeStart[i] = (int **)malloc(sizeof(int *)*AirportsNum);
		AirportsTimeEnd[i] = (int **)malloc(sizeof(int *)*AirportsNum);
	}

	for (i = 0; i<AirplaneTypeNum; i++) {
		for (j = 0; j<AirportsNum; j++) {
			if (TypeBaseN[i][j] != 0) {
				AirportsTimeStart[i][j] = (int *)malloc(sizeof(int)*AirportsNum);
				AirportsTimeEnd[i][j] = (int *)malloc(sizeof(int)*AirportsNum);
			}
		}
	}

	for (i = 0; i<AirplaneTypeNum; i++) {
		for (j = 0; j<AirportsNum; j++) {

			if (TypeBaseN[i][j] != 0) {
				if (i == 2 && j == 14) {
					printf("%d",i);
				}
				simpleOptimizeS(i, j);
				simpleOptimizeE(i, j);
			}
		}
	}

	for (k = 0; k<AirplaneTypeNum; k++) {
		for (i = 0; i<AirportsNum; i++) {
			for (j = 0; j<AirportsNum; j++) {
				if (TypeBaseN[k][i] != 0) {
					AirportsTimeStart[k][i][j] = Dbegin + AirportsTimeStart[k][i][j];
					AirportsTimeEnd[k][i][j] = Dend - AirportsTimeEnd[k][i][j];
				}
			}
		}
	}


	//    for(i=0;i<AirportsNum;i++){
	////        printf("s:%d:%d\n",i,AirportsTimeStart[29][i]);
	////        printf("e:%d:%d\n",i,AirportsTimeEnd[29][i]);
	//    }
}
//void modelPrepare()
//{
//
//}


int initialAirportGroup()
{
    int i,j;
    for(i=0;i<GroupNum;i++){
        AirportsGroup[i][0]=8;
        for(j=1;j<AirportsGroup[i][0];j++){
            do{
                AirportsGroup[i][j]=rand()%AirportsNum;
            }while(BanAirports(AirportsGroup[i][j]));
        }
    }
	return 1;
}



void BanAirlineModify(int point, int Mintime)
{
     int i,typei;
     if(UnlimitedAirports(point))
        return ;
     for(typei=0;typei<AirplaneTypeNum;typei++){
         for(i=0;i<26;i++){
            if(UnlimitedAirports(point))
                continue;
            if(RTime[typei][i][point]<Mintime&&RTime[typei][i][point]<Mintime)
                BanAirline[typei][i][point]=BanAirline[typei][point][i]=1;
         }
     }
}

void MyHeapAjust(int *AirportsN, int Now, int length, int Airporti)
{
    int tmp=AirportsN[Now];
    int child=2*Now+1;
    while(child<length){
        if(child+1<length&& RTime[1][Airporti][AirportsN[child]]<RTime[1][Airporti][AirportsN[child+1]]){
            child++;
        }
        if(RTime[1][Airporti][AirportsN[child]]>RTime[1][Airporti][AirportsN[Now]]){
            AirportsN[Now]=AirportsN[child];
            Now=child;
            child=2*Now+1;
        }else {
            break;
        }
        AirportsN[Now]=tmp;
    }

}


void MyHeapSort(int *AirportN, int n, int Airporti)
{
    int i;
    int temp;
    for(i=(n-1)/2;i>=0;i--){
        MyHeapAjust(AirportN,i,n,Airporti);
    }

    for(i=1;i<n;i++){
        temp=AirportN[0];
        AirportN[0]=AirportN[n-i];
        AirportN[n-i]=temp;
        MyHeapAjust(AirportN,0,n-i,Airporti);
    }
}

int RandomByWeight()
{
	int i;
	int Nk=rand()% AllAirportsWeight.weightSum;
	int Nowsum = 0;
	//for (i = 0; i < AirportsNum; i++) {
	//	printf("%d\t%d\n", i, AllAirportsWeight.WeightAirports[i]);
	//}
	//printf("%d\n", AllAirportsWeight.weightSum);
	for (i = 0; i < AirportsNum; i++) {
		if (AllAirportsWeight.WeightAirports[i] != 0 ) {
			Nowsum += AllAirportsWeight.WeightAirports[i];
			if (Nowsum > Nk) {
				return i;
			}
		}
	}
}

void Prepare()
{
    int i=0,j=0;
    RecordNum=0;
    //routeRecord=(struct solution *)malloc(sizeof(struct solution)*AirplanesNum*10);
    routeRecord=(struct solution *)malloc(sizeof(struct solution)*5000);
	if (!routeRecord) {
		perror("malloc routeRecord");
		exit(-1);
	}
    for(i=0;i<5000;i++){
        routeRecord[i].LengthArray[0]=routeRecord[i].LengthArray[1]=0;
    }
    AirportsRecord=(int *)malloc(sizeof(int)*AirportsNum);
	if (!AirportsRecord) {
		perror("malloc AirportsRecord");
		exit(-1);
	}

//    for(i=0;i<AirplaneTypeNum;i++){
//        for(j=0;j<AirportsNum;j++){
//            RTime[i][j][j]=QARTime[i][j][j]=0;
//        }
//    }

    StaticAirports=(int **)malloc(sizeof(int *)*AirportsNum);
	if (!StaticAirports) {
		perror("malloc StaticAirports");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
		if (APT[i].Twn_hour > 0) {
			StaticAirports[i] = (int *)malloc(sizeof(int)*APT[i].Twn_hour*2);
			if (!StaticAirports[i]) {
				perror("malloc StaticAirports");
				exit(-1);
			}
			memset(StaticAirports[i], 0, sizeof(int)*APT[i].Twn_hour*2);
		}
		else {
			StaticAirports[i] = NULL;
		}
    }

    AirportsNeighboor=(int **)malloc(sizeof(int *)*AirportsNum);
	if (!AirportsNeighboor) {
		perror("malloc AirportsNeighboor");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        AirportsNeighboor[i]=(int *)malloc(sizeof(int)*AirportsNum);
		if (!AirportsNeighboor[i]) {
			perror("malloc AirportsNeighboor");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            AirportsNeighboor[i][j]=j;
        }
    }


	AllAirportsWeight.WeightAirports = (int *)malloc(sizeof(int)*AirportsNum);
	if (!AllAirportsWeight.WeightAirports) {
		perror("malloc AllAirportsWeight");
		exit(-1);
	}
	for (i = 0; i < AirportsNum; i++) {
		AllAirportsWeight.WeightAirports[i] = 1;
	}
	AllAirportsWeight.weightSum = AirportsNum;
}

void Handleinformation()
{
    int i,j,t,typei;

	//flight library
	for (i = 0; i < AirportsNum; i++) {
		for (j = 0; j < AirportsNum; j++) {
			for (typei = 0; typei < AirplaneTypeNum; typei++) {
				if (AirPassNum[i][j] == 0 || RTime[typei][i][j] == MaxTime) {
					BanAirline[typei][i][j] = BanAirline[typei][j][i] = 1;
					RTime[typei][i][j] = RTime[typei][j][i] = MaxTime;
				}
			}
		}
	}

	//high speed
	for (i = 0; i<AirportsNum; i++) {
		for (j = 0; j<AirportsNum; j++) {
			if (HighSpeed[i][j][1] <= 36 && HighSpeed[i][j][0] >= 1) {
				BanAirline[2][i][j] = BanAirline[2][j][i] = 1;
				RTime[2][i][j] = RTime[2][j][i] = MaxTime;
			}
		}
	}

	for (i = 0; i<AirportsNum; i++) {
		for (j = 0; j<AirportsNum; j++) {
			if (RTime[2][i][j]<18) {
				BanAirline[2][i][j] = BanAirline[2][i][j] = 1;
				RTime[2][i][j] = RTime[2][j][i] = MaxTime;
			}
		}
	}

    simpleOptimize();
    for(i=0;i<AirportsNum;i++){
        MyHeapSort(AirportsNeighboor[i],AirportsNum,i);
//        for(j=0;j<AirportsNum;j++){
//            printf("%d:%d %d\n",j,AirportsNeighboor[i][j],RTime[1][i][AirportsNeighboor[i][j]]);
//        }
    }

    AirportsGroup=(int **)malloc(sizeof(int *)*GroupNum);
	if (!AirportsGroup) {
		perror("malloc AirportsGroup");
		exit(-1);
	}
    for(i=0;i<GroupNum;i++){
        AirportsGroup[i]=(int *)malloc(sizeof(int)*30);
		if (!AirportsGroup[i]) {
			perror("malloc AirportsGroup");
			exit(-1);
		}
    }




    //其他航空公司
		//int value_dis;
  //  for(typei=0;typei<AirplaneTypeNum;typei++){
  //      for(i=0;i<AirportsNum;i++){
  //          for(j=0;j<AirportsNum;j++){
  //              if(OSeatAmount[i][j][0]>=2)
  //                  value_dis=1;
  //              else value_dis=1;
  //              for(t=Dbegin;t<Dend;t++){
  //                  ValueMatrix[typei][i][j][t%DayTtime]=(int)(ValueMatrix[typei][i][j][t%DayTtime]*value_dis);
  //              }
  //          }
  //      }
  //  }
    //较近的协调机场
//    for(i=0;i<26;i++){
//        BanAirlineModify(i,24);
//    }

//    for(i=0;i<AirportsNum;i++){
//        for(j=0;j<AirportsNum;j++){
//            if(BanAirline[2][i][j]==0&&AirPassNum[i][j]==0){
//                BanAirline[2][i][j]=1;
//            }
//        }
//    }




    ////190不飞协调机场间
    //for(i=0;i<26;i++){
    //    for(j=0;j<26;j++){
    //        BanAirline[1][i][j]=1;
    //    }
    //}



}

void Timeset(int AirPlaneTypei)
{
    if(AirPlaneTypei==1){
        MaxBeginTime=60;
        MaxEndTime=60;
        LongestFlightTime=96;
    }
    if(AirPlaneTypei==2){
        MaxBeginTime=36;
        MaxEndTime=60;
        LongestFlightTime=-1;
    }
}



void InsertFirstK(int *FirstK, int k, int Nownum)
{
    int i,j;
    for(i=k-1;i>=0;i--){
        if(Nownum>=FirstK[i]){
            break ;
        }
    }
    for(j=k-1;j>i;j--){
        FirstK[j]=FirstK[j-1];
    }
    if(i!=k-1)
        FirstK[i+1]=Nownum;
}

int randomGroup(int AirPlaneTypei, int *involeAirports)
{
    int i;
    int k=3;
    int *firstk= (int *)malloc(sizeof(int)*k);
    for(i=0;i<GroupNum;i++){
        InsertFirstK(firstk,k,StaticGroup[AirPlaneTypei]);
    }

    for(i=0;i<k;i++){
        if(rand()%(k-i+1)<2){
            return i;
        }
    }
	free(firstk);
	return 1;
}

void SetFixedAirports(int AirplaneTypei, int *Begin_Airports, int *involeAirports, int Passport)
{
    if(Passport!=-1){
        involeAirports[Passport]=1;
    }

    involeAirports[Begin_Airports[0]]=involeAirports[Begin_Airports[1]]=1;
    involeAirports[20]=involeAirports[19]=involeAirports[18]=involeAirports[29]=1;
    involeAirports[5]=involeAirports[14]=involeAirports[15]=involeAirports[16]=involeAirports[17]=1;
    involeAirports[9]=involeAirports[40];
}

void SetRandomAirports(int AirPlaneTypei, int *involeAirports)
{
    int i,j;
    i=randomGroup(AirPlaneTypei, involeAirports);
    for(j=1;j<AirportsGroup[i][0];j++){
        involeAirports[AirportsGroup[i][j]]=1;
    }
}

int Setting1(int AirPlaneTypei, int AirPlaneNum, int *Begin_Airports,int *involeAirports,int Passport)
{
    int i;
    for(i=0;i<AirportsNum;i++){
        involeAirports[i]=0;
    }
    SetFixedAirports(AirPlaneTypei, Begin_Airports,involeAirports, Passport);
    SetRandomAirports(AirPlaneTypei,involeAirports);

    return 1;
}


void UpdateWeight(int i, int AirPlaneTypei, int *involeAirports)
{
	int j;
	AllAirportsWeight.weightSum -= AllAirportsWeight.WeightAirports[i];
	AllAirportsWeight.WeightAirports[i] = 0;
	//printf("%d %d\n",i, AllAirportsWeight.weightSum);
	for (j = 0; j < AirportsNum; j++) {
		//printf("%d,%d\n", j, RTime[AirPlaneTypei][i][j]);
		if (involeAirports[j] == 1
			|| AllAirportsWeight.WeightAirports[j]>0)
			continue;
		if (RTime[AirPlaneTypei][i][j] < MaxTime
			&&RTime[AirPlaneTypei][i][j] >= 18) {
			AllAirportsWeight.WeightAirports[j] += 1;
			//printf("%d,%d,%d\n", j, RTime[AirPlaneTypei][i][j], AllAirportsWeight.WeightAirports[j]);
			AllAirportsWeight.weightSum += AllAirportsWeight.WeightAirports[j];
		}
	}
	//printf("%d\n",AllAirportsWeight.weightSum);
}
int Setting(int AirPlaneTypei, int AirPlaneNum, int *Begin_Airports,int *involeAirports,int Passport)
{
    int i,k;
	for (i = 0; i < AirportsNum; i++) {
		AllAirportsWeight.WeightAirports[i] = 0;
	}
	AllAirportsWeight.weightSum = 0;
    for(i=0;i<AirportsNum;i++){
        involeAirports[i]=0;
    }
    if(Passport!=-1){
        involeAirports[Passport]=1;
    }
    involeAirports[Begin_Airports[0]]=involeAirports[Begin_Airports[1]]=1;
	UpdateWeight(Begin_Airports[0], AirPlaneTypei, involeAirports);
	UpdateWeight(Begin_Airports[1], AirPlaneTypei, involeAirports);
    if(AirPlaneTypei==1){
        if(AirPlaneNum==2){
            involeAirports[20]=involeAirports[19]=involeAirports[29]=1;
            involeAirports[5]=involeAirports[14]=involeAirports[15]=involeAirports[16]=involeAirports[17]=1;
            for(k=0;k<10;k++){
                do{
                    i=rand()%(AirportsNum-26)+26;
                }while(involeAirports[i]==1||BanAirports(i));
                involeAirports[i]=1;
            }
            return 1;
        }
        if(AirPlaneNum==1){
            involeAirports[20]=involeAirports[19]=involeAirports[29]=1;
            involeAirports[5]=involeAirports[14]=involeAirports[15]=involeAirports[16]=involeAirports[17]=1;
            for(k=0;k<10;k++){
                do{
                    i=rand()%(AirportsNum-26)+26;
                }while(involeAirports[i]==1||BanAirports(i));
                involeAirports[i]=1;
            }
            return 1;
        }
    }
    if(AirPlaneTypei==2){
        int BaseAirports[5]={5,14,15,17,25};
        if(AirPlaneNum==2){
			for (k = 0; k < 2; k++) {
				i = BaseAirports[rand() % 5];
				involeAirports[i] = 1;
				UpdateWeight(i, AirPlaneTypei, involeAirports);
			}

            for(k=0;k<6;k++){
				i = RandomByWeight();
				involeAirports[i] = 1;
				UpdateWeight(i, AirPlaneTypei, involeAirports);
            }
			for (i = 0; i < AirportsNum; i++) {
				if (involeAirports[i] == 1) {
					printf("%d\t", i);
				}
			}
            printf("\n");
//            involeAirports[39]=involeAirports[22]=involeAirports[31]=involeAirports[6]=involeAirports[8]=1;
            //involeAirports[39]=involeAirports[8]=1;
            return 1;
        }
        if(AirPlaneNum==1){
			for (k = 0; k < 3; k++) {
				i = BaseAirports[rand() % 5];
				involeAirports[i] = 1;
				UpdateWeight(i, AirPlaneTypei, involeAirports);
			}


			for (k = 0; k<6; k++) {
				i = RandomByWeight();
				involeAirports[i] = 1;
				UpdateWeight(i, AirPlaneTypei, involeAirports);
			}
			for (k = 0; k < AirportsNum; k++) {
				if (involeAirports[k] == 1) {
					printf("%d\t", k);
				}
			}
			printf("\n");
            return 1;
        }
    }
    return 0;
}

int insertStaticAirports(int airporti, int timei, int direction)
{
	int i;
	if (APT[airporti].Twn_hour == -1) {
		return 1;
	}
	if (timei >= 252 || timei <= 84)
		return 1;
	for (i = 0; i < APT[airporti].Twn_hour; i++) {
		if (timei >= APT[airporti].TimeWindows_hour[i * 4] && timei < APT[airporti].TimeWindows_hour[i * 4 + 1]) {
			StaticAirports[airporti][i * 2+direction]++;
			return 1;
		}
	}
}


int findvalue(int left, int right, __int64 key)
{
	int mid=0;
	while (left <= right) {
		mid = (left + right) / 2;
		if (SupportRecord[mid] == key) {
			return -1;
		}
		else if (SupportRecord[mid]<key) {
			right = mid - 1;
		}
		else if (SupportRecord[mid]>key) {
			left = mid + 1;
		}
	}
	if (SupportRecord[mid]>key) {
		return mid + 1;
	}
	else return mid;
}



int FindBestArriveTime(int AirPlaneTypei, int *Begin_Airports, int AirPlaneNum, int *involeAirports, int PassAirport, int ArriveTime)
{
    int i,j,k;
    double Nvalue;
    int findBest=0;
	int ret;
    lastsolval=0;
	routeRecord[RecordNum].record = TransAirportsToInterger(involeAirports);
	for (k = NRecordNum; k < RecordNum; k++) {
		if (/*routeRecord[k].PlanesType == AirPlaneTypei &&
			routeRecord[k].PlanesNum == AirPlaneNum &&*/
			(routeRecord[k].record & routeRecord[RecordNum].record) == routeRecord[RecordNum].record &&
			(routeRecord[k].solrecord & routeRecord[RecordNum].record)==routeRecord[k].solrecord)
		{
			return -1;
		}
	}
	for (k = 0; k < supportRecordNum; k++) {
		if ( (SupportRecord[k] & routeRecord[RecordNum].record) == routeRecord[RecordNum].record)
			return -1;
	}
	//routeRecord[RecordNum].record = TransAirportsToInterger(involeAirports);
	//if (supportRecordNum == 0) {
	//	SupportRecord[supportRecordNum++] = routeRecord[RecordNum].record;
	//}
	//else {
	//	ret = findvalue(0, supportRecordNum - 1, routeRecord[RecordNum].record);
	//	if (ret == -1) {
	//		return -1;
	//	}
	//	for (i = supportRecordNum; i > ret; i--) {
	//		SupportRecord[i] = SupportRecord[i - 1];
	//	}
	//	SupportRecord[ret] = routeRecord[RecordNum].record;
	//	supportRecordNum++;
	//}

    if((Nvalue=FindBestmodel(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime))>1){
        findBest=1;
        if(Nvalue>=1000000){
            printfRecord(&routeRecord[RecordNum],RecordNum);
            system("pause");
        }
    }


    if(findBest==1){
        for(i=0;i<routeRecord[RecordNum].PlanesNum;i++){
            for(j=0;j<routeRecord[RecordNum].LengthArray[i];j++){
				insertStaticAirports(routeRecord[RecordNum].AirlineArray[i][j * 4], routeRecord[RecordNum].AirlineArray[i][j * 4 + 2], 0);
				insertStaticAirports(routeRecord[RecordNum].AirlineArray[i][j * 4+1], routeRecord[RecordNum].AirlineArray[i][j * 4 + 3], 1);
            }
        }
		routeRecord[RecordNum].record = TransAirportsToInterger(involeAirports);
		printf("%llx %llx\n", routeRecord[RecordNum].record, routeRecord[RecordNum].solrecord);
		//printfRecord(RecordNum);
        RecordNum++;
        UnfindNum=0;
        return 1;
    }
	else {
		SupportRecord[supportRecordNum++] = routeRecord[RecordNum].record;
		return -1;
	}
}

//int initalpopulation(int NeedNum)
//{
//    int Nownum=RecordNum;
//    int i;
//    for(i=Nownum;i<NeedNum+Nownum;i++){
//        FindBestArriveTime();
//    }
//
//
//
//}
//
//void StaticsTimeWindows()
//{
//
//}
//
//int UpdateValue()
//{
//
//}
//
//int InitiailHash()
//{
//    int i;
//    for(i=0;i<Hash_value;i++){
//        HashTable[i]=NULL;
//    }involeAirports[AirportsRecord[i]]=1;
//}
//
//int FindKey(int key, int key2, int *involeAirports)
//{
//    int i;
//    struct Hash_ll *KeyBegin;
//
//    int Nowkey=key&(Hash_value-1);
//    for(KeyBegin=HashTable[Nowkey];KeyBegin!=NULL;KeyBegin=KeyBegin->next){
//        if(KeyBegin->sum_value!=key2)
//            continue;
//        for(i=0;i<AirportsNum;i++){
//            if(KeyBegin->invole[i]!=involeAirports[i]){
//                break;
//            }
//        }
//        if(i==AirportsNum)
//            return 1;
//    }
//    return 0;
//}
//
//
//int insertInHash(int key, int key2, int *involeAirports)
//{
//    int i;
//    struct Hash_ll *KeyBegin;
//    struct Hash_ll *Now_point=(struct Hash_ll*)malloc(sizeof(struct Hash_ll));
//    int Nowkey=key&(Hash_value-1);
//
//    Now_point->sum_value=key2;
//    for(i=0;i<AirportsNum;i++){
//        Now_point->invole[i]=involeAirports[i];
//    }
//    Now_point->next=NULL;
//
//    KeyBegin=HashTable[Nowkey];
//
//    Now_point->next=KeyBegin->next;
//    KeyBegin=Now_point;
//
//
//}
//int SaveAirportsHash(int *involeAirports)
//{
//    int i;
//    int key=1;
//    int key2=0;
//    int sumi=0;
//    struct Hash_ll *begin_pointer;
//    struct Hash_ll *end_pointer;
//    for(i=0;i<AirportsNum;i++){
//        if(involeAirports[i]){
//            key*=i;
//            key2+=i;
//            sumi++;
//        }
//    }
//
//    if(FindKey(key,involeAirports)==0){
//        insertInHash(key,key2,involeAirports);
//        return 1;
//    }
//
//    return 0;
//}
//
//int ChangeInvoleAirports(int *involeAirports)
//{
//    Setting();
//}

int BaseNowAdd(int *addAirports, int MinTime,int LongTime, int *involeAirports, int AirPlaneTypei, int iii)
{
    int i,j;
    int temp;
    int Nair;
    int k=0;
    int *AirportsFlag=(int *)malloc(sizeof(int)*AirportsNum);
    int Knum=0;
    memset(AirportsFlag,0,sizeof(int)*AirportsNum);
    for(i=0;i<arNum;i++){
        if(i==iii)
            continue;
        Nair=AirportsRecord[i];
        for(j=0;j<AirportsNum;j++){
            if(RTime[AirPlaneTypei][Nair][AirportsNeighboor[Nair][j]]<MinTime)
                continue;
            if(RTime[AirPlaneTypei][Nair][AirportsNeighboor[Nair][j]]>LongTime)
                break;
            if(involeAirports[AirportsNeighboor[Nair][j]]==0&&AirportsFlag[AirportsNeighboor[Nair][j]]==0&&RTime[AirPlaneTypei][Nair][AirportsNeighboor[Nair][j]]<LongTime){
                AirportsFlag[AirportsNeighboor[Nair][j]]=1;
                printf("%d %d %d\n",Nair,AirportsNeighboor[Nair][j],RTime[AirPlaneTypei][Nair][AirportsNeighboor[Nair][j]]);
                Knum++;
            }
        }
    }

    int *sortFre=(int *)malloc(sizeof(int)*Knum);
    i=0;
    for(j=0;j<AirportsNum;j++){
        if(AirportsFlag[j]==1)
            sortFre[i++]=j;
    }
    for(i=0;i<Knum;i++){
        printf("%d\n",sortFre[i]);
    }
    for(i=0;i<Knum;i++){
        Nair=i;
        for(j=i+1;j<Knum;j++){
            if(StaticAirports[AirPlaneTypei][sortFre[j]]<StaticAirports[AirPlaneTypei][sortFre[Nair]]){
                Nair=j;
            }
        }
        temp=sortFre[i];
        sortFre[i]=sortFre[Nair];
        sortFre[Nair]=temp;
    }
    printf("\n");
    for(i=0;i<Knum;i++){
        printf("%d\n",sortFre[i]);
    }

    int Ad=0;
    AddAirportsNum=Minner(AddAirportsNum,Knum);
    for(j=0;j<AddAirportsNum;j++){
        for(i=0,k=0;i<Knum;i++){
            if(sortFre[i]==-1){
                continue;
            }
            if(rand()%(Knum-j-k+1)<2){
                addAirports[Ad++]=sortFre[i];
                printf("%d %d\n",i,sortFre[i]);
                involeAirports[sortFre[i]]=1;
                sortFre[i]=-1;
                break;
            }
            k++;
        }
    }
	free(sortFre);
	free(AirportsFlag);
	return 1;
}

int FindBFSexact(int AirPlaneTypei, int *Begin_Airports, int AirPlaneNum, int PassAirport, int ArriveTime)
{
    int i,j,k;
    int ret;
    int Maxlength=4;
	__int64 airtemp;
    int Nnum=0;
	int existCom;
    int stack_p;
    int airportsStack[5];
    int *involeAirports=(int *) malloc (sizeof(int ) *AirportsNum);
    int *initialAirports=(int *)malloc (sizeof(int) *AirportsNum);

    memset(involeAirports,0,sizeof(int )*AirportsNum);
    int nowNum=RecordNum,beginnum=RecordNum;
	Timelimit = 3600;
    do{
        Setting(AirPlaneTypei,AirPlaneNum,Begin_Airports,involeAirports,PassAirport);
    }while(FindBestArriveTime(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime)==-1);
//    for(k=0;k<AirportsNum;k++){
//            if(involeAirports[k]==1){
//                printf("%d\t",k);
//                }
//            }
//            printf("\n");
    while(nowNum<RecordNum&&RecordNum-beginnum<500){
        arNum=0;
        memset(initialAirports,0,sizeof(int)*AirportsNum);
        for(i=0;i<AirPlaneNum;i++){
            for(j=0;j<routeRecord[nowNum].LengthArray[i];j++){
                if(initialAirports[routeRecord[nowNum].AirlineArray[i][j*4]]==0){
                    AirportsRecord[arNum]=routeRecord[nowNum].AirlineArray[i][j*4];
                    initialAirports[AirportsRecord[arNum]]=1;
                    arNum+=1;
                }
                if(initialAirports[routeRecord[nowNum].AirlineArray[i][j*4+1]]==0){
                    AirportsRecord[arNum]=routeRecord[nowNum].AirlineArray[i][j*4+1];
                    initialAirports[AirportsRecord[arNum]]=1;
                    arNum+=1;
                }
            }
        }

        for(i=0;i<arNum;i++){
            if(AirportsRecord[i]==Begin_Airports[0]||AirportsRecord[i]==Begin_Airports[1]||AirportsRecord[i]==PassAirport){
                continue;
            }
			memset(involeAirports, 0, sizeof(int)*AirportsNum);
			if (TransIntergerToAirports(routeRecord[nowNum].record, involeAirports) < 4) {
				continue;
			}
            involeAirports[AirportsRecord[i]]=0;
			//airtemp = TransAirportsToInterger(involeAirports);
			////printf("\n");
			////for (k = 0; k < AirportsNum; k++) {
			////	if (involeAirports[k] == 1) {
			////		printf("%d\t", k);
			////	}
			////}
			////printf("\n");
			//existCom = 0;
			//for (k = NRecordNum; k < RecordNum; k++) {
			//	if (/*routeRecord[k].PlanesType == AirPlaneTypei &&
			//		routeRecord[k].PlanesNum == AirPlaneNum &&*/
			//		(routeRecord[k].record & airtemp) == airtemp && 
			//		(routeRecord[k].solrecord & airtemp)==routeRecord[k].solrecord)
			//	{
			//		existCom = 1;
			//		break;
			//	}
			//}
			//if (existCom == 1) {
			//	continue;
			//}
             ret=FindBestArriveTime(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime);
            if(ret==-1){
                continue;
            }

        }



        if(nowNum<RecordNum)
            nowNum++;
    }
    printf("%d\n",RecordNum-beginnum);
    for(i=beginnum;i<RecordNum;i++){
        printfRecord(&routeRecord[i],i);
    }
//    printf("mission complete\n");
	free(involeAirports); 
	free(initialAirports);
    return RecordNum-beginnum;
}


int CreateRoute1(int AirPlaneTypei, int *Begin_Airports, int AirPlaneNum, int NeedNum, int PassAirport,  int ArriveTime)
{
    int Nnum=0;
	NRecordNum = RecordNum;
	supportRecordNum = 0;
    while(Nnum<NeedNum){
        Nnum+=FindBFSexact(AirPlaneTypei, Begin_Airports, AirPlaneNum, PassAirport, ArriveTime);
    }
    return 1;
}

int CreateRoute(int AirPlaneTypei, int *Begin_Airports, int AirPlaneNum, int NeedNum, int PassAirport, int ArriveTime)
{
    int i,j;
    int k=0;
//    modelPrepare();
    int *involeAirports=(int *)malloc(sizeof(int ) *AirportsNum);
    memset(involeAirports,0,sizeof(int )*AirportsNum);
    UnfindNum=0;
    int nowNum=RecordNum,beginnum=RecordNum;
    int nofind=0;
    int Lt,Mt;
    do{
        Setting(AirPlaneTypei,AirPlaneNum,Begin_Airports,involeAirports,PassAirport);
    }while(FindBestArriveTime(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime)==-1);


    int addAirports[10];
    if(AirPlaneNum==1){
        AddAirportsNum=5;
    }
    else {
        AddAirportsNum=5;
    }
    if(AirPlaneTypei==1){
        Lt=30;
        Mt=12;
    }
    else {
        Lt=60;
        Mt=24;
    }
    while(RecordNum-beginnum<NeedNum){
        if(UnfindNum>=4){
            do{
                Setting(AirPlaneTypei,AirPlaneNum,Begin_Airports,involeAirports,PassAirport);
            }while(FindBestArriveTime(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime)==-1);
        }
        arNum=0;
        for(i=0;i<AirportsNum;i++){
            involeAirports[i]=0;
        }
        for(i=0;i<AirPlaneNum;i++){
            for(j=0;j<routeRecord[nowNum].LengthArray[i];j++){
                if(involeAirports[routeRecord[nowNum].AirlineArray[i][j*4]]==0){
                    AirportsRecord[arNum]=routeRecord[nowNum].AirlineArray[i][j*4];
                    arNum+=1;
                    involeAirports[routeRecord[nowNum].AirlineArray[i][j*4]] = 1;
                }
                if(involeAirports[routeRecord[nowNum].AirlineArray[i][j*4+1]]==0){
                    AirportsRecord[arNum]=routeRecord[nowNum].AirlineArray[i][j*4+1];
                    arNum+=1;
                    involeAirports[routeRecord[nowNum].AirlineArray[i][j*4+1]] = 1;
                }
            }
        }

        if(arNum<=2*AirPlaneNum){
            do{
                Setting(AirPlaneTypei,AirPlaneNum,Begin_Airports,involeAirports,PassAirport);
            }while(FindBestArriveTime(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime)==-1);
        }

        else {
            for(i=0;i<arNum;i++){
                if(AirportsRecord[i]==Begin_Airports[0]||AirportsRecord[i]==Begin_Airports[1]||AirportsRecord[i]==PassAirport){
                    continue;
                }
                else {
                    BaseNowAdd(addAirports,Mt,Lt,involeAirports,AirPlaneTypei,i);
                    involeAirports[AirportsRecord[i]]=0;
                    k=FindBestArriveTime(AirPlaneTypei,Begin_Airports,AirPlaneNum,involeAirports,PassAirport,ArriveTime);
                    for(j=0;j<AddAirportsNum;j++)
                    {
                        involeAirports[addAirports[j]]=0;
                    }
                    involeAirports[AirportsRecord[i]]=1;
                    if(k==-1){
                        UnfindNum++;
                        printf("\n");
                        continue;
                    }
                }
            }
        }
        if(nowNum<RecordNum-1)
            nowNum++;
    }
	free(involeAirports);
	return 1;
}

//int CreateRoute(int AirPlaneTypei, int Airportsi, int Airportsj, int AirPlaneNum)
//{
//    int i,j;
//    int k=0;
//    double fvalue;
//    int involeAirports[AirportsNum];
//    for(i=0;i<AirportsNum;i++){
//        involeAirports[i]=1;
//    }
////    involeAirports[5]=1;
////    involeAirports[17]=1;
////    involeAirports[39]=1;
////    involeAirports[21]=1;
//    fvalue=FindBestmodel(AirPlaneTypei,Airportsi,Airportsj,2,involeAirports);
//    lastsolval=bestsolval=fvalue;
//    for(i=0;i<AirportsNum;i++){
//        involeAirports[i]=0;
//    }
//    arNum=0;
//    for(i=0;i<AirPlaneNum;i++){
//        for(j=0;j<routeRecord[RecordNum].LengthArray[i];j++){
//            if(involeAirports[routeRecord[RecordNum].AirlineArray[i][j*4]]==0){
//                AirportsRecord[arNum]=routeRecord[RecordNum].AirlineArray[i][j*4];
//                arNum+=1;
//                involeAirports[routeRecord[RecordNum].AirlineArray[i][j*4]] = 1;
//            }
//            if(involeAirports[routeRecord[RecordNum].AirlineArray[i][j*4+1]]==0){
//                AirportsRecord[arNum]=routeRecord[RecordNum].AirlineArray[i][j*4+1];
//                arNum+=1;
//                involeAirports[routeRecord[RecordNum].AirlineArray[i][j*4+1]] = 1;
//            }
//        }
//    }
//    RecordNum++;
//
////    while(k++<3){
////        do {
////            i=rand()%AirportsNum;
////        }while(involeAirports[i]);
////        involeAirports[i]=1;
////        do{
////            j=rand()%arNum;
////        }while(AirportsRecord[j]==Airportsi||AirportsRecord[j]==Airportsj);
////        for(i=0;i<AirportsNum;i++){
////            involeAirports[i]=1;
////        }
////        involeAirports[AirportsRecord[j]]=0;
//
//        FindBestmodel(AirPlaneTypei,Airportsi,Airportsj,2,involeAirports);
//        RecordNum++;
//        involeAirports[39]=0;
//        FindBestmodel(AirPlaneTypei,Airportsi,Airportsj,AirPlaneNum,involeAirports);
//        RecordNum++;
////    }
//}



