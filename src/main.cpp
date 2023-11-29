#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <string>

#include "main.h"
#include "response.h"
#include "utils/splitstring.h"

#define PORT 8080

int main(int argc, char const* argv[])
{
	std::map<std::string, std::string> name_to_link;
	Server::generate_link_map(name_to_link);
    int server_fd;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
	std::string hello = "<html><h1> HELLO WOLRD </h1></html>";
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket failed" << std::endl;
        return 1;
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        std::cerr << "setsockopt" << std::endl;
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        std::cerr << "bind failed" << std::endl;
        return 1;
    }
    if (listen(server_fd, 3) < 0) {
        std::cerr << "listen" << std::endl;
        return 1;
    }
	while (1) {
		int new_socket;
		if ((new_socket
			 = accept(server_fd, (struct sockaddr*)&address,
					  &addrlen))
			< 0) {
			std::cerr << "accept" << std::endl;
			return 1;
		}
		valread = read(new_socket, buffer,
					   1024 - 1); // subtract 1 for the null
								  // terminator at the end
		printf("%s\n", buffer);
		std::string full_message = "HTTP/1.0 200 OK\r\n"
			"Server: webserver-c\r\n"
			"Content-type: text/html\r\n\r\n";
		full_message.append(hello);
		//send(new_socket, full_message.c_str(), full_message.size(), 0);
		std::cout << full_message << std::endl;
		Server::handle_request(std::string(buffer), name_to_link, server_fd, new_socket);
		// closing the connected socket
		close(new_socket);
		// closing the listening socket
	}
	close(server_fd);
    return 0;
}

namespace Server {
	void handle_request(const std::string& request, std::map<std::string, std::string> link_map, int server_fd, int destination_socket) {
		std::vector<std::string> request_vec = Utils::split_string(request, '\n');
		std::string parsed_lines = request_vec.at(0);
		std::vector<std::string> parsed_vec = Utils::split_string(parsed_lines, ' ');
		std::string parsed_request = parsed_vec.at(1);
		bool conversion_succesful = false;
		std::string convert_request = convert_link(link_map, parsed_request, conversion_succesful);
		std::unique_ptr<ResponseData> response;
		bool response_generation_succesful = false;
		std::cout << parsed_request << " " << conversion_succesful << std::endl;
		if (conversion_succesful == false) {
			response = generate_response(convert_request, response_generation_succesful);
		}
		else {
			response = generate_response(parsed_request, response_generation_succesful);
		}
		if (!response_generation_succesful) {
			std::cerr << "Couldn't generate response: " << parsed_request << std::endl;
			return;
		}
		size_t response_size;
		std::unique_ptr<char[]> final_response = generate_response_text(*response, std::string("HTTP/1.0 200 OK\r\n"), response_size);
		send(destination_socket, final_response.get(), (int)response_size, 0);
	}
}
