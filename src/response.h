#pragma once

#include <string>
#include <optional>
#include <map>
#include <iostream>
#include <memory>
#include <fstream>

#define MAX_GET_REQUEST_LENGTH 10000

namespace Server {
	typedef struct ResponseData {
		~ResponseData();
		char* response;
		size_t response_length;
		std::string response_type;
	} ResponseData;

	typedef struct ExtensionMapping {
		ExtensionMapping(std::string extension, std::ios_base::openmode mode, std::string content_type);
		std::string extension;
		std::ios_base::openmode mode;
		std::string content_type;
	} ExtensionMapping;

	const std::string file_path = "../sitedata";
	const std::string not_found_path = "/404error.html";
	const ExtensionMapping extension_maps[] = { ExtensionMapping("html", std::ifstream::in, "text/html"), 
												ExtensionMapping("css", std::ifstream::in, "text/css"),
												ExtensionMapping("js", std::ifstream::in, "text/js"),
												ExtensionMapping("ico", std::ifstream::binary, "image/x-icon"),
												ExtensionMapping("jpg", std::ifstream::binary, "image/jpeg"),
												ExtensionMapping("jpeg", std::ifstream::binary, "image/jpeg"),
												ExtensionMapping("png", std::ifstream::binary, "image/png") };

	void generate_link_map(std::map<std::string, std::string>& map);
	void generate_links(std::map<std::string, std::string>& link_map);
	std::string convert_link(const std::map<std::string, std::string>& link_map, const std::string& link, bool& successful);
	std::unique_ptr<ResponseData> generate_response(const std::string& path, bool& successful);
	std::unique_ptr<ResponseData> generate_404_response();
	std::unique_ptr<char[]> generate_response_text(const ResponseData& response_data, const std::string &response_header, size_t& response_size);
};
