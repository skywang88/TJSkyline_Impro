#include "route\model.h"
#include"IO/ReadFile.h"
#include"route\CreateRoute.h"
#include<math.h>
int NRecordNum;
int **TimeWindows;
struct localMove
{
	int moveindex[2];
	int movetype;
	int movesize;
	int resultindex;
};
struct localMove *Move;
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
	for (timej = Max(timei-HourTime*6,Dbegin); timej < Min(DayTtime, timei+ HourTime*6); timej++) {
		if(abs(timei-timej)<HourTime*3)
			ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]/2;
		else ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]*2/3;
	}
}

void UpdateValueMatrixDelete(int airporti, int airportj, int timei)
{
	int timej;
	for (timej = Max(timei - HourTime * 6, Dbegin); timej < Min(DayTtime, timei + HourTime * 6); timej++) {
		if (abs(timei - timej)<HourTime * 3)
			ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]*2;
		else ValueMatrix[2][airporti][airportj][timej] = ValueMatrix[2][airporti][airportj][timej]*3/2;
	}
}

void staticsInformation(int *illegal)
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
		for (j = 0; j < routeRecord[i].PlanesNum; j++) {
			for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
				indexi= IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4], routeRecord[i].AirlineArray[j][k * 4 + 2], 0);
				if (indexi != -1) {
					if (TimeWindows[routeRecord[i].AirlineArray[j][k * 4]][indexi * 2] > APT[routeRecord[i].AirlineArray[j][k * 4]].TimeWindows_hour[indexi * 4 + 2])
					{
						illegal[i] = 1;
						break;
					}
				}
				indexi= IndexTimeWindows(routeRecord[i].AirlineArray[j][k * 4+1], routeRecord[i].AirlineArray[j][k * 4 + 3], 1);
				if (indexi != -1) {
					if (TimeWindows[routeRecord[i].AirlineArray[j][k * 4+1]][indexi * 2+1] > APT[routeRecord[i].AirlineArray[j][k * 4+1]].TimeWindows_hour[indexi * 4 + 3])
					{
						illegal[i] = 1;
						break;
					}
				}
			}
			if (illegal[i] == 1)
				break;
		}
	}

	//for (i = 0; i < RecordNum; i++) {
	//	routeRecord[i].solvalue = 0;
	//	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
	//		for (k = 0; k < routeRecord[i].LengthArray[j]; k++) {
	//			routeRecord[i].solvalue+=(ValueMatrix[routeRecord[i].PlanesType][routeRecord[i].AirlineArray[j][k * 4]][routeRecord[i].AirlineArray[j][k * 4 + 1]][routeRecord[i].AirlineArray[j][k * 4 + 2]]*2);
	//		}
	//	}
	//	printf("%d\n", routeRecord[i].solvalue);
	//}
	

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


void CopyRouteRecord(struct solution *dst, struct solution *src)
{
	int i, j, k;
	dst->solvalue = src->solvalue;
	dst->PlanesNum = src->PlanesNum;
	dst->PlanesType = src->PlanesType;
	for (i = 0; i < dst->PlanesNum; i++) {
		dst->LengthArray[i] = src->LengthArray[i];
		for (j = 0; j < dst->LengthArray[i]*4; j++) {
			dst->AirlineArray[i][j] = src->AirlineArray[i][j];
		}
	}
}

void SwapRouteRecord(int dst, int src)
{
	struct solution temp;
	CopyRouteRecord(&temp, &routeRecord[dst]);
	CopyRouteRecord(&routeRecord[dst], &routeRecord[src]);
	CopyRouteRecord(&routeRecord[src], &temp);
}

int updateBeginAirports(int AddBegin, int *begin_airports, int begin_airportNum)
{
	int begini;
	for (begini = 0; begini < begin_airportNum; begini++) {
		if (begin_airports[begini * 2] == AddBegin) {
			begin_airports[begini * 2 + 1]++;
			return 0;
		}
	}
	if (begini == begin_airportNum) {
		begin_airports[begini * 2] = AddBegin;
		begin_airports[begini * 2 + 1] = 1;
		return 1;
	}
}


//int Decide_Procedure()
//{
//	int i;
//	switch (Move.movetype)
//	{
//		case 1:
//			SwapRouteRecord(Move.moveindex[0], RecordNum);
//			AddSol(Move.moveindex[0]);
//			break;
//		case 2:
//			SwapRouteRecord(Move.moveindex[0], RecordNum);
//			AddSol(Move.moveindex[0]);
//			SwapRouteRecord(Move.moveindex[1], NRecordNum-1);
//			break;
//		case 3:
//
//			break;
//	default:
//		break;
//	}
//}

//void ReplaceSolution()
//{
//	int deleteSolution[2];
//	int begin_Airports[2];
//	int i,j,k,t;
//	int find = 0;
//
//	if (!find) i = rand() % RecordNum;
//	printf("%d %d\n", find, i);
//	i = DeleSol(i);
//
//
//	printfRecord(&routeRecord[i], i);
//	for (j = 0; j < routeRecord[i].PlanesNum; j++) {
//		begin_Airports[j] = routeRecord[i].AirlineArray[j][0];
//	}
//
//	if (APT[routeRecord[i].AirlineArray[0][0]].Twn_hour != -1) {
//		for (j = 0; j<APT[i].Twn_hour; j++) {
//			printf("T:%d %d Use:%d %d\n", APT[routeRecord[i].AirlineArray[0][0]].TimeWindows_hour[j * 4 + 2], APT[routeRecord[i].AirlineArray[0][0]].TimeWindows_hour[j * 4 + 3], TimeWindows[routeRecord[i].AirlineArray[0][0]][j * 2], TimeWindows[routeRecord[i].AirlineArray[0][0]][j * 2 + 1]);
//		}
//	}
//
//	FindBestmodel(2, begin_Airports, routeRecord[i].PlanesNum, involveAirports, -1, -1);
//	if (find || routeRecord[RecordNum].solvalue > routeRecord[i].solvalue) {
//		CopyRouteRecord(i, RecordNum);
//	}
//	printfRecord(&routeRecord[RecordNum], i);
//	int indexi;
//	AddSol(i);
//	free(involveAirports);
//}

void LocalSearch()
{
	int i,j,ii;
	int niter = 0;
	int *involveAirports = (int *)malloc(sizeof(int)*AirportsNum);
	int *begin_Airports = (int *)malloc(sizeof(int)*AirportsNum);
	int begin_AirportsNum = 0;
	int illegal[100];
	memset(involveAirports, 0, sizeof(int)*AirportsNum);
	memset(illegal, 0, sizeof(int) * 100);
	for (i = 0; i < AirportsNum; i++) {
		involveAirports[i] = 1;
	}
	Timeset(2);
	staticsInformation(illegal);
	NRecordNum = RecordNum;
	struct localMove Move[100];
	for (i = 0; i < NRecordNum; i++) {
		printf("%d\t", illegal[i]);
	}
	int movei;
	int bestmove = 0;
	int AddBegin;
	int	begin_airportNum;
	while (niter++<3) {
		bestmove = -1;
		movei = 0;
		for (i = 11; i < NRecordNum; i++) {
			begin_airportNum = 0;
			Move[movei].movetype = 1;
			Move[movei].movesize = routeRecord[i].PlanesNum;
			Move[movei].moveindex[0] = i;
			
			DeleSol(i);
			for (ii = 0; ii < routeRecord[i].PlanesNum; ii++) {
				AddBegin = routeRecord[i].AirlineArray[ii][0];
				begin_airportNum += updateBeginAirports(AddBegin, begin_Airports, begin_airportNum);
			}
			if (FindBestmodel(2, begin_Airports, begin_airportNum, involveAirports, -1, -1) > 0) {
				Move[movei].resultindex = RecordNum;
				movei++;
				printfRecord(&routeRecord[RecordNum], RecordNum);
				RecordNum++;
			}
			for (j = i+1; j < NRecordNum; j++) {
				if (routeRecord[i].PlanesNum + routeRecord[j].PlanesNum > 2)
					continue;
				Move[movei].movetype = 2;
				Move[movei].movesize = 2;
				Move[movei].moveindex[0] = i;
				Move[movei].moveindex[1] = j;
				movei++;
				DeleSol(j);
				for (ii = 0; ii < routeRecord[j].PlanesNum; ii++) {
					AddBegin = routeRecord[j].AirlineArray[ii][0];
					begin_airportNum += updateBeginAirports(AddBegin, begin_Airports, begin_airportNum);
				}
				if (FindBestmodel(2, begin_Airports, begin_airportNum, involveAirports, -1, -1) > 0) {
					Move[movei].resultindex = RecordNum;
					movei++;
					printfRecord(&routeRecord[RecordNum], RecordNum);
					RecordNum++;
				}
				AddSol(j);
			}
			AddSol(i);
		}
		for (i = 0; i < movei; i++) {
			printfRecord(&routeRecord[Move[i].resultindex], Move[i].resultindex);
		}
		//begin_AirportsNum = Pre_Procedure(involveAirports, begin_Airports);
		//FindBestmodel(2, begin_Airports, begin_AirportsNum, involveAirports, -1, -1);
	//	Decide_Procedure();
	}
}
