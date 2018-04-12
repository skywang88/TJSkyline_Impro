#ifndef IO_x
#define IO_x
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../setting/set.h"
#include"../cJSONFiles/cJSON.h"
#define TimeUnit (5)
#define HourTime (60/TimeUnit)
#define DayTtime (24*60/TimeUnit)
#define BIGM (100000)
#define MaxTime (300)

void ReadFile(char *filename);
int BanAirports(int pointi);
int UnlimitedAirports(int point1);


struct AirPlane
{
    int Airplanetype;	
    int AtBase;
};

struct AirPort
{
    int isBase;		/*是否为基地*/
    int Twn_hour;	/*以小时为单位时间窗数量   -1表示时间无限制 0表示没有可用时间窗*/
    int Twn_five;	/*以5分钟为单位时间窗数量 -1表示时间无限制 0表示没有可用时间窗*/
    int *TimeWindows_hour; /*机场以小时为单位时间资源信息 TimeWindows_hour[4*k] 第k个时间窗 [4]--[时间窗左区间,时间窗右区间,起飞数量,降落数量]*/
    int *Timewindows_five;
    int *PassTime;	/*不同机型过站时间 PassTime[k] 机型k*/
    int *Passable;	/*机型是否可停靠*/
    int *Maintain;	/*对不同机型的维修能力*/
};

struct Quadruples
{
    int aindex;
    int dindex;
    int iindex;
    int jindex;
};


//extern char AirportsIndex[46][20];
//extern char AirPlaneIndex[4][5];
extern struct AirPlane *APE;
extern struct AirPort *APT;
extern struct Quadruples *MQP;
extern int QuadNum;
extern int ***QARTime;
extern int ***RTime;
extern int ****ValueMatrix;
extern int **TypeBaseN;
extern int Days;
//extern int Dbegin;
//extern int Dend;
extern int *Ttime;
extern int AirplanesNum;
extern int AirportsNum;
extern int AirplaneTypeNum;
extern int ***BanAirline;
extern int ***CAirline;
extern int MaxBeginTime;
extern int MaxEndTime;
extern int LongestFlightTime;
extern int ***TSeatAmount;
extern int ***OSeatAmount;
extern int **AirPassNum;

extern int ***HighSpeed;
extern char CityIndex[56][20];
int TypeToindex(char *Te);
int CityToindex(char *Cy);






#endif

