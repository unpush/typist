/*
    http.h   2007.08.24 Takeshi Ogihara
 */

#ifndef  __DEF_HTTP__
#define  __DEF_HTTP__

#if defined(USE_WINSOCK)

typedef struct {
	FILE	*fp;
	int	sock;
} FILEsIO;
extern FILEsIO file_io;

# define illegalSockect(t)       ((t) == INVALID_SOCKET)
# define illegalSockectCall(t)   ((t) == SOCKET_ERROR)
void init_sock(void);
void final_sock(void);
char *iogets(char *str, int size, FILEsIO *stream);
int ioclose(FILEsIO *stream);
FILEsIO *makeFilesio(FILE *t);

#else
/* Unix like Socket */

typedef FILE	FILEsIO;

# define illegalSockect(t)       ((t) < 0)
# define illegalSockectCall(t)   ((t) < 0)
# define init_sock()	/* none */
# define final_sock()	/* none */
# define iogets		fgets
# define ioclose	fclose
# define makeFilesio(t)	(t)
#endif /* no USE_WINSOCK */

FILEsIO *open_url(const char *url);

#endif /* __DEF_HTTP__ */
