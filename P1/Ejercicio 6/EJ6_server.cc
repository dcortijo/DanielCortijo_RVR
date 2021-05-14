#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

#define MAX_THREAD 10

class EJ6_thread
{
    public:
        EJ6_thread(int sd) : _sd(sd) {};
        void do_message() 
        {
            char buffer[80];

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            struct sockaddr cliente;
            socklen_t       clientelength = sizeof(struct sockaddr);

            while(true)
            {
                int bytes = recvfrom(_sd, (void*) buffer, 80, 0, &cliente, &clientelength);

                if(bytes == -1)
                {
                    std::cout << "Error al recibir: " << strerror(errno) << "\n";
                    continue;
                }

                getnameinfo(&cliente, clientelength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

                std::cout << bytes << " recibidos de " << host << ":" << serv << " en thread " << std::this_thread::get_id() <<"\n";

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
                        if(sendto(_sd, timebuf, timesize + 1, 0, &cliente, clientelength) < 0)
                        {
                            std::cout << "Error enviando mensaje: " << strerror(errno) << "\n";
                            continue;
                        }
                        break;
                    }
                    break;
                    case 'd':
                    {
                        timesize = strftime(timebuf, 255, "%Y-%m-%d", info);
                        timebuf[timesize] = '\0';
                        if(sendto(_sd, timebuf, timesize + 1, 0, &cliente, clientelength) < 0)
                        {
                            std::cout << "Error enviando mensaje: " << strerror(errno) << "\n";
                            continue;
                        }
                        break;
                    }
                    break;
                    default:
                        std::cout << "Comando no soportado " << buffer[0] << "\n";
                    break;
                }

                sleep(5);
            }
        }
    private:
        int _sd;
};

int main(int argc, char** argv)
{
    
    if(argc < 3)
    {
        std::cout << "Formato incorrecto: <dirección> <puerto>\n";
        return -1;
    }
    
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
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

    for(int i = 0; i < MAX_THREAD; ++i)
    {
        EJ6_thread* mT = new EJ6_thread(sd);
        std::thread([mT](){ // No referencia, si no se cambia
            mT->do_message();
            delete mT;
        }).detach();
    }

    char s[80]; 
    while(true)
    {
        std::cin >> s;
        if(strcmp(s, "q\0") == 0) break;
    }

    std::cout << "Saliendo...\n";

    close(sd);

    return 0;
}