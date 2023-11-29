#pragma once

#include <string>
#include <map>

namespace Server {
	void handle_request(const std::string& requests, std::map<std::string, std::string> link_map, int server_fd, int destination_socket);
}
