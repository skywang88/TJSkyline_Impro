#include"CreateRoute.h"
#include"gurobi_c.h"
#include"time.h"
#include<stdlib.h>
#include<string.h>
#include"..\IO\OutxqxFile.h"
#define MAXFLIGHTDAY 1
#define LeastValue 1

struct variable
{
    int variableType;
    int airplane;
    int airplaneType;
    int point1;
    int point2;
    int time1;
    int time2;
};

struct callback_data
{
	int AirPlaneNum;
	int AirPlaneTypei;
	int varNum;
	int xNum;
};

struct variable *Deval;
int Sumv[4];
int constraitNum=0;
double *sol;
int *ind;
double *val;

int ShiftTimeNum = 2;
int ShiftTimeBegin[2] = {15,13};
int ShiftTimeEnd[2] = {18,14};


void MySort(struct solution *Nsol)
{
	int i, j, k;
	int temp;
	for (i = 0; i<Nsol->PlanesNum; i++) {
		for (j = 0; j<Nsol->LengthArray[i]; j++) {
			for (k = 0; k<Nsol->LengthArray[i] - j - 1; k++) {
				if (Nsol->AirlineArray[i][k * 4 + 2]>Nsol->AirlineArray[i][k * 4 + 6]) {
					temp = Nsol->AirlineArray[i][k * 4];
					Nsol->AirlineArray[i][k * 4] = Nsol->AirlineArray[i][k * 4 + 4];
					Nsol->AirlineArray[i][k * 4 + 4] = temp;
					temp = Nsol->AirlineArray[i][k * 4 + 1];
					Nsol->AirlineArray[i][k * 4 + 1] = Nsol->AirlineArray[i][k * 4 + 5];
					Nsol->AirlineArray[i][k * 4 + 5] = temp;
					temp = Nsol->AirlineArray[i][k * 4 + 2];
					Nsol->AirlineArray[i][k * 4 + 2] = Nsol->AirlineArray[i][k * 4 + 6];
					Nsol->AirlineArray[i][k * 4 + 6] = temp;
					temp = Nsol->AirlineArray[i][k * 4 + 3];
					Nsol->AirlineArray[i][k * 4 + 3] = Nsol->AirlineArray[i][k * 4 + 7];
					Nsol->AirlineArray[i][k * 4 + 7] = temp;
				}
			}
		}
	}
}





int __stdcall
subtourelim(GRBmodel *model,
	void     *cbdata,
	int       where,
	void     *usrdata)
{
	struct callback_data *mydata = (struct callback_data *) usrdata;
	  int AirPlaneNum = mydata->AirPlaneNum;
	  int AirPlaneTypei= mydata->AirPlaneTypei;
	  struct solution NowSol;
	  int varNum = mydata->varNum;
	  int xNum = mydata->xNum;
	  double *nsol = NULL;
	   int    *nind = NULL;
	   double *nval = NULL;
	  int indexarray[100];
	  int indexsum;
	  int apei;
	  int timeSum;
	  int findindex;
	  int i, j, k;
	  int nz;
	int error = 0;





	
	  if (where == GRB_CB_MIPSOL) {
	        nind = (int *)    malloc(100*sizeof(int));
	        nval = (double *) malloc(100*sizeof(double));
	        nsol  = (double *) malloc(varNum *sizeof(double));
	        if (nind == NULL || nval == NULL) {
				 fprintf(stderr, "Out of memory\n");
				 exit(1);
			  }
			if (nsol == NULL) {
			  fprintf(stderr, "Out of memory\n");
			  exit(1);
			}
	    GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOL, nsol);
	
	    NowSol.PlanesNum=AirPlaneNum;
	    NowSol.PlanesType=AirPlaneTypei;
	    NowSol.LengthArray[0]=NowSol.LengthArray[1]=0;
	    int firstsolindex=0;
	    int secondsolindex=0;
	    for(i=0,indexsum=0;i<xNum;i++){
	        if(nsol[i]==1){
	            indexarray[indexsum++]=i;
	            printf("%d %d %d %d\n",i,indexsum,Deval[i].point1,Deval[i].point2);
	            apei=Deval[i].airplane;
	            NowSol.AirlineArray[apei][NowSol.LengthArray[apei]*4]=Deval[i].point1;
	            NowSol.AirlineArray[apei][NowSol.LengthArray[apei]*4+1]=Deval[i].point2;
	            NowSol.AirlineArray[apei][NowSol.LengthArray[apei]*4+2]=Deval[i].time1;
	            NowSol.AirlineArray[apei][NowSol.LengthArray[apei]*4+3]=Deval[i].time2;
	            NowSol.LengthArray[apei]++;
	        }
	    }
	    MySort(&NowSol);
		//for (i = 0; i < AirPlaneNum; i++) {
		//	for (j = 1; j < NowSol.LengthArray[i]; j++) {
		//		if (NowSol.AirlineArray[i][j * 4 ] != NowSol.AirlineArray[i][j*4-3]) {
		//			system("pause");
		//		}
		//	}
		//}
	    printfRecord(&NowSol,0);
	    for(i=0;i<AirPlaneNum;i++){
	        timeSum=0;
	        k=0;
	        for(j=0;j<NowSol.LengthArray[i];j++){
	            timeSum+=(NowSol.AirlineArray[i][j*4+3]-NowSol.AirlineArray[i][j*4+2]);
	            for(findindex=0;findindex<indexsum;findindex++){
	                if(Deval[indexarray[findindex]].point1==NowSol.AirlineArray[i][j*4]
	                   &&Deval[indexarray[findindex]].point2==NowSol.AirlineArray[i][j*4+1]){
	                    nind[k]=indexarray[findindex];
	                    nval[k]=1.0;
	                    k++;
	                    break;
	                }
	            }
	            printf("%d %d %d %d\n",findindex, indexarray[findindex],Deval[indexarray[findindex]].point1,Deval[indexarray[findindex]].point2);
				
	            if(timeSum>CrewMaxFlightTime){
					write_log(LogFile, 1, "cblazy\n");
	                for(nz=0;nz<k;nz++){
						write_log(LogFile,0,"%d  %d %d %d\n",i, nind[nz], Deval[nind[nz]].point1,Deval[nind[nz]].point2);
	                }
	                error= GRBcblazy(cbdata,k,nind,nval,GRB_LESS_EQUAL, k-1);
	                goto QUIT1;
	            }
	            if(TypeBaseN[AirPlaneTypei][NowSol.AirlineArray[i][j * 4 + 1]]>0){
	                timeSum=0;
	                k=0;
	            }
	        }
	    }
	
	      /* Add subtour elimination constraint */
	    QUIT1:
	      free(nind);
	      free(nval);
	      free(nsol);
	  }

	return error;
}

int IsMatchConstrait1(int point1, int time1, int direction)
{
    int i;
//    if(timetable[point1][time1]==0)
//        return 1;
	time1 = time1 % DayTtime;
	if (time1 == 0) {
		return 0;
	}
	if (APT[point1].Twn_five == -1 && APT[point1].Twn_hour == -1) {
		if (direction == 0 && (time1 >= 264 || time1 <= 84))
			return 0;
		else return 1;
	}

    if(APT[point1].Twn_five==0&&APT[point1].Twn_hour==0)
        return 0;
    if(APT[point1].Twn_hour!=-1){
        for(i=0;i<APT[point1].Twn_hour;i++){
            if(time1>=APT[point1].TimeWindows_hour[i*4]
            &&time1<APT[point1].TimeWindows_hour[i*4+1]){
                if(direction==0&&APT[point1].TimeWindows_hour[i*4+2]-TimeWindows[point1][i*2]>0){
                    return 1;
                }
                if(direction==1&&APT[point1].TimeWindows_hour[i*4+3]-TimeWindows[point1][i*2+1]>0){
                    return 1;
                }
            }
        }
    }
    if(APT[point1].Twn_five!=-1){
        for(i=0;i<APT[point1].Twn_five;i++){
            if(time1>=APT[point1].Timewindows_five[i*4]*1
            &&time1<APT[point1].Timewindows_five[i*4]*1+1){
                if(direction==0&&APT[point1].Timewindows_five[i*4+2]>0){
                    return 1;
                }
                if(direction==1&&APT[point1].Timewindows_five[i*4+3]>0){
                    return 1;
                }

            }
        }
    }
   // printf("IsMatch: %d %d %d",point1, time1, direction);
    return 0;
}

//换班时间设定
int IsShiftTime(int time1)
{
	for (int i = 0; i < ShiftTimeNum; i++) {
		if (time1 >= ShiftTimeBegin[i]*HourTime &&
			time1 < ShiftTimeEnd[i]*HourTime)
			return 1;
	}
	return 0;
}

int CanAirport(int point1, int AirplaneTypei)
{
    if(!APT[point1].Passable[AirplaneTypei]){
       // printf("Canairport: %d ",point1);
        return 0;
    }
//    else if(UnReachable[point1]){
//        return 0;
//    }
    return 1;
}

int IsFeasibleAirline(int airtype, int point1, int point2)
{
    if(BanAirline[airtype][point1][point2]==0){
        return 1;
    }
    else {
        // printf("Feasible: %d %d %d\n",airtype, point1, point2);
            return 0;}
}




int CalValueNum(int AirPlaneTypei, int *Begin_Airports, int Begin_airportsNum, int *involeAirports)
{
	int i = 0;
	int begini = 0;
	int beginj = 0;
	int beginii = 0;
	int airplane, point1, point2, time1, time2;
	int *MaxTimeEnd = (int *)malloc(sizeof(int)*AirportsNum);
	memset(MaxTimeEnd, 0, sizeof(int)*AirportsNum);
	for (point2 = 0; point2 < AirportsNum; point2++) {
		for (begini = 0; begini < Begin_airportsNum; begini++) {
			MaxTimeEnd[point2] = Max(MaxTimeEnd[point2], AirportsTimeEnd[AirPlaneTypei][Begin_Airports[begini * 2]][point2]);
		}
	}
	for (begini = 0; begini < Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < Begin_Airports[begini * 2 + 1]; beginj++) {
			for (point1 = 0; point1 < AirportsNum; point1++) {
				if (involeAirports[point1] == 0) {
					continue;
				}
				if (!CanAirport(point1, AirPlaneTypei)) {
					continue;
				}
				for (point2 = 0; point2 < AirportsNum; point2++) {
					if (point1 == point2)
						continue;
					if (involeAirports[point2] == 0) {
						continue;
					}
					if (!CanAirport(point1, AirPlaneTypei)) {
						continue;
					}
					if (!IsFeasibleAirline(AirPlaneTypei, point1, point2)) {
						continue;
					}

					for (time1 = AirportsTimeStart[AirPlaneTypei][Begin_Airports[begini * 2]][point1]; time1 <= Dend; time1++) {
						if (!IsMatchConstrait1(point1, time1, 0)) {
							continue;
						}
						if (ValueMatrix[AirPlaneTypei][point1][point2][time1%DayTtime] <= LeastValue) {
							continue;
						}
						for (time2 = time1 + RTime[AirPlaneTypei][point1][point2] - 1;
							time2 <= MaxTimeEnd[point2] && time2 <= time1 + RTime[AirPlaneTypei][point1][point2] + 2; time2++) {
							if (!IsMatchConstrait1(point2, time2, 1)) {
								continue;
							}

							i++;
						}
					}

				}
			}
		}
	}
	Sumv[0] = i;

	for (begini = 0; begini < Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < Begin_Airports[begini * 2 + 1]; beginj++) {
			for (point1 = 0; point1 < AirportsNum; point1++) {
				if (involeAirports[point1] == 0) {
					continue;
				}
				if (!CanAirport(point1, AirPlaneTypei)) {
					continue;
				}

				for (time1 = AirportsTimeStart[AirPlaneTypei][Begin_Airports[begini * 2]][point1]; time1 <= Dend; time1++) {
					if (!IsMatchConstrait1(point1, time1, 1)) {
						continue;
					}
					for (time2 = time1 + APT[point1].PassTime[AirPlaneTypei];
						time2 <= MaxTimeEnd[point1] && time2 <= time1 + APT[point1].PassTime[AirPlaneTypei] + 24; time2++) {
						if (!IsMatchConstrait1(point1, time2, 0)) {
							continue;
						}
						i++;

						if (TypeBaseN[AirPlaneTypei][point1] > 0
							&& IsShiftTime(time1)) {
							i++;
						}
					}
				}

			}
		}
	}
	Sumv[1] = i;


	for (begini = 0; begini < Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < Begin_Airports[begini * 2 + 1]; beginj++) {
			for (time1 = Dbegin - 36, time2 = Dbegin; time2 <= Dbegin + MaxBeginTime; time2++) {
				if (!IsMatchConstrait1(Begin_Airports[begini * 2], time2, 0)) {
					continue;
				}
				i++;
			}

			for (beginii = 0; beginii < Begin_airportsNum; beginii++) {
				for (time1 = Dend - MaxEndTime; time1 <= Dend; time1++) {
					if (!IsMatchConstrait1(Begin_Airports[beginii * 2], time1, 1)) {
						continue;
					}
					i++;
				}
			}
		}
	}

	Sumv[2] = i;
	free(MaxTimeEnd);
	Deval = (struct variable *)malloc(sizeof(struct variable)*i);
	sol = (double *)malloc(sizeof(double)*i);
	ind = (int *)malloc(sizeof(int)*i);
	val = (double *)malloc(sizeof(double)*i);
	return 1;
}



int PreValue(int AirPlaneTypei, int *Begin_Airports, int Begin_airportsNum, int *involeAirports)
{
	int i = 0;
	int begini, beginii, beginj;
	int airplane, point1, point2, time1, time2;
	int *MaxTimeEnd = (int *)malloc(sizeof(int)*AirportsNum);
	memset(MaxTimeEnd, 0, sizeof(int)*AirportsNum);
	for (point2 = 0; point2 < AirportsNum; point2++) {
		for (begini = 0; begini < Begin_airportsNum; begini++) {
			MaxTimeEnd[point2] = Max(MaxTimeEnd[point2], AirportsTimeEnd[AirPlaneTypei][Begin_Airports[begini * 2]][point2]);
		}
	}
	for (begini = 0, airplane = 0; begini < Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < Begin_Airports[begini * 2 + 1]; beginj++, airplane++) {
			for (point1 = 0; point1 < AirportsNum; point1++) {
				if (involeAirports[point1] == 0) {
					continue;
				}
				if (!CanAirport(point1, AirPlaneTypei)) {
					continue;
				}
				for (point2 = 0; point2 < AirportsNum; point2++) {
					if (point1 == point2)
						continue;
					if (involeAirports[point2] == 0) {
						continue;
					}
					if (!CanAirport(point1, AirPlaneTypei)) {
						continue;
					}
					if (!IsFeasibleAirline(AirPlaneTypei, point1, point2)) {
						continue;
					}
					for (time1 = AirportsTimeStart[AirPlaneTypei][Begin_Airports[begini * 2]][point1]; time1 <= Dend; time1++) {
						if (!IsMatchConstrait1(point1, time1, 0)) {
							continue;
						}
						if (ValueMatrix[AirPlaneTypei][point1][point2][time1%DayTtime] <= LeastValue) {
							continue;
						}
						for (time2 = time1 + RTime[AirPlaneTypei][point1][point2] - 1;
							time2 <= MaxTimeEnd[point2] && time2 <= time1 + RTime[AirPlaneTypei][point1][point2] + 2; time2++) {
							if (!IsMatchConstrait1(point2, time2, 1)) {
								continue;
							}
							Deval[i].variableType = 0;
							Deval[i].airplane = airplane;
							Deval[i].airplaneType = AirPlaneTypei;
							Deval[i].point1 = point1;
							Deval[i].point2 = point2;
							Deval[i].time1 = time1;
							Deval[i].time2 = time2;
							i++;
						}
					}

				}
			}
		}
	}
	Sumv[0] = i;

	for (begini = 0, airplane = 0; begini < Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < Begin_Airports[begini * 2 + 1]; beginj++, airplane++) {
			for (point1 = 0; point1 < AirportsNum; point1++) {
				if (involeAirports[point1] == 0) {
					continue;
				}
				if (!CanAirport(point1, AirPlaneTypei)) {
					continue;
				}
				for (time1 = AirportsTimeStart[AirPlaneTypei][Begin_Airports[begini * 2]][point1]; time1 <= Dend; time1++) {
					if (!IsMatchConstrait1(point1, time1, 1)) {
						continue;
					}
					for (time2 = time1 + APT[point1].PassTime[AirPlaneTypei];
						time2 <= MaxTimeEnd[point1] && time2 <= time1 + APT[point1].PassTime[AirPlaneTypei] + 24; time2++) {
						if (!IsMatchConstrait1(point1, time2, 0)) {
							continue;
						}
						//printf("%d_%d_%d_%d_%d\n", i, 1, point1, time1, time2);
						Deval[i].variableType = 1;
						Deval[i].airplane = airplane;
						Deval[i].airplaneType = AirPlaneTypei;
						Deval[i].point1 = point1;
						Deval[i].point2 = point1;
						Deval[i].time1 = time1;
						Deval[i].time2 = time2;
						//printf("%d_%d_%d_%d_%d\n",i, Deval[i].variableType, Deval[i].point1, Deval[i].time1, Deval[i].time2);
						i++;

						if (TypeBaseN[AirPlaneTypei][point1] > 0
							&& IsShiftTime(time1)) {
							//printf("%d_%d_%d_%d_%d\n", i, 3, point1, time1, time2);
							Deval[i].variableType = 3;
							Deval[i].airplane = airplane;
							Deval[i].airplaneType = AirPlaneTypei;
							Deval[i].point1 = point1;
							Deval[i].point2 = point1;
							Deval[i].time1 = time1;
							Deval[i].time2 = time2;
							//printf("%d_%d_%d_%d_%d\n", i, Deval[i].variableType, Deval[i].point1, Deval[i].time1, Deval[i].time2);
							i++;

						}
					}
				}

			}
		}
	}
	Sumv[1] = i;


	for (begini = 0, airplane = 0; begini<Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < Begin_Airports[begini * 2 + 1]; beginj++, airplane++) {
			for (time1 = Dbegin - 36, time2 = Dbegin; time2 <= Dbegin + MaxBeginTime; time2++) {
				if (!IsMatchConstrait1(Begin_Airports[begini * 2], time2, 0)) {
					continue;
				}
				Deval[i].variableType = 2;
				Deval[i].airplane = airplane;
				Deval[i].airplaneType = AirPlaneTypei;
				Deval[i].point1 = Begin_Airports[airplane];
				Deval[i].time1 = time1;
				Deval[i].time2 = time2;
				i++;
			}

			for (beginii = 0; beginii < Begin_airportsNum; beginii++) {
				for (time1 = Dend - MaxEndTime; time1 <= Dend; time1++) {
					if (!IsMatchConstrait1(Begin_Airports[beginii * 2], time1, 1)) {
						continue;
					}
					Deval[i].variableType = 2;
					Deval[i].airplane = airplane;
					Deval[i].airplaneType = AirPlaneTypei;
					Deval[i].point1 = Begin_Airports[beginii*2];
					Deval[i].time1 = time1;
					Deval[i].time2 = Dend + 36;
					i++;
				}
			}

		}
	}
	Sumv[2] = i;
	return 1;
}


double FindBestmodel(int AirPlaneTypei, int *begin_airports, int Begin_airportsNum, int *involeAirports, int PassAirports, int ArriveTime)
{
    clock_t begint,endt;
    constraitNum=0;
    Sumv[0]=Sumv[1]=Sumv[2]=0;
    constraitNum=0;
    CalValueNum(AirPlaneTypei, begin_airports, Begin_airportsNum, involeAirports);
    PreValue(AirPlaneTypei, begin_airports,  Begin_airportsNum , involeAirports);
    char name[200];
    memset(name,0,sizeof(char)*200);
    double tt;
    int TimeSelect[19]={72,84,96,108,120,132,144,156,168,180,192,204,216,228,240,252,264,276,288};
    int TimeLength=19;
    int error=0;
    int t1,t2;
    int i,j,api,apj;
    int mi;
    int apei;
	int shifti;
	int begini;
	int beginj;
	int beginii;
    double objval;
    int optimstatus;
	int solcount;
	int AirPlaneNum = 0;
    GRBenv *env=NULL;
    GRBmodel *model=NULL;

	for (begini = 0; begini < Begin_airportsNum; begini++) {
		AirPlaneNum += begin_airports[begini * 2+1];
	}

    error = GRBloadenv(&env, "model.log");
    if (error) goto QUIT;

    error = GRBnewmodel(env, &model, "model", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) goto QUIT;
//
    //error = GRBsetintparam(GRBgetenv(model), "LogToConsole", 0);
    //if (error) goto QUIT;

	//error = GRBsetdblparam(GRBgetenv(model), "TimeLimit", Timelimit);
	//if (error) goto QUIT;

    for(i=0;i<Sumv[0];i++){
        sprintf(name,"x_%d_%d_%d_%d_%d",Deval[i].airplane,Deval[i].point1,Deval[i].point2,Deval[i].time1,Deval[i].time2);
       // if(Deval[i].point1==5&&Deval[i].point2==17)
         //   printf("x_%d_%d_%d_%d_%d, %d\n",Deval[i].airplane,Deval[i].point1,Deval[i].point2,Deval[i].time1,Deval[i].time2,ValueMatrix[Deval[i].airplaneType][Deval[i].point1][Deval[i].point2][Deval[i].time1]);
        error = GRBaddvar(model, 0, NULL, NULL, ValueMatrix[AirPlaneTypei][Deval[i].point1][Deval[i].point2][Deval[i].time1%DayTtime],
                        0.0, 1.0, GRB_BINARY, name);
        if(error)
            goto QUIT;
    }

    for(i=Sumv[0];i<Sumv[1];i++){
        sprintf(name,"y_%d_%d_%d_%d_%d",Deval[i].airplane,Deval[i].variableType,Deval[i].point1,Deval[i].time1,Deval[i].time2);
        error = GRBaddvar(model, 0, NULL, NULL, 0,
                        0.0, 1.0, GRB_BINARY, name);
        if(error)
            goto QUIT;
    }
    for(i=Sumv[1];i<Sumv[2];i++){
        sprintf(name,"z_%d_%d_%d_%d",Deval[i].airplane,Deval[i].point1,Deval[i].time1,Deval[i].time2);
        error = GRBaddvar(model, 0, NULL, NULL, 0,
                        0.0, 1.0, GRB_BINARY, name);
        if(error)
            goto QUIT;
    }

    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error) goto QUIT;





//    for(timei=0;timei<9;timei++){
//        for(api=0;api<AirportsNum;api++){
//            if(involeAirports[api]==0){
//                continue;
//            }
//            if(APT[api].Passable[AirPlaneTypei]==0){
//                continue;
//            }
//            for(apj=0;apj<AirportsNum;apj++){
//                if(api==apj)
//                    continue;
//                if(involeAirports[apj]==0){
//                    continue;
//                }
//                if(APT[apj].Passable[AirPlaneTypei]==0){
//                    continue;
//                }
//                for(i=0,j=0;i<Sumv[0];i++){
//                    if(Deval[i].point1==api&&Deval[i].point2==apj
//                            &&Deval[i].time1>=Timeconstrait[timei]&&Deval[i].time1<Timeconstrait[timei]+60
//                            ){
//                        ind[j]=i;
//                        val[j]=1;
//                        j++;
//                    }
//                }
//                sprintf(name,"NotSame_%d_%d_%d",api,apj,timei);
//                error= GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 1, name);
//                constraitNum++;
//                if(error) goto QUIT;
//            }
//        }
//    }


	//if (AirPlaneNum == 2 && begin_airports[0] == begin_airports[1]) {
	//	for (i = Sumv[1], j = 0; i<Sumv[2]; i++) {
	//		if (Deval[i].time1 == Dbegin - 36) {
	//			ind[j] = i;
	//			val[j] = Deval[i].time2;
	//			if (Deval[i].airplane == 1) {
	//				val[j] = -val[j];
	//			}
	//			j++;
	//		}
	//	}
	//	sprintf(name, "FirstPlaneCons");
	//	error = GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 0, name);
	//	if (error)   goto QUIT;
	//}


    //飞机在起点和终点的独占性及路由约束
    for(api=0;api<AirportsNum;api++){
        if(involeAirports[api]==0){
            continue;
        }
        if(APT[api].Passable[AirPlaneTypei]==0){
            continue;
        }
        for(apj=api+1;apj<AirportsNum;apj++){
            if(involeAirports[apj]==0){
                continue;
            }
            if(APT[apj].Passable[AirPlaneTypei]==0){
                continue;
            }
            for(i=0,j=0;i<Sumv[0];i++){
                if(Deval[i].point1==api&&Deval[i].point2==apj){
                    ind[j]=i;
                    val[j]=1;
                    j++;
                }
            }
            //由i到j的航线每天至多MAXFLIGHTDAY条
            sprintf(name,"FBconsDay_%d_%d",api,apj);
            error= GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, MAXFLIGHTDAY, name);
            constraitNum++;
            if(error) goto QUIT;

            for(i=0;i<Sumv[0];i++){
                if(Deval[i].point1==apj&&Deval[i].point2==api){
                    ind[j]=i;
                    val[j]=-1;
                    j++;
                }
            }
            //由i到j的航线需和由j到i的航线数量一致
            sprintf(name,"FBcons_%d_%d",api,apj);
            error= GRBaddconstr(model, j, ind, val, GRB_EQUAL, 0, name);
            constraitNum++;
            if(error) goto QUIT;
        }
    }


//    route constrait
	for (begini = 0,apei=0; begini < Begin_airportsNum; begini++) {
		for (beginj = 0; beginj < begin_airports[begini*2+1];beginj++, apei++) {
			for (i = Sumv[1], j = 0; i < Sumv[2]; i++) {
				if (Deval[i].time1 == Dbegin - 36 && Deval[i].airplane == apei) {
					ind[j] = i;
					val[j] = 1;
					j++;
				}
			}
			//飞机在基地起点出度为1，保证独占性
			sprintf(name, "RouteBegin_%d", apei);
			error = GRBaddconstr(model, j, ind, val, GRB_EQUAL, 1, name);
			constraitNum++;
			if (error)   goto QUIT;
		}
	}

        for(api=0;api<AirportsNum;api++){
            if(involeAirports[api]==0){
                continue;
            }
            if(APT[api].Passable[AirPlaneTypei]==0){
                continue;
            }
            for(apei=0;apei<AirPlaneNum;apei++){
                for(t1=Dbegin;t1<=Dend;t1++){
                    for(i=0,j=0;i<Sumv[0];i++){
                        if(Deval[i].airplane==apei&&Deval[i].point2==api&&Deval[i].time2==t1){
                            ind[j]=i;
                            val[j]=1;
                            j++;
                        }
                    }
                    //飞机apei在t1时刻到达api机场
                    sprintf(name,"exclusivity_%d_%d_%d_1",api,apei,t1);
                    error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 1, name);
                    constraitNum++;
                    if(error)
                        goto QUIT;
                    for(i=Sumv[0];i<Sumv[1];i++){
                        if(Deval[i].airplane==apei&&Deval[i].point1==api&&Deval[i].time1==t1){
                            ind[j]=i;
                            val[j]=-1;
                            j++;
                        }
                    }
                    for(i=Sumv[1];i<Sumv[2];i++){
                        if(Deval[i].airplane==apei&&Deval[i].point1==api&&Deval[i].time1==t1){
                            ind[j]=i;
                            val[j]=-1;
                            j++;
                        }
                    }
                    //飞机apei在t1时刻在api机场开始进行等待
                    sprintf(name,"route_%d_%d_%d_1",api,apei,t1);
                    error=GRBaddconstr(model, j, ind, val, GRB_EQUAL, 0, name);
                    constraitNum++;
                    if(error)   goto QUIT;

                    for(i=0,j=0;i<Sumv[0];i++){
                        if(Deval[i].airplane==apei&&Deval[i].point1==api&&Deval[i].time1==t1){
                            ind[j]=i;
                            val[j]=1;
                            j++;
                        }
                    }
                    //飞机apei在t1时刻离开api机场
                    sprintf(name,"exclusivity_%d_%d_%d_2",api,apei,t1);
                    error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 1, name);
                    constraitNum++;
                    if(error)   goto QUIT;
                    for(i=Sumv[0];i<Sumv[1];i++){
                        if(Deval[i].airplane==apei&&Deval[i].point1==api&&Deval[i].time2==t1){
                            ind[j]=i;
                            val[j]=-1;
                            j++;
                        }
                    }

                    for(i=Sumv[1];i<Sumv[2];i++){
                        if(Deval[i].airplane==apei&&Deval[i].point1==api&&Deval[i].time2==t1){
                            ind[j]=i;
                            val[j]=-1;
                            j++;
                        }
                    }
                    //飞机apei在t1时刻在api机场等待结束
                    sprintf(name,"route_%d_%d_%d_2",api,apei,t1);
                    error=GRBaddconstr(model, j, ind, val, GRB_EQUAL, 0, name);
                    constraitNum++;
                    if(error)   goto QUIT;


                }
        }}





//    //passtime cons
//    for(i=Sumv[0];i<Sumv[1];i++){
//        j=0;
//        ind[j]=i;
//        val[j]=(BIGM-Deval[i].time2+Deval[i].time1+APT[Deval[i].point1].PassTime[Deval[i].airplaneType]);
//        j++;
//        for(i1=0;i1<Sumv[0];i1++){
//            if(Deval[i1].point2==Deval[i].point1&&
//               Deval[i1].time2==Deval[i].time1&&Deval[i1].airplane==Deval[i].airplane){
//                    ind[j]=i1;
//                    val[j]=(QARTime[Deval[i1].airplaneType][Deval[i1].point1][Deval[i1].point2]-Deval[i1].time2+Deval[i1].time1);
//                    j++;
//               }
//        }
//        sprintf(name,"passtime_%d_%d_%d_%d",Deval[i].airplane,Deval[i].point1,Deval[i].time1,Deval[i].time2);
//        error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, BIGM, name);
//        constraitNum++;
//        //printf("passtime_%d_%d_%d_%d\n",Deval[i].airplane,Deval[i].point1,Deval[i].time1,Deval[i].time2);
//        if(error)   goto QUIT;
//    }

        // balance cons
        for(begini=0;begini<Begin_airportsNum;begini++){
            for(i=Sumv[1],j=0;i<Sumv[2];i++){
                if(Deval[i].point1==begin_airports[begini*2]&&Deval[i].time2==Dend+36){
                    ind[j]=i;
                    val[j]=1;
                    j++;
                }
            }
            //基地平衡约束
            sprintf(name,"balance_%d_%d", begin_airports[begini * 2], begin_airports[begini * 2+1]);
            error=GRBaddconstr(model, j, ind, val, GRB_EQUAL, begin_airports[begini * 2+1], name);
            constraitNum++;
            if(error)   goto QUIT;
        }

        //CrewMAXWorkTime
       for(mi=0;mi<AirPlaneNum;mi++){
		   for (shifti = 0; shifti < ShiftTimeNum; shifti++) {
			   for (t1 = ShiftTimeBegin[shifti]; t1 < ShiftTimeEnd[shifti]; t1++) {
					   for (i = 0, j = 0; i < Sumv[0]; i++) {
						   if (Deval[i].airplane == mi && Deval[i].time1 <= t1*HourTime)
						   {
							   val[j] = Deval[i].time2 - Deval[i].time1;
							   ind[j] = i;
							   j++;
						   }
					   }
					   for (; i < Sumv[1]; i++) {
						   if (Deval[i].airplane == mi && Deval[i].time1 < t1*HourTime)
						   {
							   ind[j] = i;
							   if(Deval[i].variableType ==3)
								val[j] = Deval[i].time2 - Deval[i].time1-CrewMaxWorkTime;
							   else val[j] = Deval[i].time2 - Deval[i].time1;
							   j++;
						   }
					   }
					   sprintf(name, "CrewWorkTime1_%d_%d", mi, t1);
					   error = GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, CrewMaxWorkTime, name);
					   constraitNum++;
					   if (error)   goto QUIT;


					    for(i=0,j=0;i<Sumv[0];i++){
					        if(Deval[i].airplane==mi&&Deval[i].time2>= t1*HourTime)
					        {
					            ind[j]=i;
								val[j]=Deval[i].time2-Deval[i].time1;
					            j++;
					        }
					    }
					    for(;i<Sumv[1];i++){
					        if(Deval[i].airplane==mi&&Deval[i].time2>t1*HourTime)
					        {
					            ind[j]=i;
								if (Deval[i].variableType == 3)
									val[j] = Deval[i].time2 - Deval[i].time1 - CrewMaxWorkTime;
								else val[j] = Deval[i].time2 - Deval[i].time1;
					            j++;
					        }
					    }
					    sprintf(name,"CrewWorkTime2_%d_%d",mi,t1);
					    error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, CrewMaxWorkTime, name);
					    constraitNum++;
					    if(error)   goto QUIT;
			   }
		   }
        }

//        for(mi=0;mi<AirPlaneNum;mi++){
//            for(t1=0;t1<=6;t1++){
//                for(i=0,j=0;i<Sumv[0];i++){
//                    if(Deval[i].airplane==mi&&Deval[i].time2>=TimeSelect[t1])
//                    {
//                        ind[j]=i;
//                        if(BaseTypeN[Deval[i].point1][AirPlaneTypei]>0&&Deval[i].time1>=TimeSelect[t1])
//                            val[j]=Deval[i].time2-Deval[i].time1-144;
//                        else val[j]=Deval[i].time2-Deval[i].time1;
//                        j++;
//                    }
//                }
//                for(;i<Sumv[1];i++){
//                    if(Deval[i].airplane==mi&&Deval[i].time2>=TimeSelect[t1])
//                    {
//                        ind[j]=i;
//                        val[j]=Deval[i].time2-Deval[i].time1;
//                        j++;
//                    }
//                }
//                sprintf(name,"CrewWorkTime2_%d_%d",mi,t1);
//                error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 144, name);
//                constraitNum++;
//                if(error)   goto QUIT;
//            }
//        }

//       for(mi=0;mi<AirPlaneNum;mi++){
//            for(t1=0;t1<TimeLength;t1++){
//                for(i=0,j=0;i<Sumv[0];i++){
//                    if(Deval[i].airplane==mi&&APT[Deval[i].point1].isBase==1&&Deval[i].time1<=ArriveTime[t1])
//                    {
//                        ind[j]=i;
//                        if(APT[Deval[i].point2].isBase==1)
//                            val[j]=Deval[i].time2-Deval[i].time1-96;
//                        else val[j]=Deval[i].time2-Deval[i].time1;
//                        j++;
//                    }
//                }
//                sprintf(name,"CrewFlightTime1_%d_%d",mi,t1);
//                error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 96, name);
//                constraitNum++;
//                if(error)   goto QUIT;
//            }
//        }
//
//        for(mi=0;mi<AirPlaneNum;mi++){
//            for(t1=0;t1<TimeLength;t1++){
//                for(i=0,j=0;i<Sumv[0];i++){
//                    if(Deval[i].airplane==mi&&Deval[i].time2>=ArriveTime[t1])
//                    {
//                        ind[j]=i;
//                        if(APT[Deval[i].point1].isBase==1)
//                            val[j]=Deval[i].time2-Deval[i].time1-96;
//                        else val[j]=Deval[i].time2-Deval[i].time1;
//                        j++;
//                    }
//                }
//                sprintf(name,"CrewFlightTime1_%d_%d",mi,t1);
//                error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 96, name);
//                constraitNum++;
//                if(error)   goto QUIT;
//            }
//
//
//        for(mi=0;mi<AirPlaneNum;mi++){
//            for(i=0,j=0;i<Sumv[0];i++){
//                if(Deval[i].airplane==mi&&APT[Deval[i].point2].isBase==1&&Deval[i].time2<=ArriveTime[mi]+12&&Deval[i].time2>=ArriveTime[mi]){
//                    ind[j]=i;
//                    val[j]=Deval[i].time2;
//                    j++;
//                }
//            }
//            for(i=Sumv[1];i<Sumv[2];i++){
//                if(Deval[i].airplane==mi&&Deval[i].time1==Dbegin-36){
//                    ind[j]=i;
//                    val[j]=-Deval[i].time2;
//                    j++;
//                }
//            }
//            sprintf(name,"CrewWorkTime1_%d",mi);
//            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 144, name);
//            constraitNum++;
//            if(error)   goto QUIT;
//        }
//
//        for(mi=0;mi<AirPlaneNum;mi++){
//            for(i=0,j=0;i<Sumv[0];i++){
//                if(Deval[i].airplane==mi&&APT[Deval[i].point2].isBase==1&&Deval[i].time2<=ArriveTime[mi]+12&&Deval[i].time2>=ArriveTime[mi]){
//                    ind[j]=i;
//                    val[j]=-Deval[i].time2;
//                    j++;
//                }
//            }
//            for(i=Sumv[1];i<Sumv[2];i++){
//                if(Deval[i].airplane==mi&&Deval[i].time2==Dend+36){
//                    ind[j]=i;
//                    val[j]=Deval[i].time1;
//                    j++;
//                }
//            }
//            sprintf(name,"CrewWorkTime2_%d",mi);
//            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 144, name);
//            constraitNum++;
//            if(error)   goto QUIT;
//        }

        
        //for(mi=0;mi<AirPlaneNum;mi++){
        //    for(t1=0;t1<TimeLength;t1++){
        //        for(t2=t1+8;t2<TimeLength&&t2<=t1+12;t2++){
        //            for(i=0,j=0;i<Sumv[0];i++){
        //                if(Deval[i].airplane==mi&&Deval[i].time1>=TimeSelect[t1]&&Deval[i].time1<=TimeSelect[t2])
        //                {
        //                    ind[j]=i;
        //                    if(TypeBaseN[AirPlaneTypei][Deval[i].point2]>0&&Deval[i].time2<=TimeSelect[t2])
        //                        val[j]=Deval[i].time2-Deval[i].time1-CrewMaxFlightTime;
        //                    else val[j]=Deval[i].time2-Deval[i].time1;
        //                    j++;
        //                }
        //            }
        //            sprintf(name,"CrewFlightTime1_%d_%d_%d",mi,t1,t2);
        //            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, CrewMaxFlightTime, name);
        //            constraitNum++;
        //            if(error)   goto QUIT;
        //        }
        //    }
        //}

		//CrewMaxFlightTime
		for (mi = 0; mi<AirPlaneNum; mi++) {
			for (shifti = 0; shifti < ShiftTimeNum; shifti++) {
				for (t1 = ShiftTimeBegin[shifti]; t1 < ShiftTimeEnd[shifti]; t1++) {
					for (i = 0, j = 0; i < Sumv[0]; i++) {
						if (Deval[i].airplane == mi && Deval[i].time1 <= t1*HourTime)
						{
							val[j] = Deval[i].time2 - Deval[i].time1;
							ind[j] = i;
							j++;
						}
					}
					for (; i < Sumv[1]; i++) {
						if (Deval[i].airplane == mi && Deval[i].time1 < t1*HourTime)
						{
							ind[j] = i;
							if (Deval[i].variableType == 3)
								val[j] =  - CrewMaxFlightTime;
							j++;
						}
					}
					sprintf(name, "CrewMaxFlightTime1_%d_%d", mi, t1);
					error = GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, CrewMaxFlightTime, name);
					constraitNum++;
					if (error)   goto QUIT;


					for (i = 0, j = 0; i<Sumv[0]; i++) {
						if (Deval[i].airplane == mi&&Deval[i].time2 >= t1*HourTime)
						{
							ind[j] = i;
							val[j] = Deval[i].time2 - Deval[i].time1;
							j++;
						}
					}
					for (; i<Sumv[1]; i++) {
						if (Deval[i].airplane == mi&&Deval[i].time2>t1*HourTime)
						{
							ind[j] = i;
							if (Deval[i].variableType == 3)
								val[j] = -CrewMaxFlightTime;
							j++;
						}
					}
					sprintf(name, "CrewMaxFlightTime2_%d_%d", mi, t1);
					error = GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, CrewMaxFlightTime, name);
					constraitNum++;
					if (error)   goto QUIT;
				}
			}
		}
		
		//最多一次机组换班
		for (mi = 0; mi < AirPlaneNum; mi++) {
			for (i = Sumv[0],j=0; i < Sumv[1]; i++) {
				if (Deval[i].variableType == 3 && Deval[i].airplane == mi) {
					ind[j] = i;
					val[j] = 1;
					j++;
				}
			}
			sprintf(name, "MAXONESHIFT_%d", mi);
			error = GRBaddconstr(model, j, ind, val, GRB_EQUAL, 1, name);
			if (error) goto QUIT;
		}



        if(LongestFlightTime!=-1){
            for(mi=0;mi<AirPlaneNum;mi++){
                for(i=0,j=0;i<Sumv[0];i++){
                    if(Deval[i].airplane==mi)
                    {
                        ind[j]=i;
                        val[j]=Deval[i].time2-Deval[i].time1;
                        j++;
                    }
                }
                sprintf(name,"CrewMAXFlightTime_%d",mi);
                error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, LongestFlightTime, name);
                constraitNum++;
                if(error)   goto QUIT;
            }
        }

//
//        for(mi=0;mi<AirPlaneNum;mi++){
//            for(i=0,j=0;i<Sumv[0];i++){
//                if(Deval[i].airplane==mi&&Deval[i].time2<=ArriveTime[mi]+12){
//                    ind[j]=i;
//                    val[j]=Deval[i].time2-Deval[i].time1;
//                    j++;
//                }
//            }
//            sprintf(name,"CrewFlightTime1_%d",mi);
//            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 96, name);
//            constraitNum++;
//            if(error)   goto QUIT;
//        }
//
//        for(mi=0;mi<AirPlaneNum;mi++){
//            for(i=0,j=0;i<Sumv[0];i++){
//                if(Deval[i].airplane==mi&&Deval[i].time1>=ArriveTime[mi]){
//                    ind[j]=i;
//                    val[j]=Deval[i].time2-Deval[i].time1;
//                    j++;
//                }
//            }
//            sprintf(name,"CrewFlightTime2_%d",mi);
//            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 96, name);
//            constraitNum++;
//            if(error)   goto QUIT;
//        }


        if(PassAirports!=-1){
                /*passports有值，若飞机数量小于2，则经过次数大于1即可，否则等于1*/
                for(mi=0;mi<AirPlaneNum;mi++){
                    for(i=0,j=0;i<Sumv[0];i++){
                        if(Deval[i].airplane==mi&&Deval[i].point2==PassAirports){
                            ind[j]=i;
                            val[j]=1;
                            j++;
                        }
                    }
                    sprintf(name,"PassAirport1_%d",mi);
                    /*if(AirPlaneNum>1)
                        error=GRBaddconstr(model, j, ind, val, GRB_EQUAL, 1, name);
                    else */ error=GRBaddconstr(model, j, ind, val, GRB_GREATER_EQUAL, 1, name);
                    constraitNum++;
                    if(error)   goto QUIT;
                }

            if(ArriveTime!=-1){
                /*ArriveTime有值，则飞机必在设定[ArriveTime,ArriveTime+2h]内经过降落于机场一次*/
                for(mi=0;mi<AirPlaneNum;mi++){
                    for(i=0,j=0;i<Sumv[0];i++){
                        if(Deval[i].airplane==mi&&Deval[i].point2==PassAirports&&Deval[i].time2>=ArriveTime&&Deval[i].time2<=ArriveTime+12){
                            ind[j]=i;
                            val[j]=1;
                            j++;
                        }
                    }
                    sprintf(name,"PassAirportR_%d",mi);
                    /*if(AirPlaneNum>1)
                        error=GRBaddconstr(model, j, ind, val, GRB_EQUAL, 1, name);
                    else */ error=GRBaddconstr(model, j, ind, val, GRB_GREATER_EQUAL, 1, name);
                    constraitNum++;
                    if(error)   goto QUIT;
                }
            }

        if(AirPlaneNum>1){
            /*若多飞机在PassAirports换机，则两飞机的换机时间必须满足约束*/
            for(i=Sumv[0],j=0;i<Sumv[1];i++){
                if(Deval[i].point2==PassAirports&&Deval[i].airplane==0
					&& Deval[i].time1>=ArriveTime && Deval[i].time1 <= ArriveTime +12){
                    ind[j]=i;
                    val[j]=Deval[i].time2;
                    j++;
                }
                else if(Deval[i].point1==PassAirports&&Deval[i].airplane==1
					&& Deval[i].time1 >= ArriveTime && Deval[i].time1 <= ArriveTime + 12){
                    ind[j]=i;
                    val[j]=-Deval[i].time1;                    
					j++;
                }
            }
            sprintf(name,"PassAirportNear_1G");
            error=GRBaddconstr(model, j, ind, val, GRB_GREATER_EQUAL, 8, name);
            constraitNum++;
            if(error)   goto QUIT;
            sprintf(name,"PassAirportNear_1L");
            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 24, name);
            constraitNum++;
            if(error)   goto QUIT;


			for (i = Sumv[0], j = 0; i<Sumv[1]; i++) {
				if (Deval[i].point2 == PassAirports&&Deval[i].airplane == 1
					&& Deval[i].time1 >= ArriveTime && Deval[i].time1 <= ArriveTime + 12) {
					ind[j] = i;
					val[j] = Deval[i].time2;
					j++;
				}
				else if (Deval[i].point1 == PassAirports&&Deval[i].airplane == 0
					&& Deval[i].time1 >= ArriveTime && Deval[i].time1 <= ArriveTime + 12) {
					ind[j] = i;
					val[j] = -Deval[i].time1;
					j++;
				}
			}
            sprintf(name,"PassAirportNear_2G");
            error=GRBaddconstr(model, j, ind, val, GRB_GREATER_EQUAL, 8, name);
            constraitNum++;
            if(error)   goto QUIT;
            sprintf(name,"PassAirportNear_2L");
            error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, 24, name);
            constraitNum++;
            if(error)   goto QUIT;

            }
        }


//    }
		//struct callback_data mydata;
		//mydata.AirPlaneNum = AirPlaneNum;
		//mydata.AirPlaneTypei = AirPlaneTypei;
		//mydata.varNum = Sumv[2];
		//mydata.xNum = Sumv[0];
		//error = GRBsetcallbackfunc(model, subtourelim, (void *)&mydata);
		//if (error) goto QUIT;

		///* Must set LazyConstraints parameter when using lazy constraints */

		//error = GRBsetintparam(GRBgetenv(model), "LazyConstraints", 1);
		//if (error) goto QUIT;



    //Totaltime cons
//    for(mi=0;mi<AirplaneTypeNum;mi++){
//        for(i=0,j=0;i<Sumv[0];i++){
//            if(Deval[i].airplaneType==mi){
//                ind[j]=i;
//                val[j]=(Deval[i].time2-Deval[i].time1);
//                j++;
//            }
//        }
//        sprintf(name,"totaltime_%d",mi);
//        error=GRBaddconstr(model, j, ind, val, GRB_LESS_EQUAL, Ttime[mi], name);
//        constraitNum++;
//        if(error)   goto QUIT;
//    }
    begint=clock();
    //error = GRBwrite(model, "mip1.lp");
    //if (error) goto QUIT;
    //printf("%d %d %d %d\n",Sumv[0],Sumv[1],Sumv[2],constraitNum);
    error = GRBoptimize(model);
    if (error) goto QUIT;

    error = GRBwrite(model, "mip1.lp");
    if (error) goto QUIT;

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) goto QUIT;

	error = GRBgetintattr(model, GRB_INT_ATTR_SOLCOUNT, &solcount);
	if (error) goto QUIT;

	printf("%d\n", solcount);
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) goto QUIT;

    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, Sumv[2], sol);
    if (error) goto QUIT;



    endt=clock();
    tt=(double)(endt-begint)/CLOCKS_PER_SEC;
   // printf("time:%f\n",tt);
        if(/*optimstatus==GRB_OPTIMAL&&*/objval>lastsolval+1){
            lastsolval=objval;
            routeRecord[RecordNum].solvalue=(int)objval;
            routeRecord[RecordNum].PlanesNum=AirPlaneNum;
            routeRecord[RecordNum].PlanesType=AirPlaneTypei;
            routeRecord[RecordNum].LengthArray[0]=routeRecord[RecordNum].LengthArray[1]=0;
            int firstsolindex=0;
            int secondsolindex=0;
			__int64 l = 1;
			routeRecord[RecordNum].solrecord = 0;
			write_log(LogFile, 1,"solution record\n");
            for(i=0;i<Sumv[0];i++){
                if(sol[i]==1){
					printf("%d\n", i);
					write_log(LogFile, 0, "x_%d_%d_%d_%d_%d\n", Deval[i].airplane, Deval[i].point1, Deval[i].point2, Deval[i].time1, Deval[i].time2);
                    apei=Deval[i].airplane;
                    routeRecord[RecordNum].AirlineArray[apei][routeRecord[RecordNum].LengthArray[apei]*4]=Deval[i].point1;
                    routeRecord[RecordNum].AirlineArray[apei][routeRecord[RecordNum].LengthArray[apei]*4+1]=Deval[i].point2;
                    routeRecord[RecordNum].AirlineArray[apei][routeRecord[RecordNum].LengthArray[apei]*4+2]=Deval[i].time1;
                    routeRecord[RecordNum].AirlineArray[apei][routeRecord[RecordNum].LengthArray[apei]*4+3]=Deval[i].time2;
					routeRecord[RecordNum].solrecord = (routeRecord[RecordNum].solrecord | l << Deval[i].point1 | l << Deval[i].point2);
                    routeRecord[RecordNum].LengthArray[apei]++;
                }
            }
			for (; i < Sumv[1]; i++) {
				if(sol[i]==1)
					write_log(LogFile, 0, "y_%d_%d_%d_%d\n", Deval[i].airplane, Deval[i].point1, Deval[i].time1, Deval[i].time2);
			}
			for (; i < Sumv[2]; i++) {
				if (sol[i] == 1)
					write_log(LogFile, 0, "z_%d_%d_%d_%d\n", Deval[i].airplane, Deval[i].point1, Deval[i].time1, Deval[i].time2);
			}
			if (routeRecord[RecordNum].PlanesType > AirplaneTypeNum || routeRecord[RecordNum].PlanesType < 0) {
				printf("\n");
			}
            MySort(&routeRecord[RecordNum]);
            int mmm=0,k;
            for(j=0;j<routeRecord[RecordNum].PlanesNum;j++){
                for(k=0;k<routeRecord[RecordNum].LengthArray[j];k++){
                    mmm+=ValueMatrix[routeRecord[RecordNum].PlanesType][routeRecord[RecordNum].AirlineArray[j][k*4]][routeRecord[RecordNum].AirlineArray[j][k*4+1]][routeRecord[RecordNum].AirlineArray[j][k*4+2]];
                    //printf("%d %d %d %d %d\n",routeRecord[Nrnum].PlanesType,routeRecord[Nrnum].AirlineArray[j][k*4],routeRecord[Nrnum].AirlineArray[j][k*4+1],routeRecord[Nrnum].AirlineArray[j][k*4+2],ValueMatrix[routeRecord[Nrnum].PlanesType][routeRecord[Nrnum].AirlineArray[j][k*4]][routeRecord[Nrnum].AirlineArray[j][k*4+1]][routeRecord[Nrnum].AirlineArray[j][k*4+2]]);
                }
            }
			if (routeRecord[RecordNum].PlanesType > AirplaneTypeNum || routeRecord[RecordNum].PlanesType < 0) {
				printf("\n");
			}
            if(routeRecord[RecordNum].solvalue!=mmm){
                for(i=0;i<Sumv[0];i++){
                    if(sol[i]==1){
                        printf("%d:%d %d %d %d\n",i,Deval[i].point1,Deval[i].point2,Deval[i].time1,Deval[i].time2);
                    }
                }
                for(j=0;j<routeRecord[RecordNum].PlanesNum;j++){
                    for(k=0;k<routeRecord[RecordNum].LengthArray[j];k++){
                    mmm+=ValueMatrix[routeRecord[RecordNum].PlanesType][routeRecord[RecordNum].AirlineArray[j][k*4]][routeRecord[RecordNum].AirlineArray[j][k*4+1]][routeRecord[RecordNum].AirlineArray[j][k*4+2]];
                    printf("%d %d %d %d %d\n",routeRecord[RecordNum].PlanesType,routeRecord[RecordNum].AirlineArray[j][k*4],routeRecord[RecordNum].AirlineArray[j][k*4+1],routeRecord[RecordNum].AirlineArray[j][k*4+2],routeRecord[RecordNum].AirlineArray[j][k*4+3]);
                    }
                }
                system("pause");
            }
        }






//    for(i=0;i<AirplanesNum;i++){
//        for(j=0;j<SolSchedule[i].XnumrouteRecord;j++){
//            printf("%d %d %d %d\n",SolSchedule[i].Xinfor[j].point1,SolSchedule[i].Xinfor[j].point2,SolSchedule[i].Xinfor[j].time1,SolSchedule[i].Xinfor[j].time2);
//        }
//
//    }


    free(Deval);
    free(sol);
    free(ind);
    free(val);
      /* Free model */

      GRBfreemodel(model);

      /* Free environment */

      GRBfreeenv(env);

    return objval;


    QUIT:

      /* Error reporting */

//      if (error) {
//            printf("errornum:%d\n",error);
//        printf("ERROR: %s\n", GRBgeterrormsg(env));
//        exit(1);
//      }
        free(Deval);
        free(sol);
        free(ind);
        free(val);
      /* Free model */

      GRBfreemodel(model);

      /* Free environment */

      GRBfreeenv(env);

      return -1;

}
