#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t dataSize = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(dataSize);

        char * tmp = _data;

        memcpy(tmp, name, MAX_NAME * sizeof(char));

        tmp += MAX_NAME * sizeof(char);

        memcpy(tmp, &x, sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        int32_t dataSize = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(dataSize);   // Setear el size por si acaso

        char * tmp = data;

        memcpy(name, tmp, MAX_NAME * sizeof(char));

        tmp += MAX_NAME * sizeof(char);

        memcpy(&x, tmp, sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(&y, tmp, sizeof(int16_t));

        return 0;
    }


public:
    int16_t x;
    int16_t y;

    static const size_t MAX_NAME = 20;
    
    char name[MAX_NAME];
};

int main(int argc, char **argv)
{
    Jugador one_w("player one", 12, 345);           // Se crea jugador arbitrario

    int fd = open("./player_serialized", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if(fd < 0){
        std::cerr << "[open]: " << strerror(errno) << "\n";
    }
    
    one_w.to_bin();                                 // Se serializa
                        
    if(write(fd, one_w.data(), one_w.size()) < 0){  // Se escribe
        std::cerr << "[write]: " << strerror(errno) << "\n";
    }  
    if(close(fd) < 0){                              // Se cierra
        std::cerr << "[close]: " << strerror(errno) << "\n";
    }                            

    Jugador one_cpy("this should be player one", 0, 0);           // Se crea otro jugador arbitrario
    
    char * tmp = new char[one_w.size()];

    int fs = open("./player_serialized", O_RDONLY, 0666);
    if(fs < 0){
        std::cerr << "[open]: " << strerror(errno) << "\n";
    }

    if(read(fs, tmp, one_w.size()) < 0){           // Se lee
        std::cerr << "[read]: " << strerror(errno) << "\n";
    }

    one_cpy.from_bin(tmp);

    std::cout << "Jugador copiado: " << one_cpy.name << " | " << one_cpy.x << ", " << one_cpy.y << "\n";

    return 0;
}

