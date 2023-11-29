#include <string>
#include <map>
#include <optional>
#include <iterator>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string.h>

#include "response.h"
#include "utils/splitstring.h"

namespace Server {
	ExtensionMapping::ExtensionMapping(std::string extension, std::ios_base::openmode mode, std::string content_type) {
		this->extension		= extension;
		this->mode			= mode;
		this->content_type	= content_type;
	}

	ResponseData::~ResponseData() {
		delete this->response;
	}

	void generate_link_map(std::map<std::string, std::string>& map) {
		map.insert({"", "index.html"});
		map.insert({"about", "about.html"});
	}

	std::string convert_link(const std::map<std::string, std::string>& link_map, const std::string& link, bool& successful) {
		std::map<std::string, std::string>::const_iterator link_iterator = link_map.begin();
		
		std::cout << "converting" << std::endl;
		while (link_iterator != link_map.end()) {
			std::cout << "converting" << std::endl;
			if (link_iterator->first == link) {
				successful = true;
				return link_iterator->second; 
			}
			link_iterator++;
		}
		
		successful = false;
		std::cout << "done" << std::endl;
		return "";
	}


	std::unique_ptr<ResponseData> generate_response(const std::string& path, bool& successful) {
		std::unique_ptr<ResponseData> response = std::unique_ptr<ResponseData>(new ResponseData);
		std::cout << path << "PATH" << std::endl;
		std::vector<std::string> get_extension = Utils::split_string(path, '.');
		std::ios_base::openmode open_mode;
		bool found_answer = false;

		std::cout << "started thing" << std::endl;
		std::cout << get_extension.size() << std::endl;
		if (get_extension.size() <= 0) {
			std::cerr << "couldn't parse get request" << std::endl;
			successful = false;
		}
		for (ExtensionMapping extension : extension_maps) {
			if (extension.extension == get_extension.at(get_extension.size()-1)) {
				open_mode = extension.mode;
				response->response_type = extension.content_type;
				found_answer = true;

			}
		}
		std::cout << "finished thing" << std::endl;

		std::ifstream stream(file_path + path, open_mode);

		if (!stream.is_open()) {
			stream.close();
			successful = false;
			return response;
		}
		stream.seekg(std::ios::end);
		response->response_length = stream.tellg();
		stream.seekg(std::ios::beg);
		//IF THERE IS A GARBAGE CHARACTER IN SENT FILES SUBTRACT 1 FROM THE LENGTH
		response->response = new char[response->response_length];

		int file_place = 0;
		char next_char;
		std::cout << "started parsing" << std::endl;
		while ((next_char = stream.get())) {
			response->response[file_place] = next_char;
			file_place++;
		}
		std::cout << "finished parsing" << std::endl;
		stream.close();

		return response;
	}

	std::unique_ptr<char[]> generate_response_text(const ResponseData& response_data, const std::string &response_header, size_t& response_size) {
		std::string response = std::string(response_header.c_str());
		response.append(response_data.response_type);
		response.append("Server: webserver-c\r\n");
		size_t final_response_size = response.size() + response_data.response_length;
		response_size = final_response_size;
		std::unique_ptr<char[]> char_response = std::unique_ptr<char[]>(new char[final_response_size]);

		int current_char = 0;
		while (current_char < response.size()) {
			char_response[current_char] = response.at(current_char);
			current_char++;
		}

		while (current_char - response.size() < response_data.response_length) {
			char_response[current_char] = response_data.response[current_char - response.size()];
			current_char++;
		}
		return char_response;
	}
}
