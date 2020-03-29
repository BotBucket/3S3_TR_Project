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


struct sockaddr_in Sin = {AF_INET};	//Le reste est nul

#define LBUFFER 100
char buffer[LBUFFER];

int main(int N, char *P[]){

	int sock;
	int n;
	struct hostent *h;
	char userChoice;

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
	printf("    2.Tracer une courbe avec xgraph\n");
	printf("    0.Quitter\n");

	// Récupération du choix de l'utilisateur //
//	userChoice = getchar();

	if( fgets(buffer, LBUFFER, stdin) > 0 ){
		write(sock, buffer, strlen(buffer));
	}else{
		fprintf(stderr, "Erreur de saisie !\n");
		exit(5);
	}

	n = read(sock, buffer, LBUFFER);

	buffer[n] = '\0';

	printf("SERVER: %s\n", buffer);

	close(sock);

	return 0;
/*
	if( userChoice == '1' ){
		printf(">> ACQUISITION <<\n");

		if( fgets(buffer, LBUFFER, stdin) > 0){
			write(sock, buffer, strlen(buffer));
		}else{
			fprintf(stderr, "Erreur de saisie !\n");
			exit(5);
		}

		n = read(sock, buffer, LBUFFER);

		buffer[n] = '\0';

		printf("Reponse du serveur : %s\n", buffer);

		close(sock);

		return 0;

	}else if( userChoice == '2' ){
		printf(">> TRACE XGRAPH <<\n");
		//system("./trace_xgraph.sh");
		close(sock);
		return 0;

	}else if( userChoice == '0'){
		printf(">> QUITTER <<\n");
		close(sock);
		return 0;

	}else{
		printf("Mauvaise saisie... Fin du programme");
		close(sock);
		return 0;
	}
*/
}
