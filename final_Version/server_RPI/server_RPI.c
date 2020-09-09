/* Exemple de mise à niveau de la tache main() avec synchronisation par semaphore */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/sem.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>


// VARIABLES GLOBALES //
static RT_SEM semProd;
static RT_SEM semCons;

#define C10MSEC 10000000 // 10ms en nano-sec //
//#define BUFFER_SIZE 650  // Taille du buffer partagé - 1 période correspond à 650 valeurs donc pour 2 Periodes 650 x 2 = 1300 //
#define BUFFER_SIZE 1300  // Taille du buffer partagé - 1 période correspond à 650 valeurs donc pour 2 Periodes 650 x 2 = 1300 //

int flag = 0;

int acqBuffer[BUFFER_SIZE];
int sharedBuffer_01[BUFFER_SIZE];
int sharedBuffer_02[BUFFER_SIZE];
int sharedBuffer_03[BUFFER_SIZE];
int buffPos = 0, buffNum = 1;


// Fonction du consommateur //
static void fct_cons(void *parameter){

	int error;
	int i_cons = 0;

	while(1){

		// Consommateur - semP //
		if( (error = rt_sem_p(&semCons, TM_INFINITE)) != 0 ){
			printf("ERROR: fun. rt_sem_p() %d\n", error);
			return ;
		}

		// Consommer a partir de T[ic]; //
		//ATTENTION PAS DE PRINTF DANS LE PROJET
		if( buffNum == 1 ){
//			printf("CONS: Je consomme la case #%d du buffer #%d: %d!\n", i_cons, buffNum, sharedBuffer_01[i_cons]);
			sharedBuffer_01[i_cons] = acqBuffer[i_cons];
		}else if( buffNum == 2 ){
//			printf("CONS: Je consomme la case #%d du buffer #%d: %d!\n", i_cons, buffNum, sharedBuffer_02[i_cons]);
			sharedBuffer_02[i_cons] = acqBuffer[i_cons];

		}else if( buffNum == 3 ){
//			printf("CONS: Je consomme la case #%d du buffer #%d: %d!\n", i_cons, buffNum, sharedBuffer_03[i_cons]);
			sharedBuffer_03[i_cons] = acqBuffer[i_cons];

		}

		// Consommateur - semV //
		if( (error = rt_sem_v(&semProd)) != 0 ){
			printf("ERROR: fun. rt_sem_v() %d\n", error);
			return ;
		}

		i_cons += 1;

		// En fin de buffer on réinitialise l'indice i_cons et on change de buffer de stockage //
		if( i_cons == BUFFER_SIZE ){
			i_cons = 0;
			if( buffNum == 3 ){
				buffNum = 1;
			}else if( buffNum != 3 ){
				buffNum++;
			}
		}
	}
	return;
}


// Fonction du producteur - Effectue les acquisition //
static void fct_prod(void *parameter){

	int error;
	int i_prod = 0;
	unsigned long long int start = 0;
	RT_TIMER_INFO timer_info;

	// Setting task Periodic //
	if(rt_task_set_periodic(
				NULL,   // La tache courrante
				TM_NOW, // Delai de mise en place, TM_NOW = none
				C10MSEC // Valeur en nanosecondes
				) != 0){
		printf("ERROR : fun. rt_task_set_periodic()\n");
		return;
	}

	// Starting Acquisition //
	rt_timer_inquire(&timer_info);
	start = timer_info.date/C10MSEC;

	while(1){

		// Producteur - semP //
		if( (error = rt_sem_p(&semProd, TM_INFINITE)) != 0 ){
			printf("ERROR: fun. rt_sem_p() %d\n", error);
			return ;
		}

		// Produire dans T[ip]; //
		rt_task_wait_period(NULL);
		rt_timer_inquire(&timer_info);


		// Sauvegarde de la valeur acquise dans le buffer d'acquisition //
		acqBuffer[i_prod] = (timer_info.date/C10MSEC - start);

		// Producteur - semV //
		if( (error = rt_sem_v(&semCons)) != 0 ){
			printf("ERROR: fun. rt_sem_v() %d\n", error);
			return ;
		}

		if( i_prod != BUFFER_SIZE ){
			i_prod++;
		}else{
			i_prod = 0;
			flag = 1;
		}
	}//END WHILE
	return;
}


// Fonction de récuperation des saisies utilisateur du côté client_PC //
#define LBUFF 4
int readlig(int fileDescriptor, char *buffer, int max){

	int n;
	char c;

	for(n=0 ; n < max ; n++){
		if( read(fileDescriptor, &c, 1) <= 0 ){
			break;
		}
		if(c == '\n'){
			break;
		}
		*buffer++ = c;
	}
	*buffer = '\0';
	return(n);
}

// Fonction mise à disposition par le serveur (i.e qu'il execute à chaque nouveau socket détecté) //
void * service(void *parameter){

	char buffer[LBUFF];
	long socketID = (long)parameter;
	int n;

	printf("New incomming socket detected !\n");
	// Reading data from socket //
	n = readlig(socketID, buffer, LBUFF);

	if( n < 0){
		perror("ERROR: fun. read()");
		pthread_exit(NULL);
	}

	// Comparaison du choix de l'utilisateur //
	if( strcmp(buffer, "1") == 0 ){

		// Attente d'une première acquisition complète //
		while( flag != 1 ){
			// DO NOTHING
		}

		if( buffNum == 1 ){
			n = write(socketID, sharedBuffer_03, sizeof(sharedBuffer_03) );
			printf("sharedBuffer_0%d sended !\n", buffNum-1);
		}else if( buffNum == 2 ){
			n = write(socketID, sharedBuffer_01, sizeof(sharedBuffer_01) );
			printf("sharedBuffer_0%d sended !\n", buffNum-1);
		}else if( buffNum == 3 ){
			n = write(socketID, sharedBuffer_02, sizeof(sharedBuffer_02) );
			printf("sharedBuffer_0%d sended !\n", buffNum-1);
		}
		//printf(" buffNum_%d - n = %d\n", buffNum, n);

////		close(socketID);
////		pthread_exit(NULL);

	}else{
////		close(socketID);
////		pthread_exit(NULL);
	}

	close(socketID);
	pthread_exit(NULL);
}

int main(int N, char *P[]){

	RT_TASK task_Prod, task_Cons;
	int error;

	int server_fd, newSocket;
	struct sockaddr_in address = {AF_INET};
	int nameLen = sizeof(address);
	pthread_t threadID;



	// Verouillage du sgement pour eviter le swap //
	mlockall(MCL_CURRENT | MCL_FUTURE);

	printf("Debut du programme\n");

	// Creation des Semaphores TR //
	// semProd //
	if( rt_sem_create(&semProd, "SemaphoreProd", BUFFER_SIZE, S_FIFO) != 0 ){
		printf("ERROR: fun. rt_sem_create()\n");
		return 1;
	}
	// semCons //
	if( rt_sem_create(&semCons, "SemaphoreCons", 0, S_FIFO) != 0 ){
		printf("ERROR: fun. rt_sem_create()\n");
		return 1;
	}
	// prodTask //
	if( (error = rt_task_spawn(&task_Prod, "Prod_task", 0, 99, 0, &fct_prod, NULL)) != 0 ){
		printf("ERROR: fun. rt_task_spawn() %d\n", error);
		return 1;
	}
	// consTask //
	if( (error = rt_task_spawn(&task_Cons, "Cons_task", 0, 99, 0, &fct_cons, NULL)) != 0 ){
		printf("ERROR: fun. rt_task_spawn() %d\n", error);
		return 1;
	}

	// Création d'un socket //
	if( (server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
		perror("ERROR: fun. socket()");
		exit(1);
	}
	// Attachement du socket au PORT //
	if( bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0 ){
		perror("ERROR: fun. bind()");
		exit(2);
	}
	// Récupératon du nom du socket //
	if( getsockname(server_fd, (struct sockaddr*)&address, (socklen_t*)&nameLen) < 0 ){
		perror("ERROR: fun. getsockname()");
		exit(3);
	}
	// Affichage du #PORT dans le terminal //
	printf("Server is active on PORT %u\n", ntohs(address.sin_port) );
	// Attente de connexion sur la socket & Definition du nombre d'appel simultanes (ici 3) //
	if( listen(server_fd, 3) < 0 ){
		perror("ERROR: fun. listen()");
		exit(4);
	}

	// Boucle d'attente d'une connexion //
	for(;;){
		if( (newSocket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&nameLen)) < 0){
			perror("ERROR: fun. accept()");
			exit(5);
		}
		// Création du thread qui va rendre le service au socket //
		if( pthread_create(&threadID, NULL, service, (void*)newSocket) < 0){
			fprintf(stderr, "ERROR: fun. pthread_create()\n");
			exit(6);
		}
		// Liberation du thread après qu'il est rendu le service //
		pthread_detach(threadID);
	}//END FOR
	return 0;
}//END MAIN
