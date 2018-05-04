#ifndef CROUTE_X
#define CROUTE_X

#include"../IO/ReadFile.h"
#include"../IO/OutxqxFile.h"
#define Min(a,b) (a<b?a:b)
#define Max(a,b) (a>b?a:b)
void CRoute();
void Timeset(int AirPlaneTypei);
void ReadSettingTxt();
void ReAddAirlineTxt();
void Prepare();
void Handleinformation();
void LocalSearch();
int CreateRoute(int AirPlaneTypei, int *begin_airports, int AirPlaneNum, int BaseTypeNum, int Passairports, int ArriveTime);
int CreateRoute1(int AirPlaneTypei, int *Begin_Airports, int AirPlaneNum, int NeedNum, int PassAirport, int ArriveTime);
double FindBestmodel(int AirPlaneTypei, int *begin_Airports, int AirPlaneNum, int *involeAirports, int Passairports, int arriveTime);
int FindBestArriveTime(int AirPlaneTypei, int *Begin_Airports, int AirPlaneNum, int *involeAirports, int PassAirport, int ArriveTime);

void ChangeTimeWindows();
int recoverTimewindow();

struct solution		/*表示一条自对称航线或者两条组合对称的航线*/
{
    int PlanesType;		/*机型*/
    int PlanesNum;		/*飞机(航线)数量*/
    int solvalue;		/*航线总价值*/
    int LengthArray[2];	/*航线航段数*/
    int AirlineArray[2][40];	/*航段具体信息 AirlineArray[i][k*4] 航线i航段k [4] [起飞机场，降落机场，起飞时间，降落时间]*/
	__int64	record;		/*候选飞行机场的集合*/
	__int64 solrecord;	/*航线中的机场集合*/
};
extern double Timelimit;
extern struct solution *routeRecord;
extern double bestsolval;
extern double lastsolval;
extern int RecordNum;
extern int ***AirportsTimeStart;
extern int ***AirportsTimeEnd;
extern int **TimeWindows;
#endif // CROUTE_X
