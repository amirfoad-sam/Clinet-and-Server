#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define IP_PROTOCOL 0 
#define PORT_NO 15050 
#define NET_BUF_SIZE 32 
#define cipherKey 'S' 
#define sendrecvflag 0 
#define nofile "File Not Found!"

void clearBuf(char* b); 
int sendFile();

int sockfd, nBytes; 
struct sockaddr_in addr_con; 
int addrlen = sizeof(addr_con); 
	 
char net_buf[NET_BUF_SIZE]; 
FILE* fp;



// function to encrypt 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// function sending file 
int sendFile(FILE* fp, char* buf, int s) 
{ 
	int i, len; 
	if (fp == NULL) { 
		strcpy(buf, nofile); 
		len = strlen(nofile); 
		buf[len] = EOF; 
		for (i = 0; i <= len; i++) 
			buf[i] = Cipher(buf[i]); 
		return 1; 
	} 
	char ch, ch2; 
	for (i = 0; i < s; i++) { 
		ch = fgetc(fp); 
		ch2 = Cipher(ch); 
		buf[i] = ch2; 
		if (ch == EOF) 
			return 1; 
	} 
	return 0; 
} 

// My code to receive file
int recvFile(char* buf, int s) 
{ 
	int i; 
	char ch; 
	for (i = 0; i < s; i++) { 
		ch = buf[i]; 
		ch = Cipher(ch); 
		if (ch == EOF) 
			return 1; 
		else
			printf("%c", ch); 
	} 
	return 0; 
} 

// function to service the client
void serviceClient( )
{	
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(PORT_NO); 
	addr_con.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0) 
		printf("\nSocket is OK! Successfully binded!\n"); 
	else
		printf("\nBinding Failed!\n"); 
	
	// Send
	while (1) { 
		printf("\n%d child\n", getpid());
		printf("\n**********Waiting for client**********\n"); 

		// receive file name 
		for(int i =0 ; i< NET_BUF_SIZE ; i++) 
				net_buf[i] = '\0';

		nBytes = recvfrom(sockfd, net_buf, 
						NET_BUF_SIZE, sendrecvflag, 
						(struct sockaddr*)&addr_con, &addrlen); 

		fp = fopen(net_buf, "r"); //open file for reading 
		printf("\nFile Name Received: %s\n", net_buf); 
		if (strcmp(net_buf, "quit") == 0 ){
			printf("\nSocket is closing!\n");
			break;
		}
		else if (fp == NULL) 
			printf("\nFile open failed!\n"); 
		else{
			printf("\nFile Successfully opened!\n");
			for(int i=0; i<50; i++)
				printf("_");
		}

		while (1) { 
			
			// process	 
			if (sendFile(fp, net_buf, NET_BUF_SIZE)) { 
				sendto(sockfd, net_buf, NET_BUF_SIZE, 
					sendrecvflag, 
					(struct sockaddr*)&addr_con, addrlen); 
				break; 
			} 

			// send 
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, 
				(struct sockaddr*)&addr_con, addrlen); 
			for(int i =0 ; i< NET_BUF_SIZE ; i++) 
				net_buf[i] = '\0';
		} 
		if (fp != NULL) 
			fclose(fp); 
	}
	/**
	// Receive
	while (1)
	{
		
		//printf("file name: %s\n", net_buf);
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, (struct sockaddr*)&addr_con, 
				addrlen); 
				
			//printf("\n---------File is Downloading---------\n"); 

			while (1) { 
				// receive 
				//clearBuf(net_buf); 
				for(int i =0 ; i< NET_BUF_SIZE ; i++) 
					net_buf[i] = '\0';
				nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
								sendrecvflag, (struct sockaddr*)&addr_con, 
								&addrlen); 

				// process 
				if (recvFile(net_buf, NET_BUF_SIZE)) { 
					break; 
				} 
			} 
			printf("\n---------File Downloaded---------\n"); 
	}**/
	
}

// main function
int main() 
{ 
	pid_t child[20];

	// socket() 
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL); 
	
	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	//else
	//	printf("\nfile descriptor #%d received\n", sockfd); 
	

	while (1)
	{			
			if(fork() == 0) 
        	{ 
            //printf("child pid %d from parent pid %d\n",getpid(),getppid()); 
			serviceClient();
            exit(0); 
        	} 
    	
    	 
    		wait(NULL); 
			printf("\nparent is here!\n");
		
		
		/*
		//printf("\nWaiting for client\n");
		//for( int n = 0; n < 4; ++n ) {
    		switch( fork()) { 
			case 0:// child 
				serviceClient();
				printf("\nchild #%d\n", getpid());
				exit( 0 );
			case -1: // Error!
				perror( "\nfork error!\n" );
				exit( 1 );
			default:  //parent
				printf("\nWaiting for client\n");
    		} 
		//}*/

		//int child[100], i=0;
		
		//if((child[i]= fork()) == 0)
			//serviceClient();	
	}
	
	return 0; 
} 
