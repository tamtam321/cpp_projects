#ifndef EXCEPTIONS_HPP_INCLUDED
#define EXCEPTIONS_HPP_INCLUDED

#include <exception>

class internal_error: public std::exception {
	const char *message;

public:
	internal_error(const char *msg = "Hiba van az implementacioban!") :
			message(msg) {
	}

	const char *what() const throw () {
		return message;
	}
};

class invalid_binary_search_tree: public std::exception {
public:
	const char *what() const throw () {
		return "Serulnek a binaris keresofa kriteriumok!";
	}
};

class invalid_rb_tree: public std::exception {
public:
	const char *what() const throw () {
		return "Serulnek az piros-fekete fa tulajdonsagok!";
	}
};

#endif // EXCEPTIONS_HPP_INCLUDED
