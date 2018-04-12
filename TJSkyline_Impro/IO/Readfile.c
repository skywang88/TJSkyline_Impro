#include"ReadFile.h"
#include"OutxqxFile.h"
#include<stdlib.h>
#include<string.h>
struct AirPlane *APE;	/*airplane �ɻ����ݼ���*/
struct AirPort *APT;	/*airports �������ݼ���*/
struct Quadruples *MQP; /*ά�����ݼ���*/
int QuadNum;	/*ά��������*/
int ***QARTime;	/*QAR����ʱ����� QAR[k][i][j] ����k ��ɻ���i �������j*/
int ***RTime;	/*ʵ�ʷ���ʱ����� R[k][i][j] ����k ��ɻ���i �������j*/
int ****ValueMatrix;	/*��ֵ�������� valueMatrix[k][i][j][t] ����k ��ɻ���i �������j ���ʱ��t*/
int **TypeBaseN;	/*�����ڻ��ص��������� TypeBaseN[k][i] ����k �ڻ���i������*/
int **AirPassNum;	/*�캽�Ƿ�ִ�ɹ��ĺ��� AirPassNum[i][j]  ����i ����j*/
int Days;
int *Mtime;	/*��ͬ���Ͷ�Ӧ��������ʱ�� Mtime[k]*/
int AirplanesNum; /*�ɻ�����*/
int AirportsNum;	/*��������*/
int AirplaneTypeNum;	/*��������*/
int ***BanAirline;	/*��ִֹ�ɵĺ��� BanAirline[k][i][j] ����k ����i ����j*/
int ***CAirline;	/*˦�ɺ���*/

int ***TSeatAmount;	/*�캽�ں����ϵĿ��� TSeatAmount[i][j][t] ����i ����j ʱ��t ����*/
int ***OSeatAmount; /*�������չ�˾�ں����ϵĿ��� OSeatAmount[i][j][t] ����i ����j ʱ��t ����*/

int ***HighSpeed; /*���߶�Ӧ�������� HighSpeed[i][j][2]  ������i �����j [2] -- [��������,����ʱ��]*/

cJSON* GetJsonObject(char* fileName);
int GetValueString(cJSON* json,int id, char* name, char* param);

char CityIndex[56][20] =
{
	"�Ϻ�","����","�ɶ�","����","����","����","����","����","����","�Ͼ�",
	"��ɳ","�人","֣��","�ൺ","��³ľ��","����","����","���","������","����",
	"����","����","����","����","����","����","̫ԭ","����","����","���ͺ���",
	"�ϲ�","����","�Ϸ�","ʯ��ׯ","����","����","����","�麣","����","��̨",
	"����","��˫����","����","Ȫ��","����","����","����","��ͬ","������","����",
	"����","����","��ʲ","����","����","��ͷ"
};

char CoordinateAirports[32][20] =
{
	"�Ϻ�","����","�ɶ�","����","����","����","����","����","����","�Ͼ�",
	"��ɳ","�人","֣��","�ൺ","��³ľ��","����","����","���","������","����",
	"����","����","����","����","̫ԭ","����","����","����","ʯ��ׯ","����",
	"����","����"
};

char TypeIndex[4][5] =
{
	"E145","E190","A320","A330"
};


int TypeToindex(char *Te)
{
	int i;
	for (i = 0; i<AirplaneTypeNum; i++) {
		if (!strcmp(Te, TypeIndex[i])) {
			return i;
		}
	}
	return -1;
}

int CityToindex(char *Cy)
{
	int i;
	for (i = 0; i<AirportsNum; i++) {
		if (!strcmp(Cy, CityIndex[i])) {
			return i;
		}
	}
	return -1;
}


int ReadAndSetInitial()
{
//    FILE *fp;
//
//    fopen("");
    return 1;
}

//�Ϻ� ���� �ɶ� ������Щ����û��ʱ����Դ
int BanAirports(int pointi)
{
    if(!strcmp(CityIndex[pointi],"�Ϻ�")
		|| !strcmp(CityIndex[pointi], "����")
		|| !strcmp(CityIndex[pointi], "�ɶ�") 
		|| !strcmp(CityIndex[pointi], "����")){
        return 1;
    }
    else return 0 ;
}

int IsCoordinateAirports(char *Airportsi)
{
	int i;
	for (i = 0; i < 32; i++) {
		if (!strcmp(Airportsi,CoordinateAirports[i]))
			return 1;
	}
	return 0;
}

//���� ֣�� ���� ���ʱ����Դ����
int UnlimitedAirports(int pointi)
{
    if(!strcmp(CityIndex[pointi], "����") 
		|| !strcmp(CityIndex[pointi], "֣��") 
		|| !strcmp(CityIndex[pointi], "����")
		|| !strcmp(CityIndex[pointi], "���"))
        return 1;
    else return 0;
}

//��ȡ���ݹ�ģ�����ļ�
void ReadHeadFile()
{
    cJSON *head;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"head.json");
  //  head=GetJsonObject("resource\\head.json");
    head=GetJsonObject(filename);
    cJSON *root = cJSON_GetObjectItem(head, "data");

    cJSON *C_current= cJSON_GetObjectItem(root, "airplanesNum");
    AirplanesNum=C_current->valueint;

    C_current=cJSON_GetObjectItem(root, "airplanesTypeNum");
    AirplaneTypeNum=C_current->valueint;

    C_current=cJSON_GetObjectItem(root, "airportsNum");
    AirportsNum=C_current->valueint;

    if(head)
        cJSON_Delete(head);
    return ;
//    C_current=cJSON_GetObjectItem(root, "airportsData");
//    int array_size = cJSON_GetArraySize(C_current);
//    printf("array size is %d\n",array_size);
}

//��ȡ��������  ������ͬ���Ͷ�Ӧ�����Ĺ�վʱ��
void ReadAPTFile()
{
    cJSON *head,*APT_data, *Type_list ,*Type_data,*A_current;
    int i;
    int indexi,indexj;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"part1.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "airportsData");
    if(!js_list){
        printf("no list!\n");
        return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
   // printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    int type_size;
    for(i=0; i< array_size; i++) {
		APT_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(APT_data, "index");

        indexi=A_current->valueint-1;
        A_current = cJSON_GetObjectItem(APT_data, "isBase");
        APT[indexi].isBase=A_current->valueint;
  //      APT[indexi].Twn_five=APT[indexi].Twn_hour=-1;
		//Type_list = cJSON_GetObjectItem(APT_data, "typedates");
  //      type_size=cJSON_GetArraySize(Type_list);
		//Type_data = Type_list->child;
  //      while(Type_data !=NULL){
  //          A_current = cJSON_GetObjectItem(Type_data, "index");
  //          indexj=A_current->valueint-1;
  //          A_current = cJSON_GetObjectItem(Type_data, "passTime");
  //          APT[indexi].PassTime[indexj]=A_current->valueint/TimeUnit;
  //          APT[indexi].Passable[indexj]=1;
		//	Type_data = Type_data->next;
  //      }
    }
	for (i = 0; i < AirportsNum; i++) {
		APT[i].Twn_five = APT[i].Twn_hour = -1;
		APT[i].Passable[2] = 1;
		if(IsCoordinateAirports(CityIndex[i]))
			APT[i].PassTime[2] = 65/TimeUnit;
		else APT[i].PassTime[2] = 45 / TimeUnit;
	}
    if(head)
        cJSON_Delete(head);
    return ;
}

//memory����
void initialsetting()
{
    int i,j,t;
    APE=(struct AirPlane *)malloc(sizeof(struct AirPlane)*AirplanesNum);
	if (!APE) {
		perror("malloc APE");
		exit(-1); 
	}

    APT=(struct AirPort *)malloc(sizeof(struct AirPort)*AirportsNum);
	if (!APT) {
		perror("malloc APT");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        APT[i].Passable=(int *)malloc(sizeof(int)*AirplaneTypeNum);
		if (!APT[i].Passable) {
			perror("malloc APT");
			exit(-1);
		}
        APT[i].PassTime=(int *)malloc(sizeof(int)*AirplaneTypeNum);
		if (!APT[i].PassTime) {
			perror("malloc APT");
			exit(-1);
		}
		for (j = 0; j < AirplaneTypeNum; j++) {
			APT[i].PassTime[j] = MaxTime;
		}
    }

    TypeBaseN=(int **)malloc(sizeof(int *)*AirplaneTypeNum);
	if (!TypeBaseN) {
		perror("malloc BaseTypeN");
		exit(-1);
	}
    for(i=0;i<AirplaneTypeNum;i++){
        TypeBaseN[i]=(int *)malloc(sizeof(int)*AirportsNum);
		if (!TypeBaseN[i]) {
			perror("malloc BaseTypeN");
			exit(-1);
		}
        memset(TypeBaseN[i],0,sizeof(int)*AirportsNum);
    }



    BanAirline=(int ***)malloc(sizeof(int **)*AirplaneTypeNum);
	if (!BanAirline) {
		perror("malloc BanAirline");
		exit(-1);
	}
    for(t=0;t<AirplaneTypeNum;t++){
        BanAirline[t]=(int **)malloc(sizeof(int*)*AirportsNum);
		if (!BanAirline[t]) {
			perror("malloc BanAirline");
			exit(-1);
		}
        for(i=0;i<AirportsNum;i++){
            BanAirline[t][i]=(int *)malloc(sizeof(int)*AirportsNum);
			if (!BanAirline[t][i]) {
				perror("malloc BanAirline");
				exit(-1);
			}
            for(j=0;j<AirportsNum;j++){
                BanAirline[t][i][j]=1;
            }
        }
    }

    RTime=(int ***)malloc(sizeof(int **)*AirplaneTypeNum);
	if (!RTime) {
		perror("malloc RTime");
		exit(-1);
	}
    for(i=0;i<AirplaneTypeNum;i++){
        RTime[i]=(int **)malloc(sizeof(int *)*AirportsNum);
		if (!RTime[i]) {
			perror("malloc RTime");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            RTime[i][j]=(int *)malloc(sizeof(int)*AirportsNum);
			if (!RTime[i][j]) {
				perror("malloc RTime");
				exit(-1);
			}
            for(t=0;t<AirportsNum;t++){
                RTime[i][j][t]=MaxTime;
            }
        }
    }

    QARTime=(int ***)malloc(sizeof(int **)*AirplaneTypeNum);
	if (!QARTime) {
		perror("malloc QARTime");
		exit(-1);
	}
    for(i=0;i<AirplaneTypeNum;i++){
        QARTime[i]=(int **)malloc(sizeof(int *)*AirportsNum);
		if (!QARTime[i]) {
			perror("malloc QARTime");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            QARTime[i][j]=(int *)malloc(sizeof(int)*AirportsNum);
			if (!QARTime[i][j]) {
				perror("malloc QARTime");
				exit(-1);
			}
            for(t=0;t<AirportsNum;t++){
                QARTime[i][j][t]=MaxTime;
            }
        }
    }

    ValueMatrix=(int ****)malloc(sizeof(int ***)*AirplaneTypeNum);
	if (!ValueMatrix) {
		perror("malloc ValueMatrix");
		exit(-1);
	}
    for(t=0;t<AirplaneTypeNum;t++){
        ValueMatrix[t]=(int ***)malloc(sizeof(int **)*AirportsNum);
		if (!ValueMatrix[t]) {
			perror("malloc ValueMatrix");
			exit(-1);
		}
        for(i=0;i<AirportsNum;i++){
            ValueMatrix[t][i]=(int **)malloc(sizeof(int *)*AirportsNum);
			if (!ValueMatrix[t][i]) {
				perror("malloc ValueMatrix");
				exit(-1);
			}
            for(j=0;j<AirportsNum;j++){
                ValueMatrix[t][i][j]=(int *)malloc(sizeof(int )*DayTtime);
                if(!ValueMatrix[t][i][j]) {
					perror("malloc ValueMatrix");
					exit(-1);
				}
                memset(ValueMatrix[t][i][j],0,sizeof(int)*DayTtime);
            }
        }
    }



    CAirline=(int ***)malloc(sizeof(int **)*AirportsNum);
	if (!CAirline) {
		perror("malloc CAirline");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        CAirline[i]=(int **)malloc(sizeof(int *)*AirportsNum);
		if (!CAirline[i]) {
			perror("malloc CAirline");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            CAirline[i][j]=(int *)malloc(sizeof(int )*5);
			if (!CAirline[i][j]) {
				perror("malloc CAirline");
				exit(-1);
			}
            CAirline[i][j][0]=0;
        }
    }//˦��

    TSeatAmount=(int ***)malloc(sizeof(int **)*AirportsNum);
	if (!TSeatAmount) {
		perror("malloc TSeatAmount");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        TSeatAmount[i]=(int **)malloc(sizeof(int*)*AirportsNum);
		if (!TSeatAmount[i]) {
			perror("malloc TSeatAmount");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            TSeatAmount[i][j]=(int *)malloc(sizeof(int)*DayTtime);
			if (!TSeatAmount[i][j]) {
				perror("malloc TSeatAmount");
				exit(-1);
			}
            memset(TSeatAmount[i][j],0,sizeof(int)*DayTtime);
        }
    }

    OSeatAmount=(int ***)malloc(sizeof(int **)*AirportsNum);
	if (!OSeatAmount) {
		perror("malloc OSeatAmount");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        OSeatAmount[i]=(int **)malloc(sizeof(int*)*AirportsNum);
		if (!OSeatAmount[i]) {
			perror("malloc OSeatAmount");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            OSeatAmount[i][j]=(int *)malloc(sizeof(int)*DayTtime);
			if (!OSeatAmount[i][j]) {
				perror("malloc OSeatAmount");
				exit(-1);
			}
            memset(OSeatAmount[i][j],0,sizeof(int)*DayTtime);
        }
    }

    HighSpeed=(int ***)malloc(sizeof(int**)*AirportsNum);
	if (!HighSpeed) {
		perror("malloc HighSpeed");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        HighSpeed[i]=(int **)malloc(sizeof(int*)*AirportsNum);
		if (!HighSpeed[i]) {
			perror("malloc HighSpeed");
			exit(-1);
		}
        for(j=0;j<AirportsNum;j++){
            HighSpeed[i][j]=(int *)malloc(sizeof(int )*2);
			if (!HighSpeed[i][j]) {
				perror("malloc HighSpeed");
				exit(-1);
			}
            HighSpeed[i][j][0]=HighSpeed[i][j][1]=0;
        }
    }

    AirPassNum=(int **)malloc(sizeof(int *)*AirportsNum);
	if (!AirPassNum) {
		perror("malloc AirPassNum");
		exit(-1);
	}
    for(i=0;i<AirportsNum;i++){
        AirPassNum[i]=(int *)malloc(sizeof(int)*AirportsNum);
		if (!AirPassNum[i]) {
			perror("malloc AirPassNum");
			exit(-1);
		}
        memset(AirPassNum[i],0,sizeof(int)*AirportsNum);
    }
}

//��ȡ�ɻ������ڻ�������
void ReadAPEFile()
{
    cJSON *head,*Airplane_data,*A_current;
    int i;
    int indexi,indexj;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"part3.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "aptPlaneCounts");
    if(!js_list){
     printf("no list!\n");
     return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
  //  printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Airplane_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

    A_current = cJSON_GetObjectItem(Airplane_data, "aptId");
    indexi=A_current->valueint-1;
    A_current = cJSON_GetObjectItem(Airplane_data, "planeMdlId");
    indexj=A_current->valueint-1;
    A_current = cJSON_GetObjectItem(Airplane_data, "atFlyNum");
	TypeBaseN[indexj][indexi]+=A_current->valueint;
    A_current = cJSON_GetObjectItem(Airplane_data, "standByNum");
	TypeBaseN[indexj][indexi]+=A_current->valueint;
    }
    if(head)
        cJSON_Delete(head);
    return ;
}



//��ȡ����ʱ�����
void ReadTimeFile()
{
    cJSON *head,*Airline_data, *Time_list ,*A_current, *Time_data;
    int i,j;
    int typei,indexi,indexj;
    int Time_list_size;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"routeWithTime.json");
    head=GetJsonObject(filename);
   // cJSON *root = cJSON_GetObjectItem(head, "timeMatrices");
    cJSON *js_list = cJSON_GetObjectItem(head, "Route");
    if(!js_list){
     printf("no list!\n");
     return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
   // printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Airline_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(Airline_data, "from");
        indexi=CityToindex(A_current->valuestring);
		//printf("%s,%d\t", A_current->valuestring, indexi);
        A_current = cJSON_GetObjectItem(Airline_data, "to");
        indexj= CityToindex(A_current->valuestring);
		//printf("%s,%d\n", A_current->valuestring, indexj);
        A_current = cJSON_GetObjectItem(Airline_data, "timecost");

        if(BanAirports(indexi)==0&&BanAirports(indexj)==0){
             RTime[2][indexi][indexj]=(A_current->valueint)/TimeUnit;
             QARTime[2][indexi][indexj]=(A_current->valueint)/TimeUnit;
             BanAirline[2][indexi][indexj]=0;
        }
        
    }
    if(head)
        cJSON_Delete(head);
    return ;
}

//��ȡ���߼�ֵ����
void ReadValueFile()
{
    cJSON *head,*Type_data, *Airline_list ,*A_current, *Time_data, *Time_list, *Airline_data;
    int i,j,k;
    int t1;
    int typei,indexi,indexj;
    int timei,timej;
    double Nweight;
    int airline_list_size;
    int time_size;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"final_full.json");
    head=GetJsonObject(filename);
    //cJSON *root = cJSON_GetObjectItem(head, "valueMatrices");
    cJSON *js_list = cJSON_GetObjectItem(head, "type");
    if(!js_list){
     printf("no list!\n");
     return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
   // printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Type_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(Type_data, "index");
        typei=TypeToindex(A_current->valuestring);
		Airline_list = cJSON_GetObjectItem(Type_data, "airline");
        airline_list_size = cJSON_GetArraySize(Airline_list);

        for(j=0;j< airline_list_size;j++){
			Airline_data = cJSON_GetArrayItem(Airline_list, j);
            A_current = cJSON_GetObjectItem(Airline_data, "from");
            indexi=CityToindex(A_current->valuestring);
            A_current = cJSON_GetObjectItem(Airline_data, "to");
            indexj= CityToindex(A_current->valuestring);
			Time_list = cJSON_GetObjectItem(Airline_data, "valueMatrix");
            time_size = cJSON_GetArraySize(Time_list);
            for(k=0;k<time_size;k++){
				Time_data =  cJSON_GetArrayItem(Time_list, k);

                A_current = cJSON_GetObjectItem(Time_data, "begin");
                timei = A_current->valueint/TimeUnit;
                A_current = cJSON_GetObjectItem(Time_data, "end");
                timej = A_current->valueint/ TimeUnit;
                A_current = cJSON_GetObjectItem(Time_data, "value");
                for(t1=timei;t1<timej;t1++){
                   // printf("%d %d %d %d %d\n",typei,indexi,indexj,t1,A_current->valueint);
                    ValueMatrix[typei][indexi][indexj][t1]=A_current->valueint;
                }
            }
        }
    }
    if(head)
        cJSON_Delete(head);
    return ;
}

void ReadCAirline()
{
    cJSON *head,*Type_data,*A_current;
    int i;
    int passNum;
    int api, apj;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"route.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "Route");
    if(!js_list){
        printf("no list!\n");
        return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
   // printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Type_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(Type_data, "from");
        api=CityToindex (A_current->valuestring);

        A_current = cJSON_GetObjectItem(Type_data, "to");
        apj= CityToindex(A_current->valuestring);

        AirPassNum[api][apj]+=1;
    }
    if(head)
        cJSON_Delete(head);
    return ;
}

void ReadTianjinFlightFile()
{
    cJSON *head,*Airline_data, *Time_list ,*A_current, *Time_data;
    int i,j,k;
    int Time_list_size;
    int timei,timej;
    int api, apj;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"Tianjin_airline.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "Airline");
    if(!js_list){
        printf("no list!\n");
        return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
   // printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Airline_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(Airline_data, "from");
        api=A_current->valueint-1;

        A_current = cJSON_GetObjectItem(Airline_data, "to");
        apj=A_current->valueint-1;

		Time_list = cJSON_GetObjectItem(Airline_data, "NumMatrix");
		Time_list_size = cJSON_GetArraySize(Time_list);

        for(j=0;j< Time_list_size;j++){
			Time_data = cJSON_GetArrayItem(Time_list, j);

            A_current = cJSON_GetObjectItem(Time_data, "begin");
            timei=A_current->valueint/5;
            A_current = cJSON_GetObjectItem(Time_data, "end");
            timej=A_current->valueint/5;
            A_current = cJSON_GetObjectItem(Time_data, "quantity");
            for(k=timei;k<timej;k++)
                TSeatAmount[api][apj][k]=A_current->valueint;
        }
    }
    if(head)
        cJSON_Delete(head);
    return ;
}


void ReadOtherFlightFile()
{
    cJSON *head,*Airline_data, *Time_list ,*A_current, *Time_data;
    int i,j,k;
    int Time_list_size;
    int timei,timej;
    int api, apj;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"Others_airline.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "Airline");
    if(!js_list){
        printf("no list!\n");
        return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
   // printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Airline_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(Airline_data, "from");
        api=A_current->valueint-1;

        A_current = cJSON_GetObjectItem(Airline_data, "to");
        apj=A_current->valueint-1;

        Time_list = cJSON_GetObjectItem(Airline_data, "NumMatrix");
        Time_list_size = cJSON_GetArraySize(Time_list);

        for(j=0;j< Time_list_size;j++){
            Time_data = cJSON_GetArrayItem(Time_list, j);

            A_current = cJSON_GetObjectItem(Time_data, "begin");
            timei=A_current->valueint/5;
            A_current = cJSON_GetObjectItem(Time_data, "end");
            timej=A_current->valueint/5;
            A_current = cJSON_GetObjectItem(Time_data, "quantity");
            for(k=timei;k<timej;k++)
                OSeatAmount[api][apj][k]=A_current->valueint;
        }
    }
    if(head)
        cJSON_Delete(head);
    return ;
}

void ReadTimeSourceFile()
{
    cJSON *head,*Airports_data, *Time_list ,*A_current, *Time_data;
    int i,j;
    int airportsi;
    int Time_list_size;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"TJAirlineRouteTime-7.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "type");
    if(!js_list){
        printf("no list!\n");
        return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
  //  printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Airports_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;
		
        A_current = cJSON_GetObjectItem(Airports_data, "index");
		airportsi = CityToindex(A_current->valuestring);
		//airportsi =A_current->valueint-1;

		Time_list = cJSON_GetObjectItem(Airports_data, "timeWindowsArray");
        Time_list_size = cJSON_GetArraySize(Time_list);

        APT[airportsi].Twn_hour=Time_list_size;
        APT[airportsi].TimeWindows_hour=(int *)malloc(sizeof(int)*Time_list_size*4);
        for(j=0;j< Time_list_size;j++){
			Time_data = cJSON_GetArrayItem(Time_list, j);

            A_current = cJSON_GetObjectItem(Time_data, "time");
            APT[airportsi].TimeWindows_hour[j*4]=A_current->valueint*HourTime;
            APT[airportsi].TimeWindows_hour[j*4+1]=A_current->valueint*HourTime+HourTime;
            A_current = cJSON_GetObjectItem(Time_data, "takeoff_amount");
            APT[airportsi].TimeWindows_hour[j*4+2]=A_current->valueint;
            A_current = cJSON_GetObjectItem(Time_data, "lan_amount");
            APT[airportsi].TimeWindows_hour[j*4+3]=A_current->valueint;
        }
    }
    for(i=0;i<AirportsNum;i++){
        if(BanAirports(i)){
            APT[i].Twn_hour=0;
        }
    }
    //APT[0].Twn_hour=APT[1].Twn_hour=APT[2].Twn_hour=APT[4].Twn_hour=0;
   // APT[5].Twn_hour=APT[12].Twn_hour=APT[15].Twn_hour=APT[17].Twn_hour=-1;
    if(head)
        cJSON_Delete(head);
    return ;


}



void ReadHighSpeed()
{
    cJSON *head,*Airline_data, *A_current;
    int i;
    int api, apj;
    char filename[80];
    strcpy(filename,resourceLocation);
    strcat(filename,"GT_data.json");
    head=GetJsonObject(filename);
    cJSON *js_list = cJSON_GetObjectItem(head, "info");
    if(!js_list){
        printf("no list!\n");
        return ;
    }
    int array_size = cJSON_GetArraySize(js_list);
  //  printf("array size is %d\n",array_size);
//    char *p=NULL;
//    cJSON *it;
    for(i=0; i< array_size; i++) {
		Airline_data = cJSON_GetArrayItem(js_list, i);
//        p = cJSON_PrintUnformatted(airdata);
//        it = cJSON_Parse(p);
//        if(!it)
//            continue ;

        A_current = cJSON_GetObjectItem(Airline_data, "from");
		api = CityToindex(A_current->valuestring);
		//printf("%d %s\t", api, A_current->valuestring);
        A_current = cJSON_GetObjectItem(Airline_data, "to");
        apj = CityToindex(A_current->valuestring);;
		//printf("%d %s\t", apj, A_current->valuestring);
        A_current = cJSON_GetObjectItem(Airline_data, "freq");
        HighSpeed[api][apj][0]=A_current->valueint;

        A_current = cJSON_GetObjectItem(Airline_data, "timecost");
        HighSpeed[api][apj][1]=A_current->valueint*HourTime;
    }
    if(head)
        cJSON_Delete(head);
    return ;
}

void ReadFile(char *filename)
{
    ReadHeadFile();
    initialsetting();
    ReadAPTFile();
    ReadAPEFile();
    ReadTimeFile();
    ReadCAirline();
    ReadValueFile();
    ReadTimeSourceFile();
    ReadHighSpeed();
    ReadTianjinFlightFile();
    ReadOtherFlightFile();
   // ReadFlightFile();
   // ValueModify();
}


cJSON* GetJsonObject(char* fileName)
{
    long len;
    char* pContent;
    int tmp;
    cJSON* json;
    FILE* fp = fopen(fileName, "rb+");
    if(!fp)
    {
        return NULL;
    }
    fseek(fp,0,SEEK_END);
    len=ftell(fp);
    if(0 == len)
    {
        return NULL;
    }

    fseek(fp,0,SEEK_SET);
    pContent = (char*) malloc (sizeof(char)*len);
    tmp = fread(pContent,1,len,fp);

    fclose(fp);
    json=cJSON_Parse(pContent);
    if (!json)
    {
        return NULL;
    }
    free(pContent);
    return json;
}

int GetValueString(cJSON* json,int id, char* name, char* param)
{
    cJSON* node;
    node = cJSON_GetArrayItem(json,id);
    if(!node)
    {
        return 0;
    }
    printf("%s", cJSON_GetObjectItem(node, name)->valuestring);
    //sprintf(param, "%s", cJSON_GetObjectItem(node, name)->valuestring);
    return 1;
}