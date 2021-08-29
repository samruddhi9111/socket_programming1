/*
Developed for DESD-Batch, May 2021
Dated : Aug,28, 2021
Task:
1.Design a concurrent TCP server using fork() system call
2.Can be used for bi directional chat application
3.Communication--Full Duplex
*/
#include <stdio.h>              //printf
#include <sys/types.h>          //Header for socket system call and accept
#include <sys/socket.h>         //Header for socket system call and accept
#include <stdlib.h>             //exit
#include <sys/socket.h>         // man 7 ip for (socket address struct sockaddr_in)
#include <netinet/in.h>         // man 7 ip for (socket address struct sockaddr_in)
#include <netinet/ip.h>         // man 7 ip for (socket address struct sockaddr_in)
#include <strings.h>            // bzero
#include <arpa/inet.h>          //htons man 3 htons - host to network byte order short
#include <unistd.h>             //read/write
/*
        Handler for Errors
*/

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main( int argc, char *argv[])
{
       //return types of socket, bind and accept
       int sockfd, retbind, connfd;
       //server and client structure declaration of type sockaddr_in (man 7 ip for details)
       struct sockaddr_in serv_addr, cli_addr;
       
        //clear the structure buffer with either bzero or memset

       bzero((char*)&serv_addr,sizeof(serv_addr));

       // Server port number and client length variable
       int port_number, cli_len;
       //buffer to be used for reading and writing
       char writebuf[1024];
       char readbuf[1024];
       
       //condition check for command line argument

       if (argc < 2)
       {
           printf("Kindly provide the port Number !! \n");
           printf("use this format : ./filename <portnumber> \n");
       }
        // first argument - command line - port number
        port_number = atoi(argv[1]) ;  // typecast to integer format 
        
        // Initialize the structure members

        /*
                Remember !!
                IP address and Port Number must be in the network byte order which is "Big Endian"
                So Keep uniformirt between client and server endianness it is good practice to explicitely make it to 
                the network byte order

               struct sockaddr_in {
               sa_family_t    sin_family;  address family: AF_INET 
               in_port_t      sin_port;   port in network byte order 
               struct in_addr sin_addr;   internet address 
           };

           /* Internet address. 
           struct in_addr {
               uint32_t       s_addr;     /* address in network byte order 
           };

        */

        serv_addr.sin_family = AF_INET;  // Address Family -> IPv4
        serv_addr.sin_port = htons(port_number);  // host byte order to network byte order (short int)
        serv_addr.sin_addr.s_addr = INADDR_ANY;  //  can be any IP address like "your system IP/loopnack/localhost" e.g "192.168.1.4"
        //serv_addr.sin_addr.s_addr = htonl("192.168.1.5"); specific ip must be in network byte order

        /*
        INADDR_ANY - will accept any incoming request and bind to all the available interfaces that are active e.g
        1. Ethernet
        2. Wi-fi
        3. localhost
        4. Cellular (via USB0/WWAN)

        In nutshell : client can use IP address of any of the interface to connect the server

        here are several special addresses: INADDR_LOOPBACK (127.0.0.1) always refers to 
        the local host via the loopback device; INADDR_ANY (0.0.0.0) means any address for binding;
        INADDR_BROADCAST (255.255.255.255) means any host and has the same effect on bind as INADDR_ANY 
        for historical reasons.
        */

       //create a listenning socket to accept incoming request (used only for listenning)
       sockfd = socket(AF_INET,SOCK_STREAM,0); //TCP - SOCK_STREAM
       // handle the Error in case socket is not created
       if (sockfd < 0)
       {
           error("Socket creation failed !! \n");
           exit(1);
       }
       else
       {
           printf("master socket creation successfull !!\n");
       }
       
       retbind = bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

       if(retbind == 0)
       {
           printf("server is running at port number : %d\n",port_number);
       }
       else
       {
           perror("failed to bind the IPv4 address to the socket !!\n");
           exit(1);
           
       }
       
      int number_of_backlog_connections = 5;
      int retlisten = listen(sockfd,number_of_backlog_connections);
      if (retlisten == 0)
      {
          printf("listen system call is working perfectly\n");
      }
      else
      {
          printf("listen system call failed to keep connections in the backlog\n");
      }

    cli_len = sizeof(cli_addr);
    // accept system call
    connfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    
    if (connfd < 0)
    {
        printf("Unable to create socket using accept system call \n");
        exit(0);
    }
    else
    {
        printf("connfd socket descriptor is created successfully having number as %d\n",connfd);
    }
    pid_t ret;
    ret=fork();
    /*
    ret =0 -- child process block
    ret>0 -- parent process block
    ret<0 --- fork execution failed
    */
    // Child process to read data from the client application
    if(ret==0)
    {
        while(1)
        {
                bzero(readbuf,sizeof(readbuf));
                read(connfd,&readbuf,sizeof(readbuf));
                printf("Msg from Client : %s\n",readbuf);
        }
    }
    else if(ret>0)
    {
        while(1)
        {
             bzero(writebuf,sizeof(writebuf));
             int buffer_count = 0;
             printf("Msg to client : ");
             while((writebuf[buffer_count++] = getchar()) != '\n');
             write(connfd,&writebuf, sizeof(writebuf));



        }
    }
    else 
    {
        if(ret<0)
        {
            perror("Failed to create child process !! fork() failed !!!!");
        }
    }

    
    close(connfd);      // close the accept socket/file descriptor
    close(sockfd);      // close the socket socket/file descriptor

    return 0;

}