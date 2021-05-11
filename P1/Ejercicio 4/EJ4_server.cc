#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    
    if(argc < 3)
    {
        std::cout << "Formato incorrecto: <dirección> <puerto>\n";
        return -1;
    }
    
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << "\n";
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1)
    {
        std::cerr << "[socket] " << strerror(errno) << "\n";
        return -errno;
    }

    if(bind(sd, res->ai_addr, res->ai_addrlen) < 0){
        std::cout << "[bind] " << strerror(errno) << "\n";
        return -errno;
    };
    
    freeaddrinfo(res);

    if(listen(sd, 5) < 0){
        std::cout << "[listen] " << strerror(errno) << "\n";
        return -errno;
    };   

    char buffer[80];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t       clientelength = sizeof(struct sockaddr);

    while(true){
        int cs = accept(sd, &cliente, &clientelength);

        if(cs < 0)
        {
            return -1;
        }

        if(getnameinfo(&cliente, clientelength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) < 0)
        {
            std::cout << "[getnameinfo]: error\n";
            return -1;
        };

        std::cout << "Conexión desde " << host << ":" << serv << "\n";
        while(true){
            int bytes = recvfrom(cs, buffer, 80, 0, &cliente, &clientelength);

            if(bytes < 0){
                std::cout << "[recv] " << strerror(errno) << "\n";
                return -errno;
            }
            else if(bytes == 0)
            {
                std::cout << "Conexión terminada\n";
                break;
            }

            if(sendto(cs, buffer, bytes, 0, &cliente, clientelength) < 0)
            {
                std::cout << "Error enviando mensaje\n";
                return -1;
            };  
        }
        close(cs);
    }

    close(sd);


    return 0;
}