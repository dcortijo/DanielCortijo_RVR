#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    
    if(argc < 2)
    {
        std::cout << "Formato incorrecto: ./gai <dirección>\n";
        return -1;
    }
    
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(argv[1], nullptr, &hints, &res);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << "\n";
        return -1;
    }

    for(auto i = res; i != nullptr; i = i->ai_next)
    {
        char host[NI_MAXHOST];

        if(getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, nullptr, NI_MAXSERV, NI_NUMERICHOST) != 0)
        {
            std::cerr << "[getnameinfo] " << gai_strerror(rc) << "\n";
            return -1;
        }

        std::cout << host << "\t" << i->ai_family << "\t" << i->ai_socktype << "\n";
    }

    return 0;
}