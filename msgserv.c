#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char * argv[])
{

	char name[100], ip[20], siip[20], sipt[20];
	uint upt, tpt;
	int fd, addrlen, ret, nread, m ,r, port_id, siipi, i;
	struct sockaddr_in addr;
	char buffer[128];
	struct hostent *h;
	struct in_addr *a;

//Open UDP socket
	fd=socket(AF_INET,SOCK_DGRAM,0);
	if(fd==-1){
		printf("Error fd=socket\n");
		exit(1);//error
	}
//Clear addr for use
	memset((void*)&addr,(int)'\0',sizeof(addr));
//Specify family
	addr.sin_family=AF_INET;

//Begin to separate input arguments. With or whitout optional input arguments.
	if((argc < 9)){
		printf("Invalid input: Not enough arguments\n");
		exit(1);
	}else{ 
		if((strcmp(argv[1],"-n")!=0)||(strcmp(argv[3],"-j")!=0)||(strcmp(argv[5],"-u")!=0)||(strcmp(argv[7],"-t")!=0)){
			printf("Invalid input: Wrong arguments\n");
			exit(1);
		}else{
//If input checks out, start to attribute default input arguments
			
			strcpy(name, argv[2]);
			strcpy(ip, argv[4]);
			upt = atoi(argv[6]);
			tpt = atoi(argv[8]);
			
			h=gethostbyname("tejo.tecnico.ulisboa.pt");
			
			if(h==NULL){
				printf("Error getting siip\n");
				exit(1);
	
			}
	
			a=(struct in_addr*)h->h_addr_list[0];
			
			addr.sin_addr.s_addr=a->s_addr;
			
			addr.sin_port=htons(59000);
			
			m=200;
			
			r=10;
		}
	}
//If optional input arguments are given by the user, utilise them
	for(i=0;i=argc;i++){
		if(strcmp(argv[i],"-i")==0){
			siipi=atoi(argv[i+1]);
			addr.sin_addr.s_addr=htonl(siipi);
		}
		if(strcmp(argv[i],"-p")==0){
			port_id=atoi(argv[i+1]);
			addr.sin_port=htons(port_id);
		}
		if(strcmp(argv[i],"-m")==0){
			m=atoi(argv[i+1]);
		}
		if(strcmp(argv[i],"-r")==0){
			r=atoi(argv[i+1]);
		}

	}

	//PAREI AQUI 3/3/17

	ret=bind(fd,(struct sockaddr*)&addr,sizeof(addr));
	if(ret==-1){
		printf("Error ret=bind\n");
		exit(1);//error
	}

	while(1){
		addrlen=sizeof(addr);
		nread=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
		if(nread==1){
			printf("Error nread\n");
			exit(1);//error
		}
		ret=sendto(fd,buffer,nread,0,(struct sockaddr*)&addr,addrlen);
		if(ret==-1){
			printf("Error ret\n");
			exit(1);//error
		}
	}
	
close(fd);
exit(0);
}