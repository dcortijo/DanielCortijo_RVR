#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    
    if(argc < 4)
    {
        std::cout << "Formato incorrecto: <dirección> <puerto> <comando>\n";
        return -1;
    }
    
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    //hints.ai_protocol = SOCK_DGRAM;
    hints.ai_socktype = SOCK_DGRAM;

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
        return -1;
    }


    if(sendto(sd, argv[3], 1, 0, res->ai_addr, res->ai_addrlen) < 0)
    {
        std::cout << "Error enviando mensaje\n";
        return -1;
    };

    char buffer[80];

    if(recvfrom(sd, (void*) buffer, 80, 0, res->ai_addr, &res->ai_addrlen) < 0)
    {
        std::cout << "Error en revfrom\n";
        return -1;
    }

    std::cout << buffer << "\n";

    close(sd);
    freeaddrinfo(res);

    return 0;
}