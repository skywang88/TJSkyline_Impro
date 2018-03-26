#include"set.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

int CrewMaxWorkTime=144;
int CrewMaxFlightTime=95;
int Dbegin=78;
int Dend=312;
int SingNum=30;
int DoubleNum=20;
char resourceLocation[80]="resource\\";

int EnvironmentSet()
{
    FILE *fp;
    char op;
    char strline[80];
    fp=fopen("setting\\set.txt","rb");
    while(!feof(fp))
    {
        fscanf(fp,"%c:%s",&op,strline);
        switch(op)
        {
        case 'W':
            CrewMaxWorkTime=atoi(strline);
            break;
        case 'F':
            CrewMaxWorkTime=atoi(strline);
            break;
        case 'B':
            Dbegin=atoi(strline);
            break;
        case 'E':
            Dend=atoi(strline);
            break;
        case 'L':
            strcpy(resourceLocation,strline);
            break;
        case 'S':
            SingNum=atoi(strline);
            break;
        case 'D':
            DoubleNum=atoi(strline);
            break;
        default:
            printf("error setting tet");
            exit(-1);
            break;
        }
    }
    return 1;
}
