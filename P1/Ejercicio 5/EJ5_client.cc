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

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res); // Socket arbitrario

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1)
    {
        std::cerr << "[socket]: " << strerror(errno) << "\n";
        return -errno;
    }

    char buffer[80];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    if(connect(sd, res->ai_addr, res->ai_addrlen) < 0)
    {
        std::cout << "[connect]: " << strerror(errno) << "\n";
        return -1;
    }

    while(true){
        std::cin >> buffer;

        if(strcmp(buffer, "Q\0") == 0) break;

        if(sendto(sd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen) < 0)
        {
            std::cout << "Error enviando mensaje: " << strerror(errno) << "\n";
            return -errno;
        }; 

        int bytes = recvfrom(sd, buffer, 80, 0, res->ai_addr, &res->ai_addrlen);

        if(bytes < 0){
            std::cout << "[recv] " << strerror(errno) << "\n";
            return -errno;
        }
        else if(bytes == 0)
        {
            break;
        }
        else
        {
            std::cout << buffer << "\n";
        }
    }
        
    freeaddrinfo(res);
    
    close(sd);

    return 0;
}