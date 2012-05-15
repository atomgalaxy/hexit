/*
** listener.c -- a datagram sockets "server" demo
*/

#include "socklib.hpp"
#include <iostream>
#include <unordered_map>
#include <map>

static const int MYPORT = 4950;	// the port users will be connecting to
static const int HISPORT = 4951;
static const std::string HISHOSTNAME = "localhost";

int main(int argc, char** argv) {
    if (argc != 3) { std::cerr << "Usage: " << argv[0] << " address port" << std::endl;
        return 1;
    }

    auto hostname = std::string(argv[1]);
    auto port     = 0;

    std::stringstream ss(argv[2]);
    ss >> port;
    std::cerr << "connecting to " << addr << ":" << port << std::endl;

    auto socket = udpsock::AnonSocket();
    auto target = udpsock::Addr(hostname, port);

    auto message = std::vector<char>();

    utility::copy("My Bonny is over the ocean...", message);

    socket.send(target, message);
    auto reply = socket.listen();
    std::cout << reply.address << " :" reply.message << std::endl;
}


//int main(int argc, char** argv){
//    using namespace udpsock;
//    std::map<Addr, Client> clients;
//    int listenport = MYPORT;
//    if (argc > 1) {
//        std::stringstream s(argv[1]);
//        s >> listenport;
//        std::cerr << "listen port: " << listenport << std::endl;
//    }
//    auto socket = make_socket(port);

//    if (argc > 2) {
//        std::stringstream s(argv[1]);
//        s >> sendport;
//        std::cerr << "send port: " << sendport << std::endl;
//    }
//    int sendport = HISPORT;

//    // make client
//    Client other(Addr(HISHOSTNAME, HISPORT));

//    // RECVFROM
//    printf("listener: waiting to recvfrom...\n");

//    decltype(get_message(socket.sockfd)) r;
//    do {
//        r = get_message(socket.sockfd);
//        if (r.first.size() != 0) {
//            std::cout << to_string(r.first) << std::endl;

//            if (clients.count(r.second) > 0) {
//                clients[r.second].update_last_recv_time();
//            } else {
//                clients[r.second] = Client(r.second);
//            }
//            std::cerr << "Have a new client!" << std::endl;
//        }

//        // now cycle over our clients and send to each if delay exceeded or
//        // something.
//        for(auto& client : clients){
//            if (client.second.seconds_since_last_send() > 0.9) {
//                client.second.send(socket.sockfd, "say sth");
//            }
//        }
//    } while (to_string(r.first) != "stop");

//    close(socket.sockfd);
//    std::cout << "konec";
//    return 0;
//}



//int main(int argc, char *argv[])
//{
//    int sockfd;
//    struct addrinfo hints, *servinfo, *p;
//    char buf[MAXBUFLEN];
//    int rv;
//    int numbytes;

//    if (argc != 3) {
//        fprintf(stderr,"usage: talker hostname message\n");
//        exit(1);
//    }

//    memset(&hints, 0, sizeof hints);
//    hints.ai_family = AF_UNSPEC;
//    hints.ai_socktype = SOCK_DGRAM;

//    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//        return 1;
//    }

//    // loop through all the results and make a socket
//    for(p = servinfo; p != NULL; p = p->ai_next) {
//        if ((sockfd = socket(p->ai_family, p->ai_socktype,
//                p->ai_protocol)) == -1) {
//            perror("talker: socket");
//            continue;
//        }

//        break;
//    }

//    if (p == NULL) {
//        fprintf(stderr, "talker: failed to bind socket\n");
//        return 2;
//    }

//    if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
//             p->ai_addr, p->ai_addrlen)) == -1) {
//        perror("talker: sendto");
//        exit(1);
//    }
//    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
//    //dodaj cakanje na prejem podatkov
//    numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
//            (struct sockaddr *)&their_addr, &addr_len);
//    //ko so prejeti jih izpisi
//    freeaddrinfo(servinfo);
//    close(sockfd);

//    return 0;
//}
