#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <vector>
#include <math.h>
#include <limits.h>
using namespace std;

FILE *fp;

double start;
int Iter;
double set_time;
int **q;
int *solution;
int *best_solution;
long long int *Gamma;
int *TabuTable;
int *Temp;
int *Tabu_Temp;
int VarNum;
int ValNum;
int seed;
long long int f;
long long int f_best;
double t_best;
char * fichier;
char *ins;

long long int ref_best;
double ref_best_t;
int ref_best_iter;
int tc;
int cutoff;
int *global_optimal_solution;

int **Qadj;
int *Degree;

int **RefSet;
int *RefFlag;
int RefSize = 10;
int current_size;
int rebuild_count;
long long int *Ref_f;
int *f_index;
long long int *Ref_f_copy;
int *IndexNewSol;
int *IndexOldSol;
int **PairSet;
int pair_count;
int *PV;
int common_count;
int non_common_count;
int *NonCommonSet, *CommonSet;
int NewSolutions;

void q_sort(long long int numbers[ ], int left, int right, int index[ ])
{
  long long int pivot;
  int pivot_index, l_hold, r_hold;
  l_hold = left;
  r_hold = right;
  pivot = numbers[left];
  pivot_index = index[left] ;
  while (left < right)
  {
    while ((numbers[right] >= pivot) && (left < right))
      right--;
    if (left != right)
    {
      numbers[left] = numbers[right];
      index[left] = index[right];
      left++;
    }
    while ((numbers[left] <= pivot) && (left < right))
      left++;
    if (left != right)
    {
      numbers[right] = numbers[left];
      index[right] = index[left];
      right--;
    }
  }
  numbers[left] = pivot;
  index[left] = pivot_index;
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    q_sort(numbers, left, pivot-1, index);
  if (right > pivot)
    q_sort(numbers, pivot+1, right, index);
}

void Quick_Sort(long long int numbers[], int array_size, int index[])
{
  q_sort(numbers, 0, array_size - 1, index);
}

void Construct_Qadj()
{
	for(int i = 0; i < VarNum; i++)
	{
		int k = 0;
		for(int j = 0; j < VarNum; j++)
		{
			if(i != j && q[i][j] != 0)
			{
			    Qadj[i][k] = j;
			    k++;
			}
		}
	    Degree[i] = k;
	}
}

void InitData()
{
     ifstream FIC;
     FIC.open(fichier);
     if ( FIC.fail() )
     {
           cout << "### File Open Error " << fichier << endl;
           exit(0);
     }
     char StrReading[100];
     FIC >> StrReading;
     if(FIC.eof())
     {
          cout << "### File Open Error " << fichier << endl;
          exit(0);
     }

     if(strcmp(StrReading, "p" )==0 )
     {

          FIC >> VarNum >> ValNum;
          solution=new int[VarNum];
		  best_solution = new int[VarNum];

          q=new int*[VarNum];
          for(int x=0;x<VarNum;x++)
             q[x]=new int[VarNum];

		  Qadj=new int*[VarNum];
          for(int x=0;x<VarNum;x++)
             Qadj[x]=new int[VarNum];

		  Degree = new int[VarNum];

          Gamma=new long long int[VarNum];
          for(int x=0;x<VarNum;x++)
             Gamma[x]=0;

          Temp=new int[VarNum];
          for(int x=0;x<VarNum;x++)
             Temp[x]=0;

          Tabu_Temp=new int[VarNum];
          for(int x=0;x<VarNum;x++)
             Tabu_Temp[x]=0;

          TabuTable=new int[VarNum];
          for(int x=0;x<VarNum;x++)
             TabuTable[x]=0;
     }
     for(int i = 0; i < VarNum; i++)
         for(int j = 0; j< VarNum; j++)
         {
               q[i][j] = 0;
			   Qadj[i][j] = -1;
         }
    for(int i = 0; i < ValNum; i++)
        {
                int x1, x2 , x3;
                FIC >> x1 >> x2 >> x3;
                x1--; x2--;
               if ( x1<0 || x2<0 || x1>=VarNum || x2 >=VarNum )
               {
                     cout << "### Read Date Error : line = "<< x1 << ", column = " << x2 << endl;
                     exit(0);
               }
              if(x1 == x2)
                   q[x1][x2] = x3;
              else
			  {
				  q[x1][x2] = 2* x3;
				  q[x2][x1] = 2* x3;
			  }
         }
	 Construct_Qadj();
     FIC.close();
}

void InitialGamma()
{
     int diff;
     for(int i = 0; i <VarNum; i++)
       Gamma[i] = 0;

     for(int i = 0; i < VarNum; i++)
     {
        diff = q[i][i];
        for(int j = 0; j < Degree[i] ; j++)
        {
			int k = Qadj[i][j];
            if(solution[k] == 1)
                diff = diff + q[i][k];
        }
        if((1 - solution[i]) == 0)
           diff = 0 - diff;
        Gamma[i] = diff;
     }
}

void InitialTabuTable()
{
     for(int i = 0; i< VarNum ; i++)
        TabuTable[i] = 0;
}

void InitialObjectValue()
{
	 f = 0;
     for(int i = 0; i < VarNum; i++)
     {
       if(solution[i] == 1)
       {
            f = f + q[i][i];
            for(int j = 0; j< Degree[i]; j++)
            {
				int k = Qadj[i][j];
               if(solution[k] ==1)
                 f = f + q[i][k] / 2;
            }
       }
     }
}

int InitialObjectValue2(int *sol)
{
	int sol_value = 0;
     for(int i = 0; i < VarNum; i++)
     {
       if(sol[i] == 1)
       {
            sol_value = sol_value + q[i][i];
            for(int j = 0; j< Degree[i]; j++)
            {
				int k = Qadj[i][j];
               if(sol[k] ==1)
                 sol_value = sol_value + q[i][k] / 2;
            }
       }
     }
	 return sol_value;
}

void UpdateGamma(int bestflip,int oldvalue)
{
	 Gamma[bestflip] = 0 - Gamma[bestflip];

     for(int i = 0; i< Degree[bestflip]; i++)
	 {
		 int k = Qadj[bestflip][i];
         if(oldvalue == solution[k])
              Gamma[k] = Gamma[k]  + q[bestflip][k];
         else
              Gamma[k] = Gamma[k] - q[bestflip][k];
     }
}

void ChangeTabuTenure(int bestflip,int iter)
{
     TabuTable[bestflip] = iter + tc + rand() % 11;
}

void TabuSearch()
{
     int iter;
     int best_number,tabu_best_number;
	 int LocalOptimum = 0;

     InitialObjectValue();
     InitialGamma();
     InitialTabuTable();
	 for(int j = 0; j < VarNum; j++)
		 best_solution[j] = solution[j];
	 f_best = f;

     for(iter  = 1; ; iter++)
     {
         int num = 0;
         int tabu_num = 0;
         int bestdiff = -INT_MAX;
         int tabu_best_diff = -INT_MAX;
         int bestflip;
         int tabu_best_flip;
         int oldvalue;
         best_number = 1;
         tabu_best_number = 1;
         for(int i = 0; i < VarNum; i++)
         {
					 if(TabuTable[i] <= iter)
                     {
                          num++;
                          if(Gamma[i] > bestdiff)
                          {
                             bestdiff = Gamma[i];
                             bestflip = i;
                             best_number = 1;
                             Temp[0] = i;
                          }
                          else if(Gamma[i] == bestdiff)
                          {
                             Temp[best_number] = i;
                              best_number++;
                          }
					 }
                     else
                     {
                          tabu_num++;
                          if(Gamma[i] > tabu_best_diff)
                          {
                             tabu_best_diff = Gamma[i];
                             tabu_best_flip = i;
                             tabu_best_number = 1;
                             Tabu_Temp[0] = i;
                          }
                          else if(Gamma[i] == tabu_best_diff)
                          {
                             Tabu_Temp[tabu_best_number] = i;
                             tabu_best_number++;
                         }
                    }
         }
         if(num == 0 || (( tabu_best_diff > bestdiff ) && ( f + tabu_best_diff > f_best)))
         {
              if(tabu_best_number > 1)
                tabu_best_flip =  Tabu_Temp[rand() % (tabu_best_number)] ;
              oldvalue = solution[tabu_best_flip];
              solution[tabu_best_flip] = 1 - solution[tabu_best_flip];
              UpdateGamma(tabu_best_flip,oldvalue);
              ChangeTabuTenure(tabu_best_flip,iter);
              f = f + tabu_best_diff;
         }
         else
         {
             if(best_number > 1)
                bestflip = Temp[rand() % (best_number) ];
             oldvalue = solution[bestflip];
             solution[bestflip] = 1 - solution[bestflip];
             UpdateGamma(bestflip,oldvalue);
             ChangeTabuTenure(bestflip,iter);
             f = f + bestdiff;
         }
         if(f > f_best)
         {
              f_best = f;
			  Iter = iter;
              for(int k = 0; k < VarNum ; k++)
				  best_solution[k] = solution[k];
              t_best = (clock() - start )/ CLOCKS_PER_SEC;
			  LocalOptimum = 0;
         }
		 else
		 {
			  LocalOptimum++;
			  if(LocalOptimum >= cutoff)
			  {
				  break;
			  }
		 }
     }
}

void Distribute_Memory_PR()
{
	RefSet = new int *[RefSize];
	for(int i = 0; i < RefSize; i++)
		RefSet[i] = new int[VarNum];
	Ref_f = new long long int[RefSize];
	f_index = new int[RefSize];
	Ref_f_copy = new long long int[RefSize];
	RefFlag = new int[RefSize];
	IndexNewSol = new int[RefSize];
	IndexOldSol = new int[RefSize];
	NonCommonSet = new int[VarNum];
	CommonSet = new int[VarNum];
    PV = new int[VarNum];
	PairSet = new int *[RefSize * (RefSize - 1) / 2];
	for(int i = 0; i < RefSize * (RefSize - 1) / 2; i++)
		PairSet[i] = new int[2];
}

void Generate_Pairs()
{
	int new_count = 0;
	int old_count = 0;
	for(int i = 0; i < RefSize; i++)
	{
		if(RefFlag[i] == 1)
			IndexNewSol[new_count++] = i;
		else
			IndexOldSol[old_count++] = i;
	}
    pair_count = 0;
	for(int i = 0; i < new_count; i++)
	{
		for(int j  = i +1; j< new_count; j++)
		{
			PairSet[pair_count][0] = IndexNewSol[i];
			PairSet[pair_count][1] = IndexNewSol[j];
			pair_count++;
		}
		for(int k = 0; k < old_count; k++)
		{
			PairSet[pair_count][0] = IndexNewSol[i];
			PairSet[pair_count][1] = IndexOldSol[k];
			pair_count++;
		}
	}
}

int Put_RefSet()
{
	int record_same = 0;
	for(int i = 0; i < current_size; i++)
	{
		if(Ref_f[i] == f_best)
		{
			int count = 0;
			for(int k = 0; k < VarNum; k++)
			{
				if(RefSet[i][k] == best_solution[k])
					count++;
			}
            if(count == VarNum)
			    record_same = 1;
		}
		if(record_same == 1)
			return 0;
	}
	return 1;
}

void Initial_RefSet()
{
	int count;
	for(int i = 0; i < VarNum; i++)
	    solution[i] = rand() % 2;
    TabuSearch();
	for(int j = 0; j < VarNum; j++)
		global_optimal_solution[j] = best_solution[j];
	ref_best = f_best;
	ref_best_t = t_best;
	for(int i = 0 ; i < VarNum; i++)
		RefSet[0][i] = best_solution[i];
	Ref_f[0] = f_best;
	current_size = 1;
	count = 1;
	while(current_size < RefSize && (clock() - start )/ CLOCKS_PER_SEC < set_time)
	{
		do
		{
			for(int i = 0; i < VarNum; i++)
	           solution[i] = rand() % 2;
			TabuSearch();
			count++;
		}while(Put_RefSet() == 0 && (clock() - start )/ CLOCKS_PER_SEC < set_time);
		if(ref_best < f_best)
		{
			ref_best = f_best;
			ref_best_t = t_best;
			ref_best_iter = Iter;
			for(int j = 0; j < VarNum; j++)
					 global_optimal_solution[j] = best_solution[j];
			printf("%d %d %lld %f\n", rebuild_count,  ref_best_iter, ref_best, ref_best_t);
		}
		Ref_f[current_size] = f_best;
		for(int i = 0 ; i < VarNum; i++)
		   RefSet[current_size][i] = best_solution[i];
        current_size++;
	}
	for(int i = 0; i < RefSize; i++)
	    RefFlag[i] = 1;
	for(int i = 0; i < RefSize; i++)
		{
            f_index[i] = i;
            Ref_f_copy[i] = Ref_f[i];
            }
	Quick_Sort(Ref_f_copy, RefSize, f_index);

}

void Rebuild_Initial_RefSet()
{
	int count;
	int check_value;

	for(int i = 0 ; i < VarNum; i++)
		RefSet[0][i] = RefSet[f_index[RefSize - 1]][i];
	Ref_f[0] = Ref_f[f_index[RefSize - 1]];

	current_size = 1;
	count = 1;
	while(current_size < RefSize  && (clock() - start )/ CLOCKS_PER_SEC < set_time)
	{
		do
		{
			for(int i = 0; i < VarNum; i++)
	           solution[i] = rand() % 2;
			TabuSearch();
			count++;
		}while(Put_RefSet() == 0  && (clock() - start )/ CLOCKS_PER_SEC < set_time);
		//printf("%d ", f_best);
		if(ref_best < f_best)
		{
			ref_best = f_best;
			ref_best_t = t_best;
			ref_best_iter = Iter;
			for(int j = 0; j < VarNum; j++)
					 global_optimal_solution[j] = best_solution[j];
			printf("%d %d %lld %f\n", rebuild_count,  ref_best_iter, ref_best, ref_best_t);
		}
		Ref_f[current_size] = f_best;
		for(int i = 0 ; i < VarNum; i++)
		   RefSet[current_size][i] = best_solution[i];
        current_size++;
	}
	for(int i = 0; i < RefSize; i++)
	    RefFlag[i] = 1;
	for(int i = 0; i < RefSize; i++)
	{
		f_index[i] = i;
		Ref_f_copy[i] = Ref_f[i];
	}
	Quick_Sort(Ref_f_copy, RefSize, f_index);

}

int UpdateRefSet(int *new_sol, int new_f)
{
	int i = 0;
	int min_index;

	while(Ref_f[i] != new_f && i < RefSize) i++;
	if(i < RefSize)
		return 0;
	else
	{
	    min_index = f_index[0];
		if(new_f < Ref_f_copy[0])
			return 0;
		for(int k = 0; k < RefSize;  k++)
		{
			if(new_f  > Ref_f_copy[k])
			{
				for(int j = 0; j < VarNum; j++)
				   RefSet[f_index[0]][j] = new_sol[j];
				Ref_f[f_index[0]] = new_f;
				RefFlag[f_index[0]] = 1;
				NewSolutions = 1;
				for(int i = 0; i < RefSize; i++)
		        {
                f_index[i] = i;
                Ref_f_copy[i] = Ref_f[i];
                }

                Quick_Sort(Ref_f_copy, RefSize, f_index);
				return 1;
			}
		}
		return 0;
	}
}

void InitDeltaTable(int *Delta, int *NC, int nc, int *sol)
{
     int diff;
	 int m,n;
     for(int i = 0; i < nc; i++)
       Delta[i] = 0;

     for(int i = 0; i < nc; i++)
     {
	    m = NC[i];
        diff = q[m][m];
        for(int j = 0; j < Degree[m] ; j++)
        {
		   n = Qadj[m][j];
           if(sol[n] == 1)
                diff = diff + q[m][n];
        }
        if((1 - sol[m]) == 0)
           diff = 0 - diff;
        Delta[i] = diff;
     }
}

void UpdateDelta(int *tag, int *Delta, int bestflip,int oldvalue, int nc, int *NC, int *x)
{
     for(int i = 0; i< nc; i++)
     {
           if( tag[i] != 1 && NC[i] != NC[bestflip])
           {
               if(q[NC[bestflip]][NC[i]] != 0)
               {
                    if(oldvalue == x[NC[i]])
                          Delta[i] = Delta[i]  + q[NC[bestflip]][NC[i]];
                    else
                         Delta[i] = Delta[i] - q[NC[bestflip]][NC[i]];
               }
           }
     }
}

int SelectSolutions(int *Path, int *delta, int nc)
{
	int interval_left, interval_right;
	int max = -INT_MAX;
	int max_index;
	interval_left = nc / 3;
	interval_right = 2* nc / 3;

	int rndValAssociatedToChosenDist;

	for(int m = 0; m < 1; m++)
	{
		for(int j = interval_left; j <= interval_right; j++)
		{
			if(delta[j] > max)
			{
				max = delta[j];
				max_index = j;
				rndValAssociatedToChosenDist = rand();
			}
			else
			 if(delta[j] == max)
			 {
                int candidateRndVal = rand();
				if (candidateRndVal>rndValAssociatedToChosenDist)
				{
				   rndValAssociatedToChosenDist = candidateRndVal;
				   max = delta[j];
				   max_index = j;
				}
             }
		}
	}
	return max_index;
}

void Construct_Path_Solution(int *pv, int k, int *x, int *y, int *NC, int *C, int nc, int c)
{
	int *flag;
	flag = new int[VarNum];
	for(int i = 0; i < VarNum; i++)
		flag[i] = 0;
	for(int i = 0; i <= k; i++)
	{
		solution[NC[pv[i]]] = y[NC[pv[i]]];
		flag[NC[pv[i]]] = 1;
	}
	for(int i = 0; i < VarNum; i++)
	{
		if(flag[i] != 1)
			solution[i] = x[i];
	}
	delete[] flag;
}

int check_part(int k, int *x, int *y, int *NC)
{
	int *flag;
	int *sol;
	int v;
	flag = new int[VarNum];
	sol = new int[VarNum];
	for(int i = 0; i < VarNum; i++)
		flag[i] = 0;
	for(int i = 0; i <= k; i++)
	{
		sol[NC[PV[i]]] = y[NC[PV[i]]];
		flag[NC[PV[i]]] = 1;
	}
	for(int i = 0; i < VarNum; i++)
	{
		if(flag[i] == 0)
			sol[i] = x[i];
	}

	v = InitialObjectValue2(sol);
	delete[] flag;
	delete[] sol;
	return v;
}
//
//void Relinking_Method(int *x, int *y)
//{
//	int *Delta, *path_delta;
//	int c, point;
//	int *tag;
//	int *index;
//	int *Delta_Copy;
//
//	common_count = 0;
//	non_common_count = 0;
//	for(int i = 0; i < VarNum; i++)
//	{
//		if(x[i] == y[i])
//			CommonSet[common_count++] = i;
//		else
//			NonCommonSet[non_common_count++] = i;
//	}
//
//	Delta = new int[non_common_count];
//	Delta_Copy = new int[non_common_count];
//	tag = new int[non_common_count];
//	index = new int[non_common_count];
//	path_delta = new int[non_common_count];
//
//    InitDeltaTable(Delta, NonCommonSet, non_common_count, x);
//	for(int i = 0; i < non_common_count; i++)
//		tag[i] = 0;
//	for(int k = 0; k < non_common_count -1; k++)
//	{
//		for(int i = 0; i < non_common_count; i++)
//		{
//			index[i] = i;
//			Delta_Copy[i]= Delta[i];
//		}
//		//Quick_Sort(Delta, non_common_count, index);
//		Quick_Sort(Delta_Copy, non_common_count, index);
//		c = non_common_count - 1;
//	    while(tag[index[c]] == 1) c--;
//		PV[k] = index[c];
//		if(k == 0)
//			path_delta[k] = Delta_Copy[c];
//		else
//			path_delta[k] = path_delta[k-1] + Delta_Copy[c];
//		tag[index[c]] = 1;
//		UpdateDelta(tag, Delta, index[c], x[NonCommonSet[index[c]]], non_common_count, NonCommonSet, x);
//	}
//	point = SelectSolutions(PV, path_delta, non_common_count);
//	Construct_Path_Solution(PV, point, x, y, NonCommonSet, CommonSet, non_common_count,common_count);
//	delete[] Delta;
//	delete[] Delta_Copy;
//	delete[] tag;
//	delete[] index;
//}


void Relinking_Method_3(int *x, int *y)
{
	int *Delta, *path_delta;
	int c, point;
	int *tag;

	common_count = 0;
	non_common_count = 0;
	for(int i = 0; i < VarNum; i++)
	{
		if(x[i] == y[i])
			CommonSet[common_count++] = i;
		else
			NonCommonSet[non_common_count++] = i;
	}

	Delta = new int[non_common_count];
	tag = new int[non_common_count];
	path_delta = new int[non_common_count];

    InitDeltaTable(Delta, NonCommonSet, non_common_count, x);
	for(int i = 0; i < non_common_count; i++)
		tag[i] = 0;
	for(int k = 0; k < non_common_count -1; k++)
	{
        do
        {
           c = rand() % non_common_count;
        }while(tag[c]);
		PV[k] = c;
		if(k == 0)
			path_delta[k] = Delta[c];
		else
		    path_delta[k] = path_delta[k-1] + Delta[c];
		tag[c] = 1;
		UpdateDelta(tag, Delta, c, x[NonCommonSet[c]], non_common_count, NonCommonSet, x);
	}
	point = SelectSolutions(PV, path_delta, non_common_count);
	Construct_Path_Solution(PV, point, x, y, NonCommonSet, CommonSet, non_common_count,common_count);

	delete[] Delta;
	delete[] tag;
	delete[] path_delta;
}


void PR_Procedure()
{
	int *init_sol, *guid_sol;
	int init_sol_index, guid_sol_index;
	int NewSolutions;
	int update_succ;
	int init_f, guid_f;
	init_sol = new int[VarNum];
	guid_sol = new int[VarNum];

	Distribute_Memory_PR();
    rebuild_count = 0;
	while((clock() - start )/ CLOCKS_PER_SEC < set_time)
	{
		if(rebuild_count == 0)
			Initial_RefSet();
		else
			Rebuild_Initial_RefSet();
		NewSolutions = 1;
	    while(NewSolutions == 1)
	    {
			//cutoff = 500;
		    Generate_Pairs();
		    for(int i = 0; i < RefSize; i++)
			    RefFlag[i] = 0;
		     NewSolutions = 0;
		    for(int i = 0; i < pair_count; i++)
	        {
			     init_sol_index = PairSet[i][0];
			     guid_sol_index = PairSet[i][1];
			     for(int j = 0; j < VarNum; j++)
			     {
				       init_sol[j] = RefSet[init_sol_index][j];
				       guid_sol[j] = RefSet[guid_sol_index][j];
			     }

                 //Relinking_Method(init_sol, guid_sol);
				 Relinking_Method_3(init_sol, guid_sol);
			     TabuSearch();
				 if(ref_best < f_best)
				 {
					ref_best = f_best;
					ref_best_t = t_best;
					ref_best_iter = Iter;
					for(int j = 0; j < VarNum; j++)
							global_optimal_solution[j] = best_solution[j];
					printf("%d %d %lld %f\n", rebuild_count,  ref_best_iter, ref_best, ref_best_t);
				 }
			     update_succ = UpdateRefSet(best_solution, f_best);

				 //Relinking_Method(guid_sol, init_sol);
				 Relinking_Method_3(guid_sol, init_sol);
			     TabuSearch();
				 if(ref_best < f_best)
				 {
					ref_best = f_best;
					ref_best_t = t_best;
					ref_best_iter = Iter;
					for(int j = 0; j < VarNum; j++)
							global_optimal_solution[j] = best_solution[j];
					printf("%d %d %lld %f\n", rebuild_count,  ref_best_iter, ref_best, ref_best_t);
				 }
			     update_succ = UpdateRefSet(best_solution, f_best);
	       }
		}
	       rebuild_count++;
	 }
	delete[] init_sol;
	delete[] guid_sol;
}


int calculateXqx(char *file)
{
    char * outfilename;

    seed = time(NULL) % 1000000 ;
    srand(seed);
//	if (argc == 4)
//     {
//           fichier = argv[1];
//		   //seed = 1000;
//		   seed = time(NULL) % 1000000 ;
//		   srand(seed);
//		   outfilename = argv[2];
//		   set_time = atof(argv[3]);
//     }
//     else
//     {
//           cout << endl << "### Command Line Parameters Error " << endl;
//           cout << "ExecutiveFileName InputFileName OutputFileName TimeLimit" << endl;
//           exit(0);
            fichier=file;
            outfilename="out.txt";
            set_time=600;
//     }

     cout << endl;
     cout << "Name of Input File = " << fichier << endl;
     cout << "Name of Output File = " << outfilename << endl;
     cout << "Time Limit  = " << set_time << endl;

	 start = clock();
     InitData();
	 cutoff = 5 * VarNum;
	 tc = VarNum / 100;

	 global_optimal_solution = new int[VarNum];

	 ref_best = -INT_MAX;
     int rs_times = 0;
	 fp = fopen(outfilename,"a+");
	 printf("************************\n");

	 PR_Procedure();

     printf("\n");
     printf("%d %d %d %lld %.2f \n", seed, rebuild_count, ref_best_iter, ref_best, ref_best_t);
	 fprintf(fp, "%d %d %d %lld %.2f\n", seed, rebuild_count, ref_best_iter, ref_best, ref_best_t);
	 fprintf(fp, "Best Solution :\n");
	 for(int j = 0; j < VarNum; j++)
		fprintf(fp, "%d ", global_optimal_solution[j]);
     fprintf(fp, "\n");
     delete[] global_optimal_solution;
	 fclose(fp);
	 return 1;
}
