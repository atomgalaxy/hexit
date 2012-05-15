/*
** listener.c -- a datagram sockets "server" demo
*/

#include "socklib.hpp"
#include <iostream>
#include <unordered_map>
#include <map>

static const int MYPORT = 4950;	// the port users will be connecting to

int main(int argc, char** argv){
    using namespace udpsock;
    /* read commandline arguments */
    if (argc != 2) { std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }

    auto port     = 0;

    std::stringstream ss(argv[1]);
    ss >> port;
    std::cerr << "Listening on " << port << std::endl;
    /* end reading commandline arguments */

    std::map<Addr, Client> clients;

    auto socket = udpsock::Socket(port);
    while (true) {
        auto reply = socket.listen();
        if (reply.message == utility::to_vector("stop")) {
            std::cout << "Stopping..." << std::endl;
            break;
        }

        if (clients.count(reply.address) > 0) {
            clients[reply.address].update_last_recv_time();
        } else {
            clients[reply.address] = Client(reply.address);
            std::cerr << "Have a new client!" << std::endl;
        }

        for(auto& client : clients){
            if (client.second.seconds_since_last_send() > 0.9) {
                socket.send(client, reply.message);
            }
        }

    }

    std::cout << "konec.";
	return 0;
}
