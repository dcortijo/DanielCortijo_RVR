#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

#define MAX_THREAD 10

class EJ7_thread
{
    public:
        EJ7_thread(int sd) : _sd(sd) {};
        void do_message() 
        {
            char buffer[80];

            while(true)
            {
                int bytes = recv(_sd, (void*) buffer, 80, 0);

                if(bytes < 0)
                {
                    std::cout << "Error al recibir: " << strerror(errno) << " en thread " << std::this_thread::get_id() <<"\n";
                    return;
                }
                else if (bytes == 0)
                {
                    std::cout << "Fin de la conexión\n";
                    return;
                }

                std::cout << bytes << " recibidos de " << _host << ":" << _serv << " en thread " << std::this_thread::get_id() <<"\n";

                if(bytes < 80) buffer[bytes] = '\0';
                else buffer[80] = '\0';
                
                if(send(_sd, buffer, bytes, 0) < 0)
                {
                    std::cout << "Error enviando mensaje: " << strerror(errno) << "\n";
                    break;
                }

                sleep(5);
            }
            close(_sd);
        }
        void setup_client(char host[NI_MAXHOST], char serv[NI_MAXSERV])
        {
            _host = host;
            _serv = serv;
        };
    private:
        int _sd;
        std::string _host;
        std::string _serv;
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
        return -1;
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
            std::cout << "[accpet]: " << strerror(errno) << "\n";
            return -errno;
        }

        if(getnameinfo(&cliente, clientelength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) < 0)
        {
            std::cout << "[getnameinfo]: " << strerror(errno) << "\n";
            return -errno;
        };

        std::cout << "Conexión desde " << host << ":" << serv << "\n";
        
        EJ7_thread* mT = new EJ7_thread(cs);
        mT->setup_client(host, serv);   // Para que pueda hacer el cout con host y serv apropiados
        std::thread([mT](){
            mT->do_message();
            delete mT;
        }).detach();
    }

    close(sd);

    return 0;
}