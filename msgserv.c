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
#include <stdbool.h>



typedef struct svmessage{
	char string_message[140];
	int tmessage;
}svmessage;

typedef struct node {
   char name[40];
   char ip[40];
   int upt;
   int fd;
   int tpt;
   struct node *next;
}node;

void* insertmessage(struct svmessage* m_vector, char* message, int time, int n_messages);

void* insertmessage(struct svmessage* m_vector, char* message, int time, int n_messages){   
    strcpy(m_vector[n_messages].string_message, message);
    m_vector[n_messages].tmessage=time;
    n_messages++;
}

void* insertList(node* current, char *name, char *ip, int upt, int tpt, int fd);
void* insertList(node* current, char *name, char *ip, int upt, int tpt, int fd){
   	node *link;
   	link=(struct node*) malloc(sizeof(struct node));
	link->next=(struct node*) malloc(sizeof(struct node));

   	strcpy(link->name, name);
   	strcpy(link->ip,ip);
   	
   	link->upt=upt;
   	
   	link->tpt=tpt;
   	
   	link->next = NULL;
   	link->fd=fd;
   	
   	current->next=link;
   	
   	current=link;
}

bool ListisEmpty(node* head) {
   return head == NULL;
}


void* printList(node* head){
	node * aux=head;
	while(aux!=NULL){
		printf("%s\n", aux->name);
		printf("%s\n", aux->ip);
		printf("%d\n", aux->upt);
		printf("%d\n", aux->tpt);
		aux=aux->next; 
	}
}

int main(int argc, char * argv[])
{
	struct svmessage m_vector[2048];
	char name[100], ip[20], siip[20], sipt[20], instruction[20], protocol_message[1024], aux_pm[40], message[140],name_tcp[40], ip_tcp[40], name_new_tcp[40], ip_new_tcp[40];
	uint upt, tpt, upt_tcp, tpt_tcp, upt_new_tcp, tpt_new_tcp;
	time_t time1=0, time2=0;
	int fd, fd2, fdlisten, addrlen, addrlen2, ret_identity1, ret_identity2, ret_terminal, nread=0, m ,r, port_id, siipi, i, maxfd, nread1=0, logic_time=0, n_messages=0, len_token, offset, n_servers, clientlen, newfd;
	struct sockaddr_in addr, addr2, addr3, addrtcpc, addrtcps, clientaddr;
	char buffer1[2048],buffer2[2048],buffer3[2048], buffer4[2048], buffer5[2048];
	struct hostent *h;
	struct in_addr *a;
	fd_set readfds;
	char* token, *token2;
	struct node *head, *aux;
	head=NULL;
	struct node* current=(struct node*) malloc(sizeof(struct node));
	struct in_addr *atcp=(struct in_addr*)malloc(sizeof(struct in_addr));


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
	addr3.sin_family=AF_INET;
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

	//Get message servers to form the list for the TCP connections 
	
	addrlen=sizeof(addr);
	ret_identity2=sendto(fd,"GET_SERVERS",11,0,(struct sockaddr*)&addr,addrlen);
	if(ret_identity2==-1){
		printf("Error ret_identity2 get servers\n");
		exit(1);//error
	}
	memset((void*)&buffer2,(char)'\0',sizeof(buffer2));
	nread=recvfrom(fd,buffer2,2048,0,(struct sockaddr*)&addr,&addrlen);
	if(nread==-1){
		printf("Error rcvfrom show_servers\n");
		exit(1);//error
	}
	
//Get all the servers	
	if(buffer2!=NULL){
		token=strtok(buffer2+8, ";");
	}
	while(token!=NULL){
	
		strcpy(name_tcp, token);
		
		token=strtok(NULL, ";");
		if(token==NULL){
			break;
		}
		
		inet_aton(token, &(addr3.sin_addr));
		strcpy(ip_tcp, inet_ntoa(addr3.sin_addr));

		token=strtok(NULL, ";");
		if(token==NULL){
			break;
		}
		upt_tcp=atoi(token);

		token=strtok(NULL, ";");
		if(token==NULL){
			break;
		}
		tpt_tcp=atoi(token);


//Fill the List	
		if(head==NULL){
			n_servers++;
			head=(struct node*) malloc(sizeof(struct node));
			head->next=(struct node*) malloc(sizeof(struct node));

   			strcpy(head->name, name_tcp);
   			strcpy(head->ip,ip_tcp);
   			head->upt=upt_tcp;
   			head->tpt=tpt_tcp;
   			head->next = NULL;

  		 	current=head;
		}else{
			n_servers++;
			insertList(current, name_tcp, ip_tcp, upt_tcp, tpt_tcp, 1);
		}		
	}	

	printList(head);

//OPen TCP listen socket

	fdlisten=socket(AF_INET, SOCK_STREAM,0);
	memset((void*)&addrtcps,(int)'\0',sizeof(addrtcps));
	addrtcps.sin_family=AF_INET;
	addrtcps.sin_addr.s_addr=htonl(INADDR_ANY);
	addrtcps.sin_port=htons(tpt);

//Bind TCP port for other message servers

	bind(fdlisten,(struct sockaddr*)&addrtcps,sizeof(addrtcps));
	

//CONNECT TO ALL THE SERVERS
	aux=head;
	int ntcp;
	while(aux!=NULL){
		//Open socket
		aux->fd=socket(AF_INET, SOCK_STREAM, 0);
		if(aux->fd==-1){
			printf("Error socket TCP\n");
		}
		//Fill sockaddr_in
		memset((void*)&addrtcpc,(int)'\0',sizeof(addrtcpc));
		addrtcpc.sin_family=AF_INET;
		inet_aton(aux->ip,&addrtcpc.sin_addr);
		addrtcpc.sin_port=htons(aux->tpt);
		
		if(strcmp(aux->ip,ip)!=0){
			ntcp=connect(aux->fd, (struct sockaddr*)&addrtcpc,sizeof(addrtcpc));
			if(ntcp==-1){
				printf("Error connecting to TCP server. No accept. Skipping.\n");
			}	
			sprintf(buffer5, "%s;%s;%d;%d;\n", name, ip, upt, tpt);
			write(aux->fd,buffer5, 200);
		}
		aux=aux->next;
	}

	listen(fdlisten, 5);
//Get time at beggining of the application
	time1=time(NULL);
//Join
	sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);					
	ret_identity1=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
	if(ret_identity1==-1){
		printf("Error ret_identity1 join\n");
		exit(1);//error	
	}
	
//Start user interface
	while(1){
		time2=time(NULL);
		if(time2-time1>r){
			time1=time(NULL);
			addrlen=sizeof(addr);
			sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);					
			ret_identity1=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
			if(ret_identity1==-1){
				printf("Error ret_identity1 join\n");
				exit(1);//error	
			}
		}
		
//Set the file descriptors

		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(fd2, &readfds);
		FD_SET(fdlisten, &readfds);
		aux=head;
		while(aux!=NULL){
			FD_SET(aux->fd, &readfds);
			aux=aux->next;
		}

//Retrieve maximum fd
		maxfd=fd2;
		if(fdlisten>maxfd){
			maxfd=fdlisten;
		}
		aux=head;
		while(aux!=NULL){
			if(aux->fd>maxfd){
				maxfd=aux->fd;
			}
			aux=aux->next;
		}

		
//Select function. Checks which file descriptor is active			
		select(maxfd+1, &readfds, NULL, NULL, NULL);

//If msgserv is active read
		/*aux=head;
		while(aux!=NULL){
			if (FD_ISSET(aux->fd, &readfds)!=0)
			{
				read(newfd, buffer5, 2048);
				printf("%s\n",buffer5);
				printf("ola\n");
			}
			
			aux=aux->next;
		}*/
//If listenfd is active connect to the new server
		if(FD_ISSET(fdlisten, &readfds)){
			
			clientlen=sizeof(addrtcps);
			newfd=accept(fdlisten,(struct sockaddr*)&addrtcps,&clientlen);
			read(newfd, buffer5, 2048);
			printf("mensagem recebida: ");
			printf("%s\n",buffer5);
			
			token=strtok(buffer5, ";");
			strcpy(name_new_tcp, token);

			token=strtok(NULL, ";");
			strcpy(ip_new_tcp, token);

			token=strtok(NULL, ";");
			upt_new_tcp=atoi(token);
			
			token=strtok(NULL, ";");
			tpt_new_tcp=atoi(token);
//Insere novo server na lista
			insertList(current, name_new_tcp, ip_new_tcp,  upt_new_tcp, tpt_new_tcp, newfd);
//Imprimir a lista
			printList(head);
		}

//If stdin (0) is active read and choose what to do		
		if(FD_ISSET(0, &readfds)!=0){
			fgets(buffer1,2048,stdin);
			sscanf(buffer1,"%s", instruction);
			if(strcmp(instruction, "show_messages")==0){							
    			for(i=0;i!=n_messages;i++){
    				printf("%s\n", m_vector[i].string_message);
    			}
			}else if(strcmp(instruction, "show_servers")==0){
				addrlen=sizeof(addr);
				ret_identity2=sendto(fd,"GET_SERVERS",11,0,(struct sockaddr*)&addr,addrlen);
				if(ret_identity2==-1){
					printf("Error ret_identity2 get servers\n");
					exit(1);//error
				}
				memset((void*)&buffer2,(char)'\0',sizeof(buffer2));
				nread=recvfrom(fd,buffer2,2048,0,(struct sockaddr*)&addr,&addrlen);
				if(nread==-1){
					printf("Error rcvfrom show_servers\n");
				exit(1);//error
				}
				printf("%s\n",buffer2);

			}else if(strcmp(instruction, "exit")==0){
					printf("Program exited successfully\n");
					close(fd);
					close(fd2);
					aux=head;
					while(aux!=NULL){
						free(aux);
						aux=aux->next;
					}
					exit(0);
			}else if(strcmp(instruction, "join")==0){
				addrlen=sizeof(addr);
				sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);
			
				ret_identity1=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
				if(ret_identity1==-1){
					printf("Error ret_identity join\n");
					exit(1);//error
				}
				time1=time(NULL);
			}else{
				printf("Command not recognized\n");
			}
//If terminal file descriptor is active, read and choose what to do
		}else if(FD_ISSET(fd2, &readfds)!=0){
			addrlen2=sizeof(addr2);
			nread1=recvfrom(fd2,buffer4,2048,0,(struct sockaddr*)&addr2,&addrlen2);
			if(nread1==-1){
					printf("Error rcvfrom message\n");
				exit(1);//error
				}
			token=strtok(buffer4, " ");
			if(strcmp(token, "PUBLISH")==0){
				offset=0;
				token=strtok(NULL, " ");
				while(token!=NULL){
					len_token=strlen(token);
					strcpy(protocol_message+offset, token);
					protocol_message[offset+len_token]=' ';
					offset+=len_token+1;
					token=strtok(NULL, " ");
				}
				printf("%s", protocol_message);
//Insert message read into the message vector m_vector
				insertmessage(m_vector, protocol_message, logic_time, n_messages);
    			n_messages++;
    		}
		}
	

	}
	

exit(0);
}