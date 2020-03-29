/* serv_TR_3S3.c : TODO(Add TITILE) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

struct sockaddr_in Sin = {AF_INET};	//Le reste est nul

struct elt_dns{
	char nom[30];
	char adip[20];

} annuaire [] = {
	{"chaource", "192.168.99.1"},
	{"roquefort", "192.168.99.2"},
	{"morbier", "192.168.99.3"},
	{"abondance", "192.168.99.4"},
	{"reblochon", "192.168.99.5"},
	{"brie", "192.168.99.6"},
	{"parmesan", "192.168.99.7"},
	{"gorgonzola", "192.168.99.8"},
	{"", ""}
};

int readlig(int fd, char *buffer, int max){

	int n;
	char c;

	for(n=0 ; n<max ; n++){
		if( read(fd, &c, 1) <= 0 ){
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

#define LBUFFER 100
void * service(void *parameter){

	long sid;
	int n, i;
	char buffer[LBUFFER];

	printf("New connection detected\n");

	sid = (long)parameter;
	n = readlig(sid, buffer, LBUFFER);

	if(n < 0){
		perror("ERROR: fun. readlig()");
		pthread_exit(NULL);
	}

	i = 0;

	while(strlen(annuaire[i].nom) > 0){
		if( strcmp(buffer, annuaire[i].nom) == 0 ){
			write(sid, annuaire[i].adip, 20);
			close(sid);
			pthread_exit(NULL);
		}
		i++;
	}
	write(sid, "erreur !! ", 11);
	close(sid);
	pthread_exit(NULL);
}

int main(int N, char *P[]){

	int ln;
	int sock;
	long nsock;

	pthread_t threadID;

	// Creation du socket //
	if( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
		perror("ERROR: fun. socket()");
		exit(1);
	}

	// Attachement au PORT //
	if( bind(sock, (struct sockaddr*)&Sin, sizeof(Sin)) < 0 ){
		perror("ERROR: fun. bind()");
		exit(2);
	}

	ln = sizeof(Sin);

	// Recuperation du nom du socket //
	if( getsockname(sock, (struct sockaddr*)&Sin, (socklen_t*)&ln) < 0 ){
		perror("ERROR: fun. getsockname()");
		exit(3);
	}

	printf("Le serveur est attache au PORT %u\n", ntohs(Sin.sin_port) );

	// Definition du nombre d'appel simultanes (ici 5) //
	if( listen(sock, 5) < 0 ){
		perror("ERROR: fun. listen()");
		exit(4);
	}

	// Boucle d'attente //
	for(;;){
		if( (nsock = accept(sock, (struct sockaddr*)&Sin, (socklen_t*)&ln)) < 0 ){
			perror("ERROR: fun. accept()");
			exit(5);
		}
		// Creation du thread qui va rendre le service //
		if( pthread_create(&threadID, NULL, service, (void*)nsock) < 0){
			fprintf(stderr, "Erreur creation du thread !\n");
			exit(6);
		}
		// On libere le thread //
		pthread_detach(threadID);
	}//END FOR
}//END MAIN
