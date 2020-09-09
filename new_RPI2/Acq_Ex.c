/* Exemple de taches periodiques, avec detection des overruns */

#include <stdio.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

#define C10MSEC 10000000 // 10ms en nano-sec //

#define LBUF 326
long long unsigned int buffer[LBUF*2];
//////////////
// ECRITURE //
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
int fid;
char fBuffer[100];
void ecritfic(char *b){
	write(fid, b, strlen(b));
}


/////////////
static void fct_task(void *arg){
	volatile unsigned int i=0;
//	unsigned int n=0;
//	unsigned long int out=0;
	unsigned long long int start = 0;
	RT_TIMER_INFO timer_info;

	if(rt_task_set_periodic(
				NULL,	// La tache courrante
				TM_NOW,	// Delai de mise en place, TM_NOW = none
				C10MSEC // Valeur en nanosecondes
			       ) != 0){
		printf("ERROR : fun. rt_task_set_periodic()\n");
		return;
	}
//////////////////////////////////////////////
	rt_timer_inquire(&timer_info);
	start = timer_info.date/C10MSEC;

	while(i < 650){
		rt_task_wait_period(NULL);
		rt_timer_inquire(&timer_info);
		//printf("Value : %llu\n", timer_info.date/C10MSEC - start);
		buffer[i] = (timer_info.date/C10MSEC - start);
		i++;
	}
//////////////////////////////////////////////
/*
	rt_task_set_periodic
	rt_task_wait_period();
		rt_timer_inquire();
*/
//////////////////////////////////////////////
/*
	rt_timer_inquire(&timer_info);

	start = timer_info.date/C10MSEC;

	while(n < 30){
		// Secondary mode //
		rt_timer_inquire(&timer_info);

		// Affichage d'un message toutes les secondes //
		printf("Message #%u t= %llu s\n", n, timer_info.date/C10MSEC - start );
		n++;

		out = 0;

		// Retour en primary mode a cause d'un appel systeme Xenomai //
		switch( rt_task_wait_period(&out) ){

			case 0:
			case -ETIMEDOUT:
				if(out){
					printf("Overrun: %lu cycles perdus\n", out); // Secondary mode
				}
				break;
			default:
				printf("ERROR : fun. rt_task_wait_period()\n"); // Secondary mode
				return;
		}//END SWITCH
	}//END WHILE
*/
}//END FUNCTION

int main(int N, char*P[]){

	RT_TASK taskID;
	int n, i;

	// On empeche le swap //
	mlockall(MCL_CURRENT | MCL_FUTURE);

	if( (n = rt_task_spawn(&taskID, "maTache", 0, 99, T_JOINABLE, &fct_task, NULL)) !=0 ){
		printf("ERROR : fun. rt_task_spawn() #%d\n", n);
		return 1;
	}

	rt_task_join(&taskID);

	printf("SAVING DATAS \n");

	if( (fid = creat("sinus.xg",0644)) == -1 ){
		perror("Erreur creation sinus.xg");
		//return;
		return 1;
	}
	sprintf(fBuffer, "TitleText: Courbe de sinus\n\"Sin(t)\"\n");
	ecritfic(fBuffer);
	for(i=0 ; i < 650 ; i++){
		sprintf(fBuffer, "%d, %g\n", i, sin((double)buffer[i]*0.01));
		ecritfic(fBuffer);
	}
	ecritfic("\n");
	close(fid);

	return 0;
}

