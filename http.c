#ifndef HTTP_C
#define HTTP_C

#include "http.h"

/*struct StrPair {
	char *key;
	char *value;
};

struct StrMap{
	struct StrPair *array;
};

struct URI{
	char *path;
	struct StrMap queries;
	char *fragment;
};

struct HTTPRequest {
	char *method;
	struct URI uri;
	char *httpVersion;
	struct StrMap headers;
	char *body;
};

struct HTTPResponse {
	char *httpVersion;
	struct StrMap headers;
	char *body;
};*/

struct HTTPRequest get_parsed_request(const char *request) {
	struct HTTPRequest ret;

//	ret.method = strtok(request, ' ');
	ret.method = strtok(request, " ");

	return ret;
}

#endif // HTTP_C
