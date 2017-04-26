/**************************************************************************************************************/
/*************************************************RCI Project**************************************************/
/*RCI-Reliable Message Board												  Redes de Computadores e Internet*/
/*João Alves 			Nº78181										  		          2nd Semester - 2016/2017*/
/*Filipa Fernandes		Nº78383					         	  	   			        Instituto Superior Técnico*/ 
/*                                                       											          */
/**************************************************************************************************************/
/* 						                           msgserv.c 													  */
/**************************************************************************************************************/

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


/*Saved message structure. Contains message and it's logic clock value*/
typedef struct svmessage{
  char string_message[140];
  int tmessage;
}svmessage;


/*Connected TCP server structure*/
typedef struct node {
   char name[40];
   char ip[40];
   int upt;
   int fd;
   int tpt;
   struct node *next;
}node;

/*Function that creates new link and inserts it on list*/
node* CreateInsertNode(char name[40], char ip[40], int upt, int fd, int tpt, node* head){
  if(head==NULL){
 
    head = (node *) malloc(sizeof(node));
    strcpy(head->name, name);
    strcpy(head->ip, ip);
    head->upt=upt;
    head->tpt=tpt;
    head->next=NULL;
    head->fd=fd;
   
  }else{
/*Create node*/
    node* link=(node *) malloc(sizeof(node));
    strcpy(link->name, name);
    strcpy(link->ip, ip);
    link->upt=upt;
    link->tpt=tpt;
    link->next=NULL;
    link->fd=fd;
/*Insert in list*/
    node*aux=head;
    while(aux->next!=NULL){
      aux=aux->next;
    }
    aux->next=link;
  }
  return head;
}

/*Function that prints server list*/
void printList(node* head){
  node * aux=head;
  while(aux!=NULL){
    printf("%s\n", aux->name);
    printf("%s\n", aux->ip);
    printf("%d\n", aux->upt);
    printf("%d\n", aux->tpt);
    aux=aux->next; 
  }
}

/*Function that inserts message on message vector*/
void insertmessage(struct svmessage* m_vector, char* message, int time, int n_messages){  
	memset((void*)&m_vector[n_messages].string_message,(char)'\0',sizeof(m_vector[n_messages].string_message));
    strcpy(m_vector[n_messages].string_message, message);
    m_vector[n_messages].tmessage=time;
   
}

/*Function that initializes list*/
node * initLinkedList(void){
  	return NULL;
}

/*Function that removes link from list*/
node * removefromList(node* head, node* toremove){
	node *aux=head;

	if(head==toremove){
		head=head->next;
		aux->next=NULL;
		free(aux);
		return head;
	}else{

		while(aux->next!=toremove){
			aux=aux->next;
		}
		aux->next=toremove->next;

		toremove->next=NULL;
		free(toremove);
 		return head;
 	}
}

/*Function that frees all links in list*/
void freeList(node*head){
	node*aux=head;
	node*aux2=NULL;
	while(aux!=NULL){
		close(aux->fd);
		aux2=aux->next;
		free(aux);
		aux=aux2;
		if(aux==NULL){
			break;
		}
	}
	
	return;
}


int main(int argc, char * argv[])
{
	
	char name[100], ip[20], instruction[20], protocol_message[140], name_tcp[40], ip_tcp[40]; 
	char name_new_tcp[40], ip_new_tcp[40];
	unsigned int upt, tpt, upt_tcp, tpt_tcp, upt_new_tcp, tpt_new_tcp;
	time_t time1=0, time2=0;
	int useless, fd, fd2,fdlisten, ret_identity1, ret_identity2, ret_terminal, nread=0, m=0, r;
	int port_id, siipi, i, maxfd, nread1=0, logic_time=0, n_messages=0, len_token, offset,newfd;
	int n_messages_to_send, send_m;
	unsigned int addrlen, addrlen2,clientlen;
	struct sockaddr_in addr, addr2, addr3, addrtcpc, addrtcps;
	char buffer1[2048],buffer2[2048],buffer3[2048], buffer4[2048], buffer5[2048],buffer6[2048];
	struct hostent *h;
	struct in_addr *a;
	fd_set readfds;
	char* token, *uselesschar;
	struct node* head=initLinkedList();
	struct node* aux;


/*Open UDP socket*/
	fd=socket(AF_INET,SOCK_DGRAM,0);
	if(fd==-1){
		printf("Error fd=socket\n");
		exit(1); 
	}
	fd2=socket(AF_INET,SOCK_DGRAM,0);
	if(fd==-1){
		printf("Error fd2=socket\n");
		exit(1); 
	}
	
	
/*Clear addr for use*/
	memset((void*)&addr,(int)'\0',sizeof(addr));
	memset((void*)&addr2,(int)'\0',sizeof(addr2));

/*Specify family*/
	addr.sin_family=AF_INET;
	addr2.sin_family=AF_INET;
	addr3.sin_family=AF_INET;
/*Prepare UDP with terminals*/
	addr2.sin_addr.s_addr=htonl(INADDR_ANY);

	
/*Begin to separate input arguments. With or whitout optional input arguments.*/
	if((argc < 9)){
		printf("Invalid input: Not enough arguments\n");
		exit(1);
	}else{ 
		if((strcmp(argv[1],"-n")!=0)||(strcmp(argv[3],"-j")!=0)||(strcmp(argv[5],"-u")!=0)||(strcmp(argv[7],"-t")!=0)){
			printf("Invalid input: Wrong arguments\n");
			exit(1);
		}else{

/*If input checks out, start to attribute default input arguments*/
			
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
/*If optional input arguments are given by the user, utilise them*/
	
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
/*Declare message_vector of m size*/
	struct svmessage m_vector[m];


/*Bind the UDP server port to the terminals*/
	addr2.sin_port=htons(upt);
	ret_terminal=bind(fd2,(struct sockaddr*)&addr2,sizeof(addr2));
	if(ret_terminal==-1){
		printf("Error ret_terminal=bind\n");
		exit(1); 
	}
	
/*Join*/
	memset((void*)&buffer3,(int)'\0',sizeof(buffer3));
	sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);					
	ret_identity1=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,sizeof(addr));
	if(ret_identity1==-1){
		printf("Error ret_identity1 join\n");
		exit(1); 	
	}
	

/*Get message servers to form the list for the TCP connections*/

	ret_identity2=sendto(fd,"GET_SERVERS",11,0,(struct sockaddr*)&addr,sizeof(addr));
	if(ret_identity2==-1){
		printf("Error ret_identity2 get servers\n");
		exit(1); 
	}
	memset((void*)&buffer2,(char)'\0',sizeof(buffer2));
	addrlen=sizeof(addr);
	nread=recvfrom(fd,buffer2,2048,0,(struct sockaddr*)&addr,&addrlen);
	if(nread==-1){
		printf("Error rcvfrom show_servers\n");
		exit(1); 
	}	
/*Get all the servers*/	
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
		token=strtok(NULL, "\n");

/*Fill the List*/	
		head=CreateInsertNode(name_tcp, ip_tcp, upt_tcp, 0, tpt_tcp, head);

	}	
/*Open TCP listen socket*/

	fdlisten=socket(AF_INET, SOCK_STREAM,0);
	memset((void*)&addrtcps,(int)'\0',sizeof(addrtcps));
	addrtcps.sin_family=AF_INET;
	addrtcps.sin_addr.s_addr=htonl(INADDR_ANY);
	addrtcps.sin_port=htons(tpt);

/*Bind TCP port for other message servers*/

	bind(fdlisten,(struct sockaddr*)&addrtcps,sizeof(addrtcps));
	

/*TCP connection to all servers available*/
	aux=head;
	int ntcp;
	while(aux!=NULL){
/*Open socket*/
		aux->fd=socket(AF_INET, SOCK_STREAM, 0);
		if(aux->fd==-1){
			printf("Error socket TCP\n");
			exit(1);
		}
/*Fill sockaddr_in*/
		memset((void*)&addrtcpc,(int)'\0',sizeof(addrtcpc));
		addrtcpc.sin_family=AF_INET;
		inet_aton(aux->ip,&addrtcpc.sin_addr);
		addrtcpc.sin_port=htons(aux->tpt);
/*Connect to available servers*/		
		if(strcmp(aux->name,name)!=0){
			ntcp=connect(aux->fd, (struct sockaddr*)&addrtcpc,sizeof(addrtcpc));
			if(ntcp==-1){
/*Remove server from List if  not connected*/
				printf("Can't connect to %s Skipping it\n",aux->name);
				head=removefromList(head, aux);
/*Send server information to all servers*/
			}else{
				memset((void*)&buffer5,(char)'\0',sizeof(buffer5));	
				sprintf(buffer5, "%s;%s;%d;%d;", name, ip, upt, tpt);
				useless=write(aux->fd,buffer5, strlen(buffer5));
				if(useless==-1){
					printf("Error write1\n");
					exit(1);
				}
/*Get all messages from msg servers*/ 
				sprintf(buffer5, "SGET_MESSAGES\n");
				useless=write(head->fd,buffer5, strlen(buffer5));
				if(useless==-1){
					printf("Error write2\n");
					exit(1);
				}
			}
		}
		aux=aux->next;
	}
/*Listen for new TCP connection*/
	listen(fdlisten, 5);
/*Get time at beggining of the application*/
	time1=time(NULL);
	
/*Start user interface*/
	while(1){
/*Get time at each iteration of the while loop*/	
		time2=time(NULL);
/*If more than r seconds passed re-register to identity server and reset time1*/		
		if(time2-time1>r){
			time1=time(NULL);
/*Re-registering */
			addrlen=sizeof(addr);
			sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);					
			ret_identity1=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
			if(ret_identity1==-1){
				printf("Error ret_identity1 join\n");
				exit(1); 	
			}
		}
		
/*Set the file descriptors on readfds*/
		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(fd2, &readfds);
		FD_SET(fdlisten, &readfds);
		aux=head;
		while(aux!=NULL){
			FD_SET(aux->fd, &readfds);
			aux=aux->next;
		}
/*Retrieve maximum fd*/
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
/*Select function. Checks which file descriptor is active*/			
		select(maxfd+1, &readfds, NULL, NULL, NULL);

/*If any file descriptor of any msgserv connected to this server is active read it's message*/
		aux=head;
		while(aux!=NULL){
/*Make sure we don't check our own fd*/
			if(strcmp(aux->name,name)==0){
				head=removefromList(head, aux);
				break;
			}
			if (FD_ISSET(aux->fd, &readfds) && strcmp(aux->name,name)!=0){
/*Read message*/
				memset((void*)&buffer6,(char)'\0',sizeof(buffer6));
				useless=read(aux->fd, buffer6, 100);
				if(useless==-1){
					printf("Error read1\n");
					exit(1);
				}else if(useless==0){
					printf("End of TCP connection with %s\n", aux->name);
					head=removefromList(head, aux);
					break;
				}	

				token=strtok(buffer6, "\n");
				if(token!=NULL){
/*If SMESSAGES is received, receive the message table that follows*/
					if(strcmp(token, "SMESSAGES")==0){
						token=strtok(NULL, ";");
						while(token!=NULL){
							logic_time=atoi(token);
							token=strtok(NULL, "\n");
							if(token==NULL){
								printf("Wrong protocol message/format from another server!\n");
								break;
							}
							n_messages++;
							printf("Received from msgserv: %d;%s\n",logic_time, token);
/*Insert each message into the message vector*/
							insertmessage(m_vector, token, logic_time, n_messages-1);
							token=strtok(NULL, "\n");
							token=strtok(NULL, ";");
						}
					}
				}
				
			}
			aux=aux->next;

		}

/*If listenfd is active accept the connection to the new server*/
		if(FD_ISSET(fdlisten, &readfds)){
			
			memset((void*)&buffer5,(char)'\0',sizeof(buffer5));
			clientlen=sizeof(addrtcps);
			newfd=accept(fdlisten,(struct sockaddr*)&addrtcps,&clientlen);
			useless=read(newfd, buffer5, 2048);
			if(useless==-1){
				printf("Error read2\n");
				exit(1);
			}
			token=strtok(buffer5, ";");
			if(token==NULL){
				printf("Wrong protocol format from new server but TCP connection is still active\n");		
			}
			strcpy(name_new_tcp, token);


			token=strtok(NULL, ";");
			if(token==NULL){
				printf("Wrong protocol format from new server but TCP connection is still active\n");	
			}
			strcpy(ip_new_tcp, token);

			token=strtok(NULL, ";");
			if(token==NULL){
				printf("Wrong protocol format from new server but TCP connection is still active\n");	
			}
			upt_new_tcp=atoi(token);
			
			token=strtok(NULL, ";");
			if(token==NULL){
				printf("Wrong protocol format from new server but TCP connection is still active\n");	
			}
			tpt_new_tcp=atoi(token);
/*Insert new server on list*/
			head=CreateInsertNode(name_new_tcp, ip_new_tcp, upt_new_tcp, newfd, tpt_new_tcp, head);
/*If new servers also asks for n of this server messages, send the n messages to it*/
			token=strtok(NULL,"\n");
			if(token!=NULL){
				if(strcmp(token, "SGET_MESSAGES")==0){
					memset((void*)&buffer5,(char)'\0',sizeof(buffer5));
					sprintf(buffer5,"SMESSAGES\n");
					for(i=0;i<n_messages;i++){
						memset((void*)&buffer6,(char)'\0',sizeof(buffer6));
						sprintf(buffer6,"%d;%s\n",m_vector[i].tmessage, m_vector[i].string_message);
						printf("Sending to msgserv: %s\n", buffer6);
					}
					strcat(buffer5+10, buffer6);
/*Sending the messages*/
					useless=write(newfd, buffer5, 2048);
					if(useless==-1){
						printf("Error read3\n");
						exit(1);
					}
				}else{
					printf("Wrong protocol format from new server but TCP connection is still active\n");
				}
			}else{
				printf("Wrong protocol format from new server but TCP connection is still active\n");
			}
		}

/*If stdin (0) is active read and choose what to do*/	
		if(FD_ISSET(0, &readfds)!=0){
			uselesschar=fgets(buffer1,2048,stdin);
			if(uselesschar==NULL){
				printf("Error fgets\n");
				exit(1);
			}
			sscanf(buffer1,"%s", instruction);
/*Case show_messages*/
			if(strcmp(instruction, "show_messages")==0){					
    			for(i=0;i<n_messages;i++){
    				printf("%d;%s\n",m_vector[i].tmessage, m_vector[i].string_message);
    			}
/*Case show_servers*/
			}else if(strcmp(instruction, "show_servers")==0){
				printList(head);
/*Case exit*/
			}else if(strcmp(instruction, "exit")==0){
					printf("Program exited successfully\n");
					close(fd);
					close(fd2);
					freeList(head);
					exit(0);
/*Case join*/
			}else if(strcmp(instruction, "join")==0){
				addrlen=sizeof(addr);
/*Re-registers on the identity server*/
				sprintf(buffer3, "REG %s;%s;%d;%d", name, ip, upt, tpt);
				ret_identity1=sendto(fd,buffer3,2048,0,(struct sockaddr*)&addr,addrlen);
				if(ret_identity1==-1){
					printf("Error ret_identity join\n");
					exit(1); 
				}
				time1=time(NULL);
			}else{
				printf("Command not recognized\n");
			}
/*If rmb file descriptor is active, read and choose what to do*/
		}else if(FD_ISSET(fd2, &readfds)!=0){
			memset((void*)&buffer4,(int)'\0',sizeof(buffer4));
			memset((void*)&protocol_message,(int)'\0',sizeof(protocol_message));
			addrlen2=sizeof(addr2);
/*Receive message*/
			nread1=recvfrom(fd2,buffer4,2048,0,(struct sockaddr*)&addr2,&addrlen2);
			if(nread1==-1){
					printf("Error rcvfrom message\n");
				exit(1);
				}
			token=strtok(buffer4, " ");
/*Check type of message*/
			if(strcmp(token, "PUBLISH")==0){
				logic_time++;
				offset=0;
				token=strtok(NULL, " ");
				while(token!=NULL){
					len_token=strlen(token);
					strcpy(protocol_message+offset, token);
					protocol_message[offset+len_token]=' ';
					offset+=len_token+1;
					token=strtok(NULL, " ");
				}
				printf("Received from rmb: %d, %s\n",logic_time, protocol_message);
/*Insert message read into the message vector m_vector*/
				insertmessage(m_vector, protocol_message, logic_time, n_messages);
				n_messages++;
				aux=head;
/*Replicate new message throughout the msgservers*/
				while(aux!=NULL){
					if(strcmp(aux->name,name)!=0){
						memset((void*)&buffer5,(char)'\0',sizeof(buffer5));
						sprintf(buffer5,"SMESSAGES\n");
						memset((void*)&buffer6,(char)'\0',sizeof(buffer6));
						sprintf(buffer6,"%d;%s\n",m_vector[n_messages-1].tmessage, m_vector[n_messages-1].string_message);
			
						printf("Sending to msgserv: %s\n", buffer6);
						sprintf(buffer5+10, "%s", buffer6);	
						useless=write(aux->fd, buffer5, 2048);
						if(useless==-1){
							printf("Error write4\n");
							exit(1);
						}
					}
					aux=aux->next;
				}
/*If a rmb asks for n messages*/
    		}else if(strcmp(token, "GET_MESSAGES")==0){
    			token=strtok(NULL, "\n");
    			if (token!=NULL){   			
    				n_messages_to_send=atoi(token);
/*If there are n messages available, send them, if there aren't, send those that the server has in storeage*/
    				if(n_messages_to_send>n_messages){
    					n_messages_to_send=n_messages;
    				}

    				memset((void*)&buffer5,(char)'\0',sizeof(buffer5));
					sprintf(buffer5,"MESSAGES\n");
					for(i=n_messages-n_messages_to_send;i<n_messages;i++){
						memset((void*)&buffer6,(char)'\0',sizeof(buffer6));
						sprintf(buffer6,"%d;%s\n",m_vector[i].tmessage, m_vector[i].string_message);
						printf("Sending to rmb: %s\n", buffer6);
						strcat(buffer5+9, buffer6);
					}
/*Send them the messages*/
					send_m=sendto(fd2,buffer5, 251,0,(struct sockaddr*)&addr2,addrlen2);
					if(send_m==-1){
						printf("Error send_m\n");
						exit(1);  
					}
				}
    		}else{
				printf("Wrong protocol message/format from rmb!\n");
			}
		}
	}
	

exit(0);
}