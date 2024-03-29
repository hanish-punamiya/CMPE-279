// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include<sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080


int drop_privilege(){
    struct passwd *pwd;
    pid_t childPid,pid;
    int status = 0;

    childPid = fork();

    if(childPid ==0){
        //fork is successful
        // printf("\n fork sucessful \n");
        //getting user id with lesser privileges. ie. nobody
        pwd = getpwnam("nobody");
        if(pwd == NULL){
	        perror("Drop Privilage failure. pwd pointer is null");
	        exit(EXIT_FAILURE);	
	    }
        // assigning
        pid = setuid(pwd->pw_uid);
        if(pid==0){
            return 1;
        }
    }
    else if(childPid>0){
        // printf("Log: Parent: Waiting for child process\n");
        if(( pid = wait(&status)) < 0){
            perror("Log: Parent: Error in wait");
            exit(1);
        }
        // printf("Log: Parent: Forked child pid: %d\n", childPid);  
        // printf("Log: Parent: Completed execution\n");

    }
     else{
        perror("Error! fork() unsuccessful");
        exit(2);                                                                                                                                          
    }
    return 0;

}



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

    if(drop_privilege()){
         //message processing
            valread = read(new_socket, buffer, 1024);
            printf("Read %d bytes: %s\n", valread, buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
    }
    return 0;
}
