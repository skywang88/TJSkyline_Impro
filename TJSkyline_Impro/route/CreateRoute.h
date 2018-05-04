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

struct solution		/*��ʾһ���ԶԳƺ��߻���������϶ԳƵĺ���*/
{
    int PlanesType;		/*����*/
    int PlanesNum;		/*�ɻ�(����)����*/
    int solvalue;		/*�����ܼ�ֵ*/
    int LengthArray[2];	/*���ߺ�����*/
    int AirlineArray[2][40];	/*���ξ�����Ϣ AirlineArray[i][k*4] ����i����k [4] [��ɻ�����������������ʱ�䣬����ʱ��]*/
	__int64	record;		/*��ѡ���л����ļ���*/
	__int64 solrecord;	/*�����еĻ�������*/
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
