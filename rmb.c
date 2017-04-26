/**************************************************************************************************************/
/*************************************************RCI Project**************************************************/
/*RCI-Reliable Message Board												  Redes de Computadores e Internet*/
/*João Alves 			Nº78181										  		          2nd Semester - 2016/2017*/
/*Filipa Fernandes		Nº78383					         	  	   			        Instituto Superior Técnico*/ 
/*                                                       											          */
/**************************************************************************************************************/
/* 						                             rmb.c 													  */
/**************************************************************************************************************/

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

	
	int fdi=0, fdm=0, i=0, port_id=0, siipi=0, ret=0, ret_identity=0, nread=0, pub=0, port=0, n_show_messages_int=0, get_m, n_read_m=0;
	unsigned int addrlen;
	struct sockaddr_in addr1, addr2;
	char buffer1[2048],buffer2[2048];
	char ms_ip[128],ms_uport[128];
	char message[140];
	char instruction[2048];
	struct hostent *h;
	struct in_addr *a;
	char* uselesschar, *token=NULL;
	


/*Open UDP sockets for identity servers and msgservers*/

	fdi=socket(AF_INET,SOCK_DGRAM,0);
	fdm=socket(AF_INET,SOCK_DGRAM,0);
	if(fdi==-1||fdm==-1){
		exit(1);
	};


/*If input checks out, start to attribute default input arguments*/
		
	h=gethostbyname("tejo");		
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
	
/*Message server infromation attribution*/
	memset((void*)&ms_ip,(char)'\0',sizeof(ms_ip));
	memset((void*)&ms_uport,(char)'\0',sizeof(ms_uport));

	token=strtok(buffer2+8, ";");
	if(token==NULL){
		printf("No message servers available at the moment1\n");
		exit(0);
	}
	token=strtok(NULL, ";");
	if(token==NULL){
		printf("No message servers available at the moment2\n");
		exit(0);
	}
	strcpy(ms_ip, token);
	token=strtok(NULL, ";");
	if(token==NULL){
		printf("No message servers available at the moment3\n");
		exit(0);
	}
	strcpy(ms_uport, token);
	token=strtok(NULL, "\n");

	
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
/*Case show_servers*/
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
/*Case publish*/
		}else if(strcmp(instruction, "publish")==0){

			for(i=0;i!=7;i++){
				buffer1[i]=toupper(buffer1[i]);
			}
			addrlen=sizeof(addr2);
			memset((void*)&message,(char)'\0',sizeof(message));
			strcpy(message,buffer1);
			pub=sendto(fdm,message,140,0,(struct sockaddr*)&addr2,addrlen);
			if(pub==-1){
				printf("Error pub\n");
				exit(1);
			}
/*Case exit*/
		}else if(strcmp(instruction, "exit")==0){
			close(fdi);
			close(fdm);
			printf("Application exited successfully\n");
			exit(0);
/*Case show_latest_messages_n*/
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
/*Close fd for UDP connections*/
close(fdi);
close(fdm);
exit(0);

}

