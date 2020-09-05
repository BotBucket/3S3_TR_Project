/* client_tcp.c : Exemple d'un client socket mode connecte */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

struct sockaddr_in Sin = {AF_INET};	//Le reste est nul

#define COM_LBUFFER 100
//#define ACQ_LBUFFER 325*8
#define ACQ_LBUFFER 650*4

char comBuffer[COM_LBUFFER];
int acqBuffer[ACQ_LBUFFER];
int datas[ACQ_LBUFFER];

int fileID;
char fileBuffer[100];

void writeInFile(char *c){
	write(fileID, c, strlen(c));
}


int main(int N, char *P[]){

	int sock;
	int n;
	struct hostent *h;

	// Vérification des paramètres passés au programme //
	if(N != 3){
		fprintf(stderr, "Utilisation : \"%s @SERVEUR #PORT\" !\n", P[0]);
		exit(1);
	}

	// Creation du socket //
	if( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
		perror("ERROR: fun. socket()");
		exit(2);
	}

	// Initialisation de la structure Sin en définissant le #PORT et @SERVEUR//
	Sin.sin_port = htons( atoi(P[2]) );

	if( (h=gethostbyname(P[1])) == NULL ){
		fprintf(stderr, "ERRO: #%d fun. gethostbyname() \n", h_errno);
		exit(3);
	}

	// Copie des informations par Block Mémoire //
	bcopy((void*)h->h_addr, (void*)&(Sin.sin_addr.s_addr), h->h_length);

	// Débuter la connexion du socket //
	if( connect(sock, (struct sockaddr*)&Sin, sizeof(Sin)) < 0){
		perror("ERROR: fun. connect()");
		exit(4);
	}

	// Affichage //
	printf("Connexion etablie !\n");
	printf("************************************\n");
	printf("*   Bienvenue sur le serveur 3S3   *\n");
	printf("************************************\n");

	printf("Selectionnez une action :\n");
	printf("    1.Lancer une acquisition\n");
//	printf("    2.Tracer une courbe avec xgraph\n");
	printf("    0.Quitter\n");

	// Récupération du choix de l'utilisateur //
//	userChoice = getchar();

	if( fgets(comBuffer, COM_LBUFFER, stdin) > 0 ){
		write(sock, comBuffer, strlen(comBuffer));
	}else{
		fprintf(stderr, "Erreur de saisie !\n");
		exit(5);
	}

	n = read(sock, acqBuffer, ACQ_LBUFFER);

	printf("n = %d\n", n);

	acqBuffer[n] = '\0';

	close(sock);
/*	for(i=0 ; i < 650 ; i++){
		printf("acqBuffer[%d] = %d\n", i, acqBuffer[i]);
	}
*/
//	memcpy(datas, acqBuffer, 650);
/*	for(i=0 ; i < 650 ; i++){
		printf("datas[%d] = %d\n", i, acqBuffer[i]);
	}
*/

	// Ecriture du fichier xgraph //
	printf("SAVING DATAS \n");

	if( (fileID = creat("sinus.xg",0644)) == -1 ){
		perror("Erreur creation sinus.xg");
		//return;
		return 1;
	}

	sprintf(fileBuffer, "TitleText: Courbe de sinus\n\"Sin(t)\"\n");

	writeInFile(fileBuffer);

	int i = 0;
	for(i=0 ; i < 650 ; i++){
		sprintf(fileBuffer, "%d, %g\n", i, sin((double)acqBuffer[i]*0.01));
		writeInFile(fileBuffer);
	}
	writeInFile("\n");
	close(fileID);

	system("/usr/bin/xgraph sinus.xg");

	return 0;
}
