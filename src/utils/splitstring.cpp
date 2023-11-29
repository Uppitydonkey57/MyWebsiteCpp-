#include <iostream>

#include "splitstring.h"

namespace Utils {
	std::vector<std::string> split_string(const std::string& split, char split_char) {
		std::vector<std::string> split_vec;

		std::string temp = "";

		for(int i=0; i<(int)split.size(); i++){
			// If cur char is not del, then append it to the cur "word", otherwise
			// you have completed the word, print it, and start a new word.
			if(split[i] != split_char){
				temp += split[i];
			}
			else {
				split_vec.push_back(temp);
				temp = "";
			}
		}

		if (split_vec.size() == 0) {
			split_vec.push_back(split);
		}

		return split_vec;
	}
}
