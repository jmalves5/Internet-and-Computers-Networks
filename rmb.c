#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char * argv[]){

	
	int fdi, fdm, i, port_id, siipi, ret, ret_identity, nread, pub, port, n_show_messages_int, get_m, n_read_m;
	unsigned int addrlen;
	struct sockaddr_in addr1, addr2;
	char buffer1[2048],buffer2[2048], *b,*m, *e;
	char ms_ip[128],ms_uport[128];
	char message[140];
	char instruction[2048];
	struct hostent *h;
	struct in_addr *a;
	char* uselesschar;
	




	fdi=socket(AF_INET,SOCK_DGRAM,0);
	fdm=socket(AF_INET,SOCK_DGRAM,0);
	if(fdi==-1||fdm==-1){
		exit(1);
	};


/*If input checks out, start to attribute default input arguments*/
		
	h=gethostbyname("ubuntu");		
	if(h==NULL){
		printf("Error getting siip\n");
		exit(1);
	
	}
	a=(struct in_addr*)h->h_addr_list[0];
	if(a==NULL){
		printf("Error in_addr");
		exit(1);
	}
	
	memset((void*)&addr1,(int)'\0',sizeof(addr1));
	addr1.sin_family=AF_INET;
	addr1.sin_addr=*(struct in_addr*) h->h_addr_list[0];
	addr1.sin_port=htons(59000);
	
 /*If optional input arguments are given by the user, utilise them*/

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
	


/*Get message servers names to connect*/
	addrlen=sizeof(addr1);
	ret=sendto(fdi,"GET_SERVERS",256,0,(struct sockaddr*)&addr1,addrlen);
	if(ret==-1){
		printf("Error ret 2\n");
		exit(1);  
	}
	nread=recvfrom(fdi,buffer2,256,0,(struct sockaddr*)&addr1,&addrlen);
	if(nread==-1){
		printf("Error nread 2\n");
		exit(1);  
	}
	
/*Message server attribution*/
	b=strchr(buffer2,';');
	if(b==NULL){
		printf("No message servers available at this time\n");
		exit(1);
	}
	m=strchr(b+1,';');
	e=strchr(m+1,';');

	strncpy(ms_ip,b+1, m-b-1);
	strncpy(ms_uport, m+1, e-m-1);

	
	port=atoi(ms_uport);


	memset((void*)&addr2,(int)'\0',sizeof(addr2));
	addr2.sin_family=AF_INET;
	inet_pton(AF_INET,ms_ip,&(addr2.sin_addr));
	addr2.sin_port=htons(port);

	inet_ntop(AF_INET,&(addr2.sin_addr),ms_ip,INET_ADDRSTRLEN);
	

/*User interface*/

	while(1){
		memset((void*)&buffer1,(int)'\0',sizeof(buffer1));
		uselesschar=fgets(buffer1,2048,stdin);
		if(uselesschar==NULL){
			printf("Error fgets");
			exit(1);
		}
		sscanf(buffer1,"%s", instruction);
/*Implementing the user's commands*/

		if(strcmp(instruction, "show_servers")==0){
			addrlen=sizeof(addr1);
			ret_identity=sendto(fdi,"GET_SERVERS",256,0,(struct sockaddr*)&addr1,addrlen);
			if(ret_identity==-1){
				printf("Error ret\n");
				exit(1);  
			}
			nread=recvfrom(fdi,buffer2,256,0,(struct sockaddr*)&addr1,&addrlen);
			if(nread==-1){
				printf("Error nread\n");
				exit(1);  
			}
			printf("%s\n",buffer2);
		}else if(strcmp(instruction, "publish")==0){

			for(i=0;i!=7;i++){
				buffer1[i]=toupper(buffer1[i]);
			}

			strcpy(message,buffer1);
			pub=sendto(fdm,message,140,0,(struct sockaddr*)&addr2,addrlen);
			if(pub==-1){
				printf("Error pub\n");
				exit(1);
			}
		}else if(strcmp(instruction, "exit")==0){
			close(fdi);
			close(fdm);
			printf("Application exited successfully\n");
			exit(0);
		}else if(strcmp(instruction, "show_latest_messages")==0){
			sscanf(buffer1+strlen(instruction), " %d", &n_show_messages_int);
			
			addrlen=sizeof(addr2);
			memset((void*)&buffer2,(char)'\0',sizeof(buffer2));
			sprintf(buffer2, "GET_MESSAGES %d\n", n_show_messages_int);
			get_m=sendto(fdm,buffer2, 251,0,(struct sockaddr*)&addr2,addrlen);
			if(get_m==-1){
				printf("Error get_m\n");
				exit(1);  
			}
			memset((void*)&buffer2,(char)'\0',sizeof(buffer2));
			n_read_m=recvfrom(fdm,buffer2,251,0,(struct sockaddr*)&addr2,&addrlen);
			if(n_read_m==-1){
				printf("Error nread_m\n");
				exit(1);  
			}
			printf("%s\n",buffer2);
		}else{
			printf("Command not recognized\n");
		}
	}

close(fdi);
close(fdm);
exit(0);

}

