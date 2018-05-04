#include"transxqx.h"
#include"..\IO\OutxqxFile.h"
#include<stdlib.h>
void calculateXMatrix();
void calculateQMatrix();
void initialMatrix();
int **xqxMatrix;

void initialMatrix()
{
    int i,j;
    xqxMatrix=(int **)malloc(sizeof(int *)*RecordNum);
    for(i=0;i<RecordNum;i++){
        xqxMatrix[i]=(int *)malloc(sizeof(int)*RecordNum);
    }

    for(i=0;i<RecordNum;i++){
        for(j=0;j<RecordNum;j++){
            xqxMatrix[i][j]=0;
        }
    }
}


int similar(int *a, int *b)
{
    if(*a==*b&& *(a+1)==*(b+1)){
//        if(*(a+2)<=*(b+2)+36&&*(a+2)>=*(b+2)-36){
//            return 30000;
//        }
//        if(*(a+2)<=*(b+2)+72&&*(a+2)>=*(b+2)-72){
//            return 10000;
//        }
//        else if(*(a+2)<=*(b+2)+144&&*(a+2)>=*(b+2)-144){
//            return 5000;
//        }
//        else return 2500;
        return 200000;
    }
    else return 0;
}

int calculateQvalue(int point1, int point2)
{
    int i,j,p,q;
    int sum=0;
    for(i=0;i<routeRecord[point1].PlanesNum;i++){
        for(j=0;j<routeRecord[point1].LengthArray[i];j++){
            for(p=0;p<routeRecord[point2].PlanesNum;p++){
                for(q=0;q<routeRecord[point2].LengthArray[p];q++){
                    sum-=similar(&routeRecord[point1].AirlineArray[i][j*4],&routeRecord[point2].AirlineArray[p][q*4]);
                }
            }
        }
    }
    return sum;
}

void calculateQMatrix()
{
    int i,j;
    for(i=0;i<RecordNum;i++){
        for(j=i+1;j<RecordNum;j++){
            xqxMatrix[i][j]=xqxMatrix[j][i]=calculateQvalue(i,j);
        }
    }
}




void calculateXMatrix()
{
    int i,j,k;

    for(i=0;i<RecordNum;i++){
        for(j=0;j<routeRecord[i].PlanesNum;j++){
            for(k=0;k<routeRecord[i].LengthArray[j];k++){
                xqxMatrix[i][i]+=ValueMatrix[routeRecord[i].PlanesType][routeRecord[i].AirlineArray[j][k*4]][routeRecord[i].AirlineArray[j][k*4+1]][routeRecord[i].AirlineArray[j][k*4+2]%DayTtime];
            }
        }
        if(xqxMatrix[i][i]!=routeRecord[i].solvalue)
            printf("%d,%d %d\n",i,xqxMatrix[i][i],routeRecord[i].solvalue);
    }
}

void AddInMatrix(int *TotalNum, int Knum, int MaxFlight, int k)
{
    int i,j;
    int Ati;
    int Atj;

    for(i=0;i*2<Knum;i++){
            Ati=TotalNum[i*2];
            xqxMatrix[Ati][Ati]-=(BIGM*(TotalNum[i*2+1]*TotalNum[i*2+1]-2*MaxFlight*TotalNum[i*2+1])*k);
        for(j=i+1;j*2<Knum;j++){
            Atj=TotalNum[j*2];
            xqxMatrix[Ati][Atj]-=(BIGM*(TotalNum[i*2+1]*TotalNum[j*2+1])*k);
            xqxMatrix[Atj][Ati]-=(BIGM*(TotalNum[i*2+1]*TotalNum[j*2+1])*k);
        }
    }

}

void AddPunish()
{
    int i,j,k,m=0;
    int KnumIn,KnumOut;
    int routei,routej;
    int ToalNumIn[4000];
    int ToalNumOut[4000];
    int NumIn;
    int NumOut;
    for(i=0;i<AirportsNum;i++){
        for(j=0;j<APT[i].Twn_hour;j++){
            NumIn=NumOut=0;
            //if(APT[i].TimeWindows_hour[j*4]>=252||APT[i].TimeWindows_hour[j*4]<=72)
            //    continue;
            for(k=0;k<RecordNum;k++){
                KnumIn=KnumOut=0;
                for(routei=0;routei<routeRecord[k].PlanesNum;routei++){
                    for(routej=0;routej<routeRecord[k].LengthArray[routei];routej++){
                        if(routeRecord[k].AirlineArray[routei][routej*4]==i&&routeRecord[k].AirlineArray[routei][routej*4+2]>=APT[i].TimeWindows_hour[j*4]
                            &&routeRecord[k].AirlineArray[routei][routej*4+2]<APT[i].TimeWindows_hour[j*4+1]){
                            KnumOut+=1;
                        }
                        else if(routeRecord[k].AirlineArray[routei][routej*4+1]==i&&routeRecord[k].AirlineArray[routei][routej*4+3]>=APT[i].TimeWindows_hour[j*4]
                            &&routeRecord[k].AirlineArray[routei][routej*4+3]<APT[i].TimeWindows_hour[j*4+1]){
                            KnumIn+=1;
                        }
                    }
                }
                if(KnumIn!=0){
                    ToalNumIn[NumIn++]=k;
                    ToalNumIn[NumIn++]=KnumIn;
                  //  printf("%d %d: %d %d\n",i,j,k,KnumIn);
                }
                if(KnumOut!=0){
                    ToalNumOut[NumOut++]=k;
                    ToalNumOut[NumOut++]=KnumOut;
              //      printf("%d %d: %d %d\n",i,j,k,KnumOut);
                }
            }

            AddInMatrix(ToalNumIn,NumIn,APT[i].TimeWindows_hour[j*4+3],1);
            AddInMatrix(ToalNumOut,NumOut,APT[i].TimeWindows_hour[j*4+2],1);

        }
    }
}

void AddPunish2()
{
    int i,j,k,m=0;
    int Knum;
    int routei;
    int ToalNum[4000];
    int Num;
    for(i=0;i<AirportsNum;i++){
        if(APT[i].isBase==0)
            continue;
        for(j=0;j<AirplaneTypeNum;j++){
            if(j!=2)
                continue;
            if(TypeBaseN[j][i]==0)
                continue;
            Num=0;
            for(k=0;k<RecordNum;k++){
                if(routeRecord[k].PlanesType!=j){
                    continue;
                }
                Knum=0;
                for(routei=0;routei<routeRecord[k].PlanesNum;routei++){
                    if(routeRecord[k].AirlineArray[routei][0]==i){
                        Knum+=1;
                    }
                }
                if(Knum!=0){
                    ToalNum[Num++]=k;
                    ToalNum[Num++]=Knum;
                }
            }
            AddInMatrix(ToalNum,Num,TypeBaseN[j][i],1000);
        }
    }
}

void TransToXqx()
{
    initialMatrix();
    calculateXMatrix();
    calculateQMatrix();
    int i,j;
    //for(i=0;i<RecordNum;i++){
    //    for(j=0;j<RecordNum;j++){
    //        xqxMatrix[i][j]/=100;
    //    }
    //}

    OutxqxFile("xqxorg.txt");
//    for(i=0;i<RecordNum;i++){
//        printf("%d %d\n",i,xqxMatrix[i][i]);
//    }
    AddPunish();
//                for(i=0;i<RecordNum;i++){
//                    printf("%d %d\n",i,xqxMatrix[i][i]);
//                }
//                OutxqxFile("xqx1.txt");
    AddPunish2();
//    for(i=0;i<RecordNum;i++){
//        printf("%d %d\n",i,xqxMatrix[i][i]);
//    }

    OutxqxFile("xqx.txt");

    printf("Ttoxqx success\n");
}

