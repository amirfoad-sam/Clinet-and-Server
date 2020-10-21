#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define IP_PROTOCOL 0 
#define IP_ADDRESS "137.207.82.53" //Server IP Address
#define PORT_NO 15050 
#define NET_BUF_SIZE 32 
#define cipherKey 'S' 
#define sendrecvflag 0 
#define nofile "File Not Found!"


// function to clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

// function for decryption 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// function to receive file 
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

// function to send file
int sendFile(FILE* fp, char* buf, int s) 
{ 
	int i, len; 
	if (fp == NULL) { 
		strcpy(buf, nofile); 
		len = strlen(nofile); 
		buf[len] = EOF; //End of File!
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

// main function
int main() 
{ 
	int sockfd, nBytes; 
	struct sockaddr_in addr_con; 
	int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(PORT_NO); 
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
	char net_buf[32]; 
	char buffer[32];
	char user_command[32];
	FILE* fp; 
	clearBuf(user_command);
	user_command[4] = '\0';

	// socket() 
	sockfd = socket(AF_INET, SOCK_DGRAM, 
					IP_PROTOCOL); 

	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nSocket is OK! file descriptor #%d received\n", sockfd); 

	while (1) {

		//gets(user_command);
		printf("\nPlease enter the command\n'get put quit'\n");
		scanf("%s", user_command);
		printf("user command is %s \n", user_command);

		// Quit
		if ( strcmp(user_command, "quit") == 0 ) {
			strcpy(net_buf, user_command);
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, (struct sockaddr*)&addr_con, 
				addrlen); 
			printf("Good Bye!\n");
			break;
		}

		// Get
		if ( strcmp(user_command, "get") == 0)
		{	//printf("GEEEEEET!\n");
			
			printf("\nPlease enter file name to download it from %s:\n", IP_ADDRESS); 
			scanf("%s", net_buf);
			
			//printf("file name: %s\n", net_buf);
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, (struct sockaddr*)&addr_con, 
				addrlen); 
				
			//printf("\n---------File is Downloading---------\n"); 

			while (1) { 
				// receive 
				clearBuf(net_buf); 
				nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
								sendrecvflag, (struct sockaddr*)&addr_con, 
								&addrlen); 

				// process 
				if (recvFile(net_buf, NET_BUF_SIZE)) { 
					break; 
				} 
			} 
			printf("\n---------File Downloaded---------\n"); 
		}

		// Put
		else if ( strcmp(user_command, "put") == 0){
			//printf("Puuuuut\n");

			printf("\nPlease enter file name to upload it to %s:\n", IP_ADDRESS); 
			scanf("%s", net_buf);
			while (1) { 
				printf("\nWaiting for file name...\n"); 

				// receive file name 
				//clearBuf(net_buf); 
				//clear buffer
				for(int i =0 ; i< NET_BUF_SIZE ; i++) 
						net_buf[i] = '\0';

				nBytes = recvfrom(sockfd, net_buf, 
								NET_BUF_SIZE, sendrecvflag, 
								(struct sockaddr*)&addr_con, &addrlen); 

				fp = fopen(net_buf, "r"); 
				printf("\nFile Name Received: %s\n", net_buf); 
				if (fp == NULL) 
					printf("\nFile open failed!\n"); 
				else
					printf("\nFile Successfully opened!\n"); 

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
					//clearBuf(net_buf); 
					//clear buffer
					for(int i =0 ; i< NET_BUF_SIZE ; i++) 
						net_buf[i] = '\0';
				} 
				if (fp != NULL) 
					fclose(fp); 
			}		
			
		}

		else
		{
			printf("\nCommand should be one of:\n get put quit\n");
		}
		
	}

	
	return 0; 
} 
