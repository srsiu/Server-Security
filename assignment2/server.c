// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Show ASLR
    printf("execve=0x%p\n", execve);

    if(argc == 1){
    	    // Creating socket file descriptor
	    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	    {
		perror("socket failed");
		exit(EXIT_FAILURE);
	    }

	    // Attaching socket to port 80
	    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
			                                  &opt, sizeof(opt)))
	    {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	    }
	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = INADDR_ANY;
	    address.sin_port = htons( PORT );

	    // Forcefully attaching socket to the port 80
	    if (bind(server_fd, (struct sockaddr *)&address,
			                 sizeof(address))<0)
	    {
		perror("bind failed");
		exit(EXIT_FAILURE);
	    }
	    if (listen(server_fd, 3) < 0)
	    {
		perror("listen");
		exit(EXIT_FAILURE);
	    }
	    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
			       (socklen_t*)&addrlen))<0)
	    {
		perror("accept");
		exit(EXIT_FAILURE);
	    }
	 

	    // separate processes
	    if(fork() == 0) {  // child process
		char sockStr[10]; 
		sprintf(sockStr, "%d", new_socket);
		char *new_argv [] = {"./server", sockStr, NULL};

		//exec with arguments
		printf("fork successful, begin exec\n");
		execvp(argv[0], new_argv);
	    } else { // parent process
		int status = 0;
		pid_t wpid;
		while((wpid = wait(&status)) > 0); // wait for child to finish
		printf("child process exited, back to parent\n");
	    }
	    
	    return 0;
    } else {
	printf("child process after exec, now dropping privileges\n");

	struct passwd *pd;
	uid_t uid;
	if(NULL == (pd = getpwnam("nobody"))){
	    perror("getpwnam() error");
	    exit(EXIT_FAILURE);
	} else {
	    uid = pd->pw_uid;
	    printf("child id is: %u\n", uid);
	    setuid(uid);
	}

	int dup_socket = atoi(argv[1]);
	valread = read( dup_socket , buffer, 1024);
	printf("%s\n",buffer );
	send(dup_socket , hello , strlen(hello) , 0 );
	printf("Hello message sent\n");
	exit(0);
	}
}
