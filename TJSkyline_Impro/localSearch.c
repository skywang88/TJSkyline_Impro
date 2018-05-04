#include "route\model.h"
#include"IO/ReadFile.h"
#include"route\CreateRoute.h"
#include<math.h>

int **TimeWindows;

int IndexTimeWindows(int airportsi, int timei, int direction)
{
	int i;
	timei = timei % DayTtime;
	if (APT[airportsi].Twn_hour != -1) {
		for (i = 0; i<APT[airportsi].Twn_hour; i++) {
			if (timei >= APT[airportsi].TimeWindows_hour[i * 4]
				&& timei<APT[airportsi].TimeWindows_hour[i * 4 + 1]) {
				if (direction == 0 && APT[airportsi].TimeWindows_hour[i * 4 + 2]>0) {
					return i;
				}
				if (direction == 1 && APT[airportsi].TimeWindows_hour[i * 4 + 3]>0) {
					return i;
				}
			}
		}
	}
	return -1;
}

void UpdateValueMatrixADD(int airporti, int airportj, int timei)
{
	int timej;
	for (timej = Max(timei-HourTime*6,Dbegin); timej < Min(Dend, timei+ HourTime*6); timej++) {
		if(abs(timei-timej)<HourTime*3)
			ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]/2;
		else ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]*2/3;
	}
}

void UpdateValueMatrixDelete(int airporti, int airportj, int timei)
{
	int timej;
	for (timej = Max(timei - HourTime * 6, Dbegin); timej < Min(Dend, timei + HourTime * 6); timej++) {
		if (abs(timei - timej)<HourTime * 3)
			ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]*2;
		else ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]*3/2;
	}
}

void staticsInformation()
{
	int i,j,k;
	int indexi;
	TimeWindows = (int **)malloc(sizeof(int*)*AirportsNum);
	for (i = 0; i < AirportsNum; i++) {
		if (APT[i].Twn_hour > 0) {
			TimeWindows[i] = (int *)malloc(sizeof(int)*APT[i].Twn_hour*2);
			memset(TimeWindows[i], 0, sizeof(int)*APT[i].Twn_hour*2);
		}
	}

	for (i = 0; i < RecordNum; i++) {
		for (j = 0; j < routeRecord[i].PlanesNum; j++) {
			for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
				indexi= IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 2], 0);
				if(indexi!=-1) TimeWindows[routeRecord[i].AirlineArray[j][k * 4]][indexi*2]++;
				indexi= IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4+1], routeRecord[i].AirlineArray[j][k * 4 + 3], 1);
				if (indexi != -1) TimeWindows[routeRecord[i].AirlineArray[j][k * 4+1]][indexi * 2+1]++;
				UpdateValueMatrixADD(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 1], routeRecord[i].AirlineArray[j][k * 4 + 2]);
			}
		}
	}

	for (i = 0; i < RecordNum; i++) {
		routeRecord[i].solvalue = 0;
		for (j = 0; j < routeRecord[i].PlanesNum; j++) {
			for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
				routeRecord[i].solvalue+=(ValueMatrix[routeRecord[i].PlanesType][routeRecord[i].AirlineArray[j][k * 4]][routeRecord[i].AirlineArray[j][k * 4 + 1]][routeRecord[i].AirlineArray[j][k * 4 + 2]]*2);
			}
		}
		printf("%d\n", routeRecord[i].solvalue);
	}
	
	//for (i = 0; i < RecordNum; i++) {
	//	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
	//		for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
	//			UpdateValueMatrixDelete(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 1], routeRecord[i].AirlineArray[j][k * 4 + 2]);
	//		}
	//	}
	//}
	//printf("\n");
	//for (i = 0; i < RecordNum; i++) {
	//	routeRecord[i].solvalue = 0;
	//	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
	//		for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
	//			routeRecord[i].solvalue += ValueMatrix[routeRecord[i].PlanesType][routeRecord[i].AirlineArray[j][k * 4]][routeRecord[i].AirlineArray[j][k * 4 + 1]][routeRecord[i].AirlineArray[j][k * 4 + 2]];
	//		}
	//	}
	//	printf("%d\n", routeRecord[i].solvalue);

}

int AddSol(int i)
{
	int j, k,indexi;
	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
		for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
			indexi = IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 2], 0);
			if (indexi != -1) TimeWindows[routeRecord[i].AirlineArray[j][k * 4]][indexi * 2]++;
			indexi = IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4 + 1], routeRecord[i].AirlineArray[j][k * 4 + 3], 1);
			if (indexi != -1) TimeWindows[routeRecord[i].AirlineArray[j][k * 4 + 1]][indexi * 2 + 1]++;
			UpdateValueMatrixADD(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 1], routeRecord[i].AirlineArray[j][k * 4 + 2]);
		}
	}
	return i;
}
int DeleSol(int i)
{
	int j, k, indexi;
	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
		for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
			indexi = IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 2], 0);
			if (indexi != -1) TimeWindows[routeRecord[i].AirlineArray[j][k * 4]][indexi * 2]--;
			indexi = IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4 + 1], routeRecord[i].AirlineArray[j][k * 4 + 3], 1);
			if (indexi != -1) TimeWindows[routeRecord[i].AirlineArray[j][k * 4 + 1]][indexi * 2 + 1]--;
			UpdateValueMatrixDelete(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 1], routeRecord[i].AirlineArray[j][k * 4 + 2]);
		}
	}
	return i;
}

void CopyRouteRecord(int dst, int src)
{
	int i, j, k;
	routeRecord[dst].solvalue = routeRecord[src].solvalue;
	routeRecord[dst].PlanesNum = routeRecord[src].PlanesNum;
	routeRecord[dst].PlanesType = routeRecord[src].PlanesType;
	for (i = 0; i < routeRecord[dst].PlanesNum; i++) {
		routeRecord[dst].LengthArray[i] = routeRecord[src].LengthArray[i];
		for (j = 0; j < routeRecord[dst].LengthArray[i]*4; j++) {
			routeRecord[dst].AirlineArray[i][j] = routeRecord[src].AirlineArray[i][j];
		}
	}
}

void ReplaceSolution()
{
	int deleteSolution[2];
	int begin_Airports[2];
	int i,j,k,t;
	int find = 0;
	int *involveAirports = (int *)malloc(sizeof(int)*AirportsNum);
	for (i = 0; i < AirportsNum; i++) {
		involveAirports[i] = 1;
	}
	for (i = 0; i < RecordNum; i++) {
		for (j = 0; j < routeRecord[i].PlanesNum; j++) {
			for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
				if (APT[routeRecord[i].AirlineArray[j][k * 4]].Twn_hour != -1) {
					for (t = 0; t < APT[routeRecord[i].AirlineArray[j][k * 4]].Twn_hour; t++) {
						if (APT[routeRecord[i].AirlineArray[j][k * 4]].TimeWindows_hour[t * 4 + 2] < TimeWindows[routeRecord[i].AirlineArray[j][k * 4]][t * 2]) {
							find = 1;
							break;
						}
					}
				}
				else if (APT[routeRecord[i].AirlineArray[j][k * 4+1]].Twn_hour != -1) {
					for (t = 0; t < APT[routeRecord[i].AirlineArray[j][k * 4 + 1]].Twn_hour; t++) {
						if (APT[routeRecord[i].AirlineArray[j][k * 4 + 1]].TimeWindows_hour[t * 4 + 3] < TimeWindows[routeRecord[i].AirlineArray[j][k * 4 + 1]][t * 2 + 1]) {
							find = 1;
							break;
						}
					}
				}
			}
			if (find) break;
		}
		if (find) break;
	}
	if (!find) i = rand() % RecordNum;
	printf("%d %d\n", find, i);
	i = DeleSol(i);


	printfRecord(&routeRecord[i], i);
	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
		begin_Airports[j] = routeRecord[i].AirlineArray[j][0];
	}

	if (APT[routeRecord[i].AirlineArray[0][0]].Twn_hour != -1) {
		for (j = 0; j<APT[i].Twn_hour; j++) {
			printf("T:%d %d Use:%d %d\n", APT[routeRecord[i].AirlineArray[0][0]].TimeWindows_hour[j * 4 + 2], APT[routeRecord[i].AirlineArray[0][0]].TimeWindows_hour[j * 4 + 3], TimeWindows[routeRecord[i].AirlineArray[0][0]][j * 2], TimeWindows[routeRecord[i].AirlineArray[0][0]][j * 2 + 1]);
		}
	}

	FindBestmodel(2, begin_Airports, routeRecord[i].PlanesNum, involveAirports, -1, -1);
	if (find || routeRecord[RecordNum].solvalue > routeRecord[i].solvalue) {
		CopyRouteRecord(i, RecordNum);
	}
	printfRecord(&routeRecord[RecordNum], i);
	int indexi;
	AddSol(i);
	free(involveAirports);
}

void LocalSearch()
{
	int i;
	int niter = 0;
	Timeset(2);
	staticsInformation();
	while (niter++<3) {
		ReplaceSolution();
	}
}
