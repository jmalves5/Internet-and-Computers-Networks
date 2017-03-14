#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>


typedef struct svmessage{
	char string_message[140];
	int tmessage;
}svmessage;

void insertmessage(struct svmessage* m_vector, char* message, int time, int n_messages)
{   
    strcpy(m_vector[n_messages].string_message, message);
    m_vector[n_messages].tmessage=time;
    n_messages++;
}


int main(int argc, char * argv[])
{
	struct svmessage m_vector[2048];
	char name[100], ip[20], siip[20], sipt[20], instruction[20], protocol_message[1024], message[140];
	uint upt, tpt;
	time_t time1=0, time2=0;
	int fd, fd2, addrlen, addrlen2, ret_identity, ret_terminal, nread=0, m ,r, port_id, siipi, i, maxfd, nread1=0, logic_time=0, n_messages=0;
	struct sockaddr_in addr, addr2;
	char buffer1[2048],buffer2[2048],buffer3[2048], buffer4[2048];
	struct hostent *h;
	struct in_addr *a;
	fd_set readfds;

//Open UDP socket
	fd=socket(AF_INET,SOCK_DGRAM,0);
	if(fd==-1){
		printf("Error fd=socket\n");
		exit(1);//error
	}
	fd2=socket(AF_INET,SOCK_DGRAM,0);
	if(fd==-1){
		printf("Error fd2=socket\n");
		exit(1);//error
	}

//Clear addr for use
	memset((void*)&addr,(int)'\0',sizeof(addr));
	memset((void*)&addr2,(int)'\0',sizeof(addr2));

//Specify family
	addr.sin_family=AF_INET;
	addr2.sin_family=AF_INET;
//Prepare UDP with terminals
	addr2.sin_addr.s_addr=htonl(INADDR_ANY);

	
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
			
			h=gethostbyname("ubuntu");
			
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
	
	for(i=0;i<=argc-1;i++){
		
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
	//Bind the UDP server port to the terminals
	addr2.sin_port=htons(upt);
	ret_terminal=bind(fd2,(struct sockaddr*)&addr2,sizeof(addr2));
	if(ret_terminal==-1){
		printf("Error ret_terminal=bind\n");
		exit(1);//error
	}


//Start user interface
	while(1){
		if(time1>0){
			time2=time(NULL);
			if(time2-time1>r){
				addrlen=sizeof(addr);
					sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);
			
					ret_identity=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
					if(ret_identity==-1){
						printf("Error ret_identity join\n");
						exit(1);//error
					}

			}
		}


		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(fd, &readfds);
		FD_SET(fd2, &readfds);
		
		maxfd=fd;
		if(fd2>fd){
			maxfd=fd2;
		}
			
		select(maxfd+1, &readfds, NULL, NULL, NULL);
		
		if(FD_ISSET(0, &readfds)!=0){
			fgets(buffer1,2048,stdin);
			sscanf(buffer1,"%s", instruction);
			if(strcmp(instruction, "show_messages")==0){
    			for(i=0;i=n_messages;i++){
    				printf("%s\n", m_vector[i].string_message);
    			}
			}else if(strcmp(instruction, "show_servers")==0){
				addrlen=sizeof(addr);
				ret_identity=sendto(fd,"GET_SERVERS",2048,0,(struct sockaddr*)&addr,addrlen);
				if(ret_identity==-1){
					printf("Error ret_identity get servers\n");
					exit(1);//error
				}
				nread=recvfrom(fd,buffer2,2048,0,(struct sockaddr*)&addr,&addrlen);
				if(nread==-1){
					printf("Error rcvfrom show_servers\n");
				exit(1);//error
				}
				sscanf(buffer2,"%s", protocol_message);
				printf("%s\n",protocol_message);
			}else if(strcmp(instruction, "exit")==0){
					printf("Program exited successfully\n");
					close(fd);
					close(fd2);
					exit(0);
			}else if(strcmp(instruction, "join")==0){
				addrlen=sizeof(addr);
				sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);
			
				ret_identity=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
				if(ret_identity==-1){
					printf("Error ret_identity join\n");
					exit(1);//error
				}
				time1=time(NULL);
			}else{
				printf("Command not reconized\n");
			}
		}else if(FD_ISSET(fd2, &readfds)!=0){
			addrlen2=sizeof(addr2);
			nread1=recvfrom(fd2,buffer4,2048,0,(struct sockaddr*)&addr2,&addrlen2);
			if(nread1==-1){
					printf("Error rcvfrom message\n");
				exit(1);//error
				}
			logic_time++;
			sscanf(buffer4,"%s", protocol_message);
			insertmessage(m_vector, protocol_message, logic_time, n_messages);
    		printf("%s\n", m_vector[n_messages].string_message );
		}
	

	}
	
free(m_vector);
exit(0);
}