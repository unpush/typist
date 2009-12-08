/*
    Original httpget Verions 2.2    1998-12-21   by T.Ogihara

    http.c   2007.08.24 Takeshi Ogihara
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "base.h"
#if defined(USE_WINSOCK)
# include <io.h>
# include <winsock2.h>
#else
# include <libc.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
#endif
#include <limits.h>
#include <sys/types.h>
#include "http.h"

#ifndef BUFSIZ
#define BUFSIZ 512
#endif

/************* USE_WINSOCK ************/
#if defined(USE_WINSOCK)
static WSADATA wsaData;
/*extern*/ FILEsIO file_io;

void init_sock(void) {
	WORD v = MAKEWORD(1, 0);
	if (WSAStartup(v, &wsaData) != 0) {
		perror("winsock");
		exit(1);
	}
}

/* Only one socket can be used at the same time.
   If you want to use 2 or more, add these vars to
   struct FILEsIO.
 */
static char iogets_buf[BUFSIZ];
static int iogets_idx = 0;
static int iogets_max = 0;

void final_sock(void) {
	WSACleanup();
}

char *iogets(char *str, int size, FILEsIO *stream)
{
	int sx, ch;

	if (stream->fp != NULL)
		return fgets(str, size, stream->fp);

	sx = 0;
	while (sx < size) {
		while (iogets_idx < iogets_max) {
			str[sx++] = ch = iogets_buf[iogets_idx++];
			if (ch == '\n') {
				str[sx] = 0;
				return str;
			}
			if (sx >= size)
				return str;
		}
		iogets_max = recv(stream->sock, iogets_buf, BUFSIZ, 0);
		if (iogets_max == 0 || iogets_max == SOCKET_ERROR) {
			iogets_max = 0;
			if (sx > 0) {
				str[sx] = 0;
				return str;
			}
			return NULL;
		}
		iogets_idx = 0;
	}
	return str;
}

int ioclose(FILEsIO *stream)
{
	if (stream->fp != NULL)
		return fclose(stream->fp);
	return closesocket(stream->sock);
}

FILEsIO *makeFilesio(FILE *t)
{
	if (t == NULL)
		return NULL;
	file_io.fp = t;
	return &file_io;
}

#endif
/********** End of USE_WINSOCK *********/

static int parse_url(const char* url, char* host, unsigned short* port, char* fstr)
{
	*port = 0;
	if (*url == 'h') {
		if(strncmp(url, "http://", 7) != 0)
			return 0;
		url += 7;
	}
	if(!*url)
		return 0;
	while(*url && *url != '/' &&  *url != ':')
		*host++ = *url++;
	*host = '\0';
	if(*url == ':') {
		url++;
		*port = atoi(url);
		while(*url && *url != '/')
			url++;
	}

	while(*url)
		*fstr++ = *url++;
	*fstr = '\0';
	return 1;
}

static int getaddrnumber(const char *host, char *buf)
{
	int	i, j, v, cc;

	for (i = 0, j = 0; ; i++) {
		v = 0;
		for (cc = host[j]; isdigit(cc); cc = host[++j])
			v = (v * 10) + (cc - '0');
		buf[i] = v;
		if (cc != '.')
			return i+1;
		j++;
	}
	return i;
}

static int connect_to_server(const char* hostname, unsigned short port)
{
	int fd;
	int err;
	struct sockaddr_in in;
	struct hostent *hp;
	char buf[16];

	memset((char *)&in, 0, sizeof(in));
	in.sin_family = AF_INET;
	in.sin_port   = htons(port);

	if (getaddrnumber(hostname, buf) == 4)
		memcpy(&in.sin_addr, buf, 4);
	else {
		if((hp = gethostbyname(hostname)) == NULL) {
			perror("gethostbyname");
			return -1;
		}
		memcpy(&in.sin_addr, hp->h_addr, hp->h_length);
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (illegalSockect(fd)) {
		perror("socket");
		return -1;
	}

	err = connect(fd, (struct sockaddr *)&in, sizeof(struct sockaddr));
	if (illegalSockectCall(err)) {
		perror("connect");
		return -1;
	}
	return fd;
}

static void send_command(int fd, const char *filename, const char *hostname)
{
	char req[BUFSIZ * 3];
	const char *agent = "Typist/3.0 T.O./2007";

	sprintf(req, "GET %s HTTP/1.0\r\nUser-Agent: %s\r\nHost: %s\r\n\r\n",
		filename, agent, hostname);
	send(fd, req, strlen(req), 0);
}

FILEsIO *open_url(const char *url)
{
	unsigned short port = 0;
	char hostname[BUFSIZ];
	char filename[BUFSIZ];
	char buff[BUFSIZ];
	FILEsIO *fp;
	int fd;

	if(parse_url(url, hostname, &port, filename) == 0) {
		fprintf(stderr, "Invalid URL:%s\n", url);
		return NULL;
	}
	if (port == 0)
		port = 80;
	if ((fd = connect_to_server(hostname, port)) < 0) {
		fprintf(stderr, "Cannot connect to:%s\n", url);
		return NULL;
	}
	send_command(fd, filename, hostname);

#if defined(USE_WINSOCK)
	fp = &file_io;
	fp->fp = NULL;
	fp->sock = fd;
	iogets_idx = iogets_max = 0;
#else
	fp = fdopen(fd, "r");
#endif
	do { /* read http header */
		if (iogets(buff, BUFSIZ, fp) == NULL) {
			fprintf(stderr, "No documentation.\n");
			ioclose(fp);
			return NULL;
		}
	} while (buff[0] != '\r' && buff[0] != '\n');
	return fp;
}

#ifdef __ALONE__
int main(int argc, char *argv[])
{
	int ac, c;
	FILE *fp;

	for (ac = 1; ac < argc; ac++) {
		printf("================ %s\n", argv[ac]);
		if ((fp = open_url(argv[ac])) != NULL) {
			while ((c = getc(fp)) != EOF)
				putchar(c);
		}
		fclose(fp);
	}
	return 0;
}
#endif
