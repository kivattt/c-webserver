#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

struct StrPair {
	char *key;
	char *value;
};

struct StrMap{
	struct StrPair *array;
};

char *map_value_from_key(struct StrMap map, const size_t mapSize, const char *key);

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
};

struct HTTPRequest parse_request(char *request);
struct URI parse_uri(const char *uri);
char *parse_percent_encoding(const char *input);
struct StrMap parse_urlencoded(const char *urlEncoded);

#endif // HTTP_H
