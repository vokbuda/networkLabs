#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFF_SIZE 256
#define PORT_NUM 5000
#define HOST_NAME "localhost"
#define ID_CLIENT '1'
#define ID_PREVIOUS '4'
int write_to_socket(int n,int sockfd,char *text_line,int startindex, int index){
					char client[2];
					client[1]='\0';
					client[0]=ID_CLIENT;
					
					if(startindex<index){ 
						
						char message_to_send[50];
						strcpy(message_to_send,text_line);
						strcat(message_to_send,"-");
						strcat(message_to_send,client);
						n = write(sockfd, message_to_send,strlen(message_to_send));
						if (n < 0)
							printf("ERROR writing to socket");
						startindex++;
					}else{
							char message_to_send[50];
							strcpy(message_to_send,"OUTOFLINES-");
							strcat(message_to_send,client);
							n = write(sockfd, message_to_send,strlen(message_to_send));
							if (n < 0)
								printf("ERROR writing to socket");	
					}
					return startindex;	
}



int main()
{
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t reader;

    fp = fopen("in/client1_in.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
	char **txt_messages=malloc(400*sizeof(char));
	
	int index=0;
	
    while ((reader = getline(&line, &len, fp)) != -1) {
		txt_messages[index]=malloc((strlen(line)+1)*sizeof(char));
		strcpy(txt_messages[index],line);
		if(index>0){
			txt_messages[index-1][strlen(txt_messages[index-1])-1]='\0';
		}
        
        index++;
       
    }
    fclose(fp);   
    if (line)
        free(line);   
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer [BUFF_SIZE];
	portno = PORT_NUM;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket");
	server = gethostbyname(HOST_NAME) ;
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
		printf("ERROR connecting");		
	char initial[50];
	int startindex=0;
	FILE *fptr;	
	fptr = fopen("out/client1_out.txt","w");
	while(1){	
		n = read(sockfd, buffer, BUFF_SIZE-1);
		strcpy(initial,buffer);
		if(strncmp(initial,"EXIT",4)==0){
					break;
				}
		if(strlen(initial)==1){
			char last_char=initial[strlen(initial)-1];
			if(last_char==ID_CLIENT){
				startindex=write_to_socket(n,sockfd,txt_messages[startindex],startindex,index);
			}	
		}else{				
				initial[strlen(initial)-2]='\0';
				
				char last_char=initial[strlen(initial)-1];
				if(last_char!=ID_CLIENT){
					fprintf(fptr,"%c %s",initial[strlen(initial)-1],initial);
					fprintf(fptr,"\n");					
				}				
				if(last_char==ID_PREVIOUS){
					startindex=write_to_socket(n,sockfd,txt_messages[startindex],startindex,index);		
				}	
			}	
	}
	return 0;
}
