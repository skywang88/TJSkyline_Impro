// TJSkylineImpro.cpp: 定义控制台应用程序的入口点。
//



#include<stdio.h>
#include<time.h>
#include <stdlib.h>
#include<limits.h>
#include"IO/ReadFile.h"
#include"route/CreateRoute.h"
#include"IO/Outxqxfile.h"
#include"Option/Option.h"
#include"setting\set.h"
int main(void)
{
	LogFile = fopen("Runlog.log", "a");
	srand(time(NULL));
	ReadFile("data.json");
	int mode_int;
	Prepare();
	EnvironmentSet();
	Handleinformation();
	char Filename[1000];
	while (1) {
		printf("input mode number\n");
		scanf("%d", &mode_int);
		switch (mode_int) {
		case 0:
			return 1;
		case 1:
			//                ReadSettingTxt();
			scanf("%s", Filename);
			Readinformation(Filename);
			break;
		case 2:
			ReAddAirlineTxt();
			break;
		case 3:
			ReadSettingTxt();
			break;
		case 4:
			CRoute();
			outFile("airline1.txt");
			break;
		case 5:
			TransToXqx();//airline information
			break;
		case 6:

			break;
		default:
			printf("error mode\n");
			break;

		}
	}
	free(routeRecord);
	fclose(LogFile);
}
