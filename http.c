#ifndef HTTP_C
#define HTTP_C

#include <asm-generic/errno-base.h>
#include <errno.h>
#include <string.h>

#include "http.h"

char *map_value_from_key(struct StrMap map, const size_t mapSize, const char *key) {
	for (unsigned long long i = 0; i < mapSize; i++) {
		if (map.array[i].key == key)
			return map.array[i].value;
	}

	return NULL;
}

struct StrMap parse_urlencoded(const char *urlEncoded) {
	struct StrMap ret;

	enum DecoderState {
		QueryName,
		QueryValue
	};

	enum DecoderState state = QueryName;
	char *queryNameBuf;

	for (unsigned long long i=0; i < strlen(urlEncoded); i++) {
		char c = urlEncoded[i];

		if (c == '&') {
			state = QueryName;
			continue;
		} else if (c == '=') {
			state = QueryValue;
			continue;
		}

		if (state == QueryName)
			queryNameBuf += c;
		else if (state == QueryValue)
			*map_value_from_key(ret, sizeof(ret.array)/sizeof(ret.array[0]), queryNameBuf) += (c == '+' ? ' ' : c);
	}

	return ret;
}

// If this returns "", check errno
char *parse_percent_encoding(const char *input) {
	char *ret = "";

	for (unsigned long long i=0; i < strlen(input); i++) {
		if (input[i] == '%') {
			++i;

			char escapedChar = 0;
			for (unsigned char escapeIndexDelta = 0; escapeIndexDelta < 2; escapeIndexDelta++) {
				if (i + escapeIndexDelta >= strlen(input)) {
					errno = EINVAL;
					return "";
				}

				char *hex = "0123456789ABCDEF";
				char *chDigitIndex = strchr(hex, input[i + escapeIndexDelta]);
				if (chDigitIndex == NULL) {
					errno = EINVAL;
					return "";
				}

				int digitIndex = (int)(hex - chDigitIndex); // Cursed
				escapedChar |= (digitIndex << ((escapeIndexDelta ^ 1) << 2));
			}

			++i;
			ret += escapedChar;
		}

		ret += input[i];
	}

	return ret;
}

struct URI parse_uri(const char *uri) {
	enum UriDecoderState {
		Path,
		Queries,
		Fragment
	};

	struct URI ret;

	enum UriDecoderState state = Path;

	char *queriesStringToParse = "";
	for (unsigned long long i = 0; i < strlen(uri); i++) {
		char c = uri[i];

		if (c == '?') {
			state = Queries;
			continue;
		} else if (c == '#') {
			state = Fragment;
			continue;
		}

		if (state == Path) {
			if (c == '+')
				ret.path += ' ';
			else
				ret.path += c;
		} else if (state == Queries) {
			queriesStringToParse += c;
		} else if (state == Fragment) {
			ret.fragment += c;
		}
	}

	ret.queries = parse_urlencoded(parse_percent_encoding(queriesStringToParse));
	ret.path = parse_percent_encoding(ret.path);

	return ret;
}

struct HTTPRequest parse_request(char *request) {
	struct HTTPRequest ret;

	ret.method = strtok(request, " ");

//	request = strtok(request, " ");
//	ret.method = request;

	char *uriPathTmp;
//	request = strtok(request, " ");
//	uriPathTmp = request;

	request = strtok(NULL, " ");
	uriPathTmp = request;

	ret.uri = parse_uri(uriPathTmp);

	request = strtok(NULL, " ");
//	request = strtok(request, " ");
	ret.httpVersion = request;

	return ret;
}

#endif // HTTP_C
