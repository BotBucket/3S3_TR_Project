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

#define COM_LBUFFER 300
#define ACQ_LBUFFER 1300*4

char comBuffer[COM_LBUFFER];
int acqBuffer[ACQ_LBUFFER];

int fileID;
char fileBuffer[100];

void writeInFile(char *c){
	write(fileID, c, strlen(c));
}


int main(int N, char *P[]){

	int sock;
	int n, compChar = 0;
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

	// Message de connexion du serveur //
	n = read(sock, comBuffer, COM_LBUFFER);
	printf("%s", comBuffer);
	n = read(sock, comBuffer, COM_LBUFFER);
	printf("%s", comBuffer);

	// Récupération du choix de l'utilisateur //
	if( fgets(comBuffer, COM_LBUFFER, stdin) > 0 ){

		if( strcmp(comBuffer, "0\n") == 0 ){
			close(sock);
			printf("Exiting Program !\n");
			return 0;
		}
		write(sock, comBuffer, strlen(comBuffer));

	}else{
		fprintf(stderr, "Erreur de saisie !\n");
		exit(5);
	}

	// Récupération des données envoyées par le serveur //
	while ( compChar < ACQ_LBUFFER && n != -1){
			n = read(sock, (void *)acqBuffer+compChar, ACQ_LBUFFER);
			compChar += n;
			//printf("n = %d, read : %d\n", n, compChar);
	}
	acqBuffer[n] = '\0';
	close(sock);

	// Ecriture du fichier xgraph //
	printf("Datas saved !\n");

	// Création du fichier xgraph //
	if( (fileID = creat("sinus.xg",0644)) == -1 ){
		perror("Erreur creation sinus.xg");
		//return;
		return 1;
	}

	sprintf(fileBuffer, "TitleText: Courbe de sinus\n\"Sin(t)\"\n");

	writeInFile(fileBuffer);

	int i = 0;
	for(i=0 ; i < 1300 ; i++){
		sprintf(fileBuffer, "%d, %g\n", i, sin((double)acqBuffer[i]*0.01));
		writeInFile(fileBuffer);
	}
	writeInFile("\n");
	close(fileID);

	system("/usr/bin/xgraph sinus.xg");

	return 0;
}
