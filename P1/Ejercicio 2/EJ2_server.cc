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

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    char buffer[80];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t       clientelength = sizeof(struct sockaddr);

    int bytes = recvfrom(sd, (void*) buffer, 80, 0, &cliente, &clientelength);

    if(bytes == -1)
    {
        return -1;
    }

    getnameinfo(&cliente, clientelength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << bytes << " recibidos de " << host << ":" << serv << "\n";

    while(buffer[0] != 'q')
    {
        time_t rawtime;
        struct tm *info;
        time( &rawtime );
        info = localtime( &rawtime );
        char timebuf[80];
        int timesize;
        switch(buffer[0])
        {
            case 't':
                {
                timesize = strftime(timebuf, 255, "%X %p", info);
                timebuf[timesize] = '\0';
                if(sendto(sd, timebuf, timesize + 1, 0, &cliente, clientelength) < 0)
                {
                    std::cout << "Error enviando mensaje\n";
                    return -1;
                };
                }
                break;
            case 'd':
                {
                timesize = strftime(timebuf, 255, "%Y-%m-%d", info);
                timebuf[timesize] = '\0';
                if(sendto(sd, timebuf, timesize + 1, 0, &cliente, clientelength) < 0)
                {
                    std::cout << "Error enviando mensaje\n";
                    return -1;
                };
                }
                break;
            default:
                std::cout << "Comando no soportado " << buffer[0] << "\n";
                break;
        }

        int bytes = recvfrom(sd, (void*) buffer, 80, 0, &cliente, &clientelength);

        if(bytes == -1)
        {
            return -1;
        }

        getnameinfo(&cliente, clientelength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << bytes << " recibidos de " << host << ":" << serv << "\n";
    }

    std::cout << "Saliendo...\n";

    close(sd);

    return 0;
}