#ifndef SOCKLIB_HPP_GUARD
#define SOCKLIB_HPP_GUARD

#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <ctime>
#include <exception>
#include <sstream>
#include <iostream>
#include <boost/exception/all.hpp>

namespace utility {
void copy(std::string s, std::vector<char>& v) {
    for (auto c : s) {
        v.push_back(c);
    }
}

std::vector<char>
to_vector(std::string str) {
    std::vector<char> v;
    copy(str, v);
    return v;
}
}

/**
 * @file socklib.hpp
 * 
 *
 * @version 1.0
 * @since 2012-05-08 03:03:17 PM
 */
namespace udpsock
{

static const size_t MAXBUFLEN = 1500;

struct socket_data
{
    int sockfd;
    struct addrinfo hints;
    int port;
};

struct bind_socket_error : public boost::exception {};
struct addrinfo_error : public boost::exception {};


/** inet address endpoint */
struct Addr {
    struct sockaddr_storage addr;
    socklen_t len;

    Addr()
        : addr{0}
        , len(sizeof(addr))
    {}

    Addr(const std::string& str, const int port)
        : addr{0}
        , len(sizeof(addr))
    {
        socket_data data;
        data.port = port;

        std::stringstream addr;
        addr << port;
        auto address = addr.str();

        struct addrinfo *servinfo = nullptr;

        int rv = getaddrinfo(
                str.c_str(), address.c_str(), &data.hints, &servinfo);
        if (rv != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            throw addrinfo_error();
            }
        // loop through all the results and make a socket
        for(auto p = servinfo; p != nullptr; p = p->ai_next) {
            if ((data.sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("talker: socket");
                continue;
                }
            break;
	}

        //iz rv len
        freeaddrinfo(servinfo);

    }

    Addr(const decltype(addr)& addr, const decltype(len) len)
        : addr(addr)
        , len(len)
    {}

    bool operator==(const Addr& other) const {
        return memcmp(&addr, &other.addr, std::max(len, other.len)) == 0;
    }

    bool operator<(const udpsock::Addr& b) const
    {
        return memcmp(this, &b, sizeof(udpsock::Addr)) > 0;
    }
};

struct Message {
    Addr address;
    std::vector<char> message;
};


class Client {
    Addr addr;
    decltype(time(nullptr)) last_send_time;
    decltype(time(nullptr)) last_recv_time;

    public:
    Client() = default;
    Client(const Addr& address)
        : addr(address)
        , last_send_time(time(nullptr))
        , last_recv_time(time(nullptr))
    {}
    Client(const Client& client)
        : addr(client.addr)
        , last_send_time(client.last_send_time)
        , last_recv_time(client.last_recv_time)
    { }
    Client& operator=(const Client& client)
    {
        addr = client.addr;
        last_send_time = client.last_send_time;
        last_recv_time = client.last_recv_time;
        return *this;
    }

    void update_last_send_time() {
        last_send_time = time(nullptr);
    }
    void update_last_recv_time() {
        last_recv_time = time(nullptr);
    }
    double seconds_since_last_send() const
    {
        return difftime(time(nullptr), last_send_time);
    }
    double seconds_since_last_recv() const
    {
        return difftime(time(nullptr), last_recv_time);
    }

    int send(int sockfd, std::string buffer){
        auto result = sendto(
                sockfd,
                buffer.c_str(),
                buffer.length(),
                0,
                reinterpret_cast<const struct sockaddr *>(&addr.addr),
                addr.len
                );
        update_last_send_time();
        return result;
    }
    bool operator==(const Client& other) const {
        return addr == other.addr;
    }
};


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

socket_data make_socket(int port)
{
    socket_data data;
    data.port = port;

    std::stringstream addr;
    addr << port;
    auto address = addr.str();

	memset(&data.hints, 0, sizeof(data.hints));
	data.hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	data.hints.ai_socktype = SOCK_DGRAM;
	data.hints.ai_flags = AI_PASSIVE; // use my IP

    struct addrinfo *servinfo = nullptr, *p = nullptr;

    int rv = getaddrinfo(NULL, address.c_str(), &data.hints, &servinfo);
	if (rv != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        throw addrinfo_error();
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != nullptr; p = p->ai_next) {
		if ((data.sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == nullptr) {
        std::cerr << "server: failed to bind scoket\n";
        throw bind_socket_error();
	}

	freeaddrinfo(servinfo);
    return data;
}

std::pair<std::vector<char>, Addr>
get_message(int sockfd)
{
    std::vector<char> buffer(MAXBUFLEN);
    Addr addr;
    auto numbytes = recvfrom(
            sockfd, //socket
            &(buffer[0]), // buffer
            buffer.size()-1, //sizeof buffer
            0, //flags
            (struct sockaddr *)&addr.addr, // client addr
            &addr.len // address length
            );
    buffer.erase(buffer.begin()+numbytes,buffer.end());
    return make_pair(buffer, addr);
}

std::string to_string(const std::vector<char>& v) {
    std::string s;
    s.reserve(v.size());
    for (auto c : v) {
        if (c == '\0') {
            break;
        }
        s+=c;
    }
    return s;
}


class Socket{
    private:
        int port;
        socket_data info;

    public:
    Socket(int this_port)
        : port(this_port)
        , info(make_socket(port))
    {}

    udpsock::Message listen() {
        Message rtr;
        return rtr;
    }
    bool send(std::pair<Addr,Client> client, std::vector<char> message){
        
        return false;
    }
};

}


#endif
