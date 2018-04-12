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
    int isBase;		/*�Ƿ�Ϊ����*/
    int Twn_hour;	/*��СʱΪ��λʱ�䴰����   -1��ʾʱ�������� 0��ʾû�п���ʱ�䴰*/
    int Twn_five;	/*��5����Ϊ��λʱ�䴰���� -1��ʾʱ�������� 0��ʾû�п���ʱ�䴰*/
    int *TimeWindows_hour; /*������СʱΪ��λʱ����Դ��Ϣ TimeWindows_hour[4*k] ��k��ʱ�䴰 [4]--[ʱ�䴰������,ʱ�䴰������,�������,��������]*/
    int *Timewindows_five;
    int *PassTime;	/*��ͬ���͹�վʱ�� PassTime[k] ����k*/
    int *Passable;	/*�����Ƿ��ͣ��*/
    int *Maintain;	/*�Բ�ͬ���͵�ά������*/
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

