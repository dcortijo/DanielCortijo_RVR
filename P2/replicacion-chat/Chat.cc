#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char * tmp = _data;

    memcpy(tmp, &type, 1);

    tmp += 1;

    memcpy(tmp, &nick, 8);

    tmp += 8;

    memcpy(tmp, &message, 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char * tmp = _data;

    memcpy(&type, tmp, 1);

    tmp += 1;

    memcpy(&nick, tmp, 8);

    tmp += 8;

    memcpy(&message, tmp, 80);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        ChatMessage newMsg;
        Socket* newSock;

        socket.recv(newMsg, newSock); // Asigna mensaje y socket

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        switch (newMsg.type)
        {
        case ChatMessage::LOGIN:
        {
            clients.push_back(std::move(std::make_unique<Socket>(*newSock)));      // Añade y mueve
            break;
        }
        case ChatMessage::MESSAGE:
        {
            auto s = clients.begin();
            while(s != clients.end()){   // Si es distinto envía
                if(s->get() != newSock) socket.send(newMsg, *(s->get()));
                ++s;
            }
            break;
        }
        case ChatMessage::LOGOUT:
        {
            auto s = clients.begin();
            while(s != clients.end()){   // Si es igual lo borra
                if(s->get() == newSock){
                    clients.erase(s);
                    s->release();
                } 
                ++s;
            }
            break;
        }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

