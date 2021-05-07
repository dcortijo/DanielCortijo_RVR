#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    
    if(argc < 2)
    {
        std::cout << "Formato incorrecto: EJ1 <dirección>\n";
        return -1;
    }
    
    struct addrinfo * res;

    // hints innecesario, ay que no se quiere especificar nada en concreto
    int rc = getaddrinfo(argv[1], nullptr, nullptr, &res);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << "\n";
        return -1;
    }

    for(auto i = res; i != nullptr; i = i->ai_next)
    {
        char host[NI_MAXHOST];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, nullptr, NI_MAXSERV, NI_NUMERICHOST);

        std::cout << host << "\t" << i->ai_family << "\t" << i->ai_socktype << "\n";
    }

    return 0;
}