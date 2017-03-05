#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char * argv[]){

	char siip[20], sipt[20];
	int fdi, fdm, n, i, port_id, siipi, addrlen, ret, nread, pub, count, ip,port;
	struct sockaddr_in addr1, addr2;
	char buffer1[2048],buffer2[2048],buffer3[2048], *b,*m, *e;
	char colon=';';
	char ms_ip[128],ms_uport[128];
	char message[140];
	struct hostent *h;
	struct in_addr *a;
	struct in_addr *a2;



	fdi=socket(AF_INET,SOCK_DGRAM,0);
	fdm=socket(AF_INET,SOCK_DGRAM,0);
	if(fdi==-1||fdm==-1){
		exit(1);
	};


//If input checks out, start to attribute default input arguments
		
	h=gethostbyname("tejo.tecnico.ulisboa.pt");		
	if(h==NULL){
		printf("Error getting siip\n");
		exit(1);
	
	}
	a=(struct in_addr*)h->h_addr_list[0];
	
	memset((void*)&addr1,(int)'\0',sizeof(addr1));
	addr1.sin_family=AF_INET;
	addr1.sin_addr=*(struct in_addr*) h->h_addr_list[0];
	addr1.sin_port=htons(59000);
	
 //If optional input arguments are given by the user, utilise them

	for(i=0;i<=argc-1;i++){
		
		if(strcmp(argv[i],"-i")==0){
			siipi=atoi(argv[i+1]);
			addr1.sin_addr.s_addr=htonl(siipi);
		}
		if(strcmp(argv[i],"-p")==0){
			port_id=atoi(argv[i+1]);
			addr1.sin_port=htons(port_id);
		}		
	}
	


//Get message servers names to connect
	addrlen=sizeof(addr1);
	ret=sendto(fdi,"GET_SERVERS",256,0,(struct sockaddr*)&addr1,addrlen);
	if(ret==-1){
		printf("Error ret 2\n");
		exit(1);//error
	}
	nread=recvfrom(fdi,buffer2,256,0,(struct sockaddr*)&addr1,&addrlen);
	if(nread==-1){
		printf("Error nread 2\n");
		exit(1);//error
	}
	
//Message server attribution
	b=strchr(buffer2,';');
	if(b==NULL){
		printf("no message servers available\n");

		exit(0);
	}
	m=strchr(b+1,';');
	e=strchr(m+1,';');

	strncpy(ms_ip,b+1, m-b-1);
	strncpy(ms_uport, m+1, e-m-1);

	
	port=atoi(ms_uport);
	printf("%d\n", port);

	memset((void*)&addr2,(int)'\0',sizeof(addr2));
	addr2.sin_family=AF_INET;
	inet_pton(AF_INET,ms_ip,&(addr2.sin_addr));
	addr2.sin_port=htons(port);

	inet_ntop(AF_INET,&(addr2.sin_addr),ms_ip,INET_ADDRSTRLEN);
	printf("%s\n",ms_ip);


//User interface

	while(1){

		fgets(buffer1,2048,stdin);
		


//Implementing the user's commands

		if(strstr(buffer1, "show_servers")!=NULL){
			addrlen=sizeof(addr1);
			ret=sendto(fdi,"GET_SERVERS",256,0,(struct sockaddr*)&addr1,addrlen);
			if(ret==-1){
				printf("Error ret\n");
				exit(1);//error
			}
			nread=recvfrom(fdi,buffer2,256,0,(struct sockaddr*)&addr1,&addrlen);
			if(nread==-1){
				printf("Error nread\n");
				exit(1);//error
			}
			printf("%s\n",buffer2);
		}

		if(strstr(buffer1, "publish")!=NULL){
			strcpy(message,strstr(buffer1," "));
			pub=sendto(fdm,message,140,0,(struct sockaddr*)&addr2,addrlen);
			exit(1);
			if(pub==-1){
				printf("Error pub\n");
				exit(1);
			}
		}
		
	
		if(strstr(buffer1, "exit")!=NULL){
				printf("Program exited successfully\n");
				exit(0);
		}
	}

close(fdi);
close(fdm);
exit(0);

}

