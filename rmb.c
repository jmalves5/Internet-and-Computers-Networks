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
	int fd1, fd2, n, i, port_id, siipi, addrlen, ret, nread;
	struct sockaddr_in addr1, addr2;
	char buffer1[2048],buffer2[2048],buffer3[2048];
	struct hostent *h;
	struct in_addr *a;


	fd1=socket(AF_INET,SOCK_DGRAM,0);
	fd2=socket(AF_INET,SOCK_DGRAM,0);
	if(fd1==-1||fd2==-1){
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
	printf("%s\n",inet_ntoa(addr1.sin_addr));

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
	ret=sendto(fd1,"GET_SERVERS",256,0,(struct sockaddr*)&addr1,addrlen);
	if(ret==-1){
		printf("Error ret 1\n");
		exit(1);//error
	}
	nread=recvfrom(fd1,buffer2,256,0,(struct sockaddr*)&addr1,&addrlen);
	if(nread==-1){
		printf("Error nread 1\n");
		exit(1);//error
	}
	printf("%s\n",buffer2 );

	
	

//User interface
	while(1){

		fgets(buffer1,2048,stdin);
		

		if(strstr(buffer1, "show_servers")!=NULL){
			addrlen=sizeof(addr1);
			ret=sendto(fd1,"GET_SERVERS",256,0,(struct sockaddr*)&addr1,addrlen);
			if(ret==-1){
				printf("Error ret\n");
				exit(1);//error
			}
			nread=recvfrom(fd1,buffer2,256,0,(struct sockaddr*)&addr1,&addrlen);
			if(nread==-1){
				printf("Error nread\n");
				exit(1);//error
			}
			printf("%s\n",buffer2 );
		}
		

		if(strstr(buffer1, "exit")!=NULL){
				printf("Program exited successfully\n");
				exit(0);
		}
	}

close(fd1);
close(fd2);
exit(0);

}

