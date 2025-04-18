#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <stdarg.h>

#include <sys/types.h>  // sockaddr_in, AF_INET... types
#include <sys/socket.h> // socket()...
#include <netinet/in.h> // bind()...
#include <arpa/inet.h>  // htons()...
#include <unistd.h>     // close()...

#define quit(finish, string, ...) { perror("\n" string); __VA_ARGS__; finish; }
#define MAX_LISTEN       20
#define MAX_READ         1048576                                         // 1,048,576 CHARACTERS


#define ERROR500_1_1     "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 25\r\nContent-Type: text/plain\r\n\r\n500 Internal Server Error"
#define ERROR404_1_1     "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\n404 Not Found"
#define ERROR500_1_0     "HTTP/1.0 500 Internal Server Error\r\nContent-Length: 25\r\nContent-Type: text/plain\r\n\r\n500 Internal Server Error"
#define ERROR404_1_0     "HTTP/1.0 404 Not Found\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\n404 Not Found"
#define MESSAGE_200      "HTTP/%s 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\nContent-Disposition: inline\r\nCache-Control: no-store\r\n\r\n"
#define MESSAGE_200_FS   7                                               // 7 characters occupied by the format specifiers


// #define REG_ERROR_LENGTH 100
#define REG_NO_CNVRT     0
#define REG_TO_LOWER     64
#define REG_TO_UPPER     96
#define REG_YES_FULL     0
#define REG_NO_FULL      1

int reg(const char *string, const char *regex, int casecnvrt, int nofull, int varamount, ...);
/* 
   ... format: char *variable, int maxLength, char *variable, int maxLength...................
                     variable stores at most maxLength - 1 characters. If the length of the matched group exceeds maxLength - 1,
                         the first maxLength - 1 characters of the matched group are copied to variable, and 2 is returned
                     1 is returned if regcomp fails or no match is found
*/


#define ishex(x) ((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))

void decode(char *url);


const char *mime2_5_6[][2];
const char     *mime3[][2];
const char     *mime4[][2];

const char *getMime(char *path);


int intlen(int x);


int main(void)
{    
    struct sockaddr_in saddr = {
        .sin_family         = AF_INET,                                   // uses IPv4
        .sin_port           = htons(8080),                               // host byte order (8080) to network byte order
        .sin_addr.s_addr    = INADDR_ANY                                 // sin_addr is an in_addr struct and contains s_addr
    };

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);                      // socket file descriptor (-1 is returned on error)
    if (socketfd == -1) quit(return 1, "server socket")

    int optval = 1;                                                      // option value (true in this case)
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, (socklen_t) sizeof(optval)) == -1)
        printf("\nsetsockopt: %s\nThe socket cannot be reused\n", strerror(errno));

    if (bind(socketfd, (const struct sockaddr*) &saddr, sizeof(saddr))) // assigns an address to the socket based on the info from the struct sockaddr_in, returns -1 on error
        quit(return 1, "bind", close(socketfd))

    if (listen(socketfd, MAX_LISTEN))                                    // accepts requests
        quit(return 1, "listen", close(socketfd))

    puts("\nListening on http://localhost:8080\nCtrl + C to quit\n");

    for (;;)
    {
        int clientSocketFD = -1;
        struct sockaddr_in caddr;
        memset(&caddr, 0, sizeof(caddr));
        socklen_t sizeofcaddr = (socklen_t) sizeof(caddr);               // a restrict pointer is required for the third parameter of accept()

        if ((clientSocketFD = accept(socketfd, (struct sockaddr *) &caddr, &sizeofcaddr)) == -1)
            quit(continue, "accept", puts("Try again"))

        int forkN = fork();                                              // creates a child process and the parent skips the loop and waits for another client
        if (forkN == -1) quit(continue, "fork", close(clientSocketFD));  // if fork() fails (forkN > 0), close the client socket and enter a new iteration
        if (forkN > 0) continue;                                         // parent processes are filtered out

        close(socketfd);                                                 // closes the server sockets inside child processes

        while (1)                                                        // THE CHILD PROCESSES GET STUCK IN THE WHILE LOOP
        {
            char readbuf[MAX_READ + 1] = { 0 };
            int  readlen = read(clientSocketFD, readbuf, MAX_READ);      // the length of the read data
            
            char path[1024]      = { 0 };                                // 1023 characters should be sufficient to store the path [GET (/...) HTTP/1.1]
            char httpVersion[4]  = { 0 };                                // HTTP_VERSION should only be 1.0 or 1.1, 3 + \0 = 4
            char connection[256] = { 0 };                                // Connection can contain multiple values, 255 is enough to hold all of them (perhaps)

            if (readlen == -1)                                           // the length of the read data
            {
                quit(continue, "read", puts("Try again"))
            }
            else if (readlen == 0)                                       // end of file (lost connection)
            {
                close(clientSocketFD);
                return 0;                                                // ends the child process
            }                                                            // if either of them is true, the current iteration (or the whole loop) would be terminated

            readbuf[readlen] = '\0';
            puts(readbuf);

            if (reg(readbuf, "GET (\\/.*) HTTP\\/([0-1]\\.[0-1])", REG_NO_CNVRT, REG_NO_FULL, 2, path, 1024, httpVersion, 4))
            {
                puts("\nResquest rejected (Request method not being GET or HTTP version not being 1.0/ 1.1)");
                                                                         // it can also be an unexpected error
                close(clientSocketFD);
                return 1;                                                // ends the child process
            }
            decode(path);                                                // decode the path

            const char *mimeType = getMime(path);

            FILE *file = fopen(path, "rb");
            if (!file)
                quit(return 1, "fopen",
                     write(clientSocketFD, !strcmp(connection, "1.1") ? ERROR404_1_1 : ERROR404_1_0, strlen(ERROR404_1_1)),
                     close(clientSocketFD))

            long   size = 0;
            char  *message;
            size_t msglen = strlen(MESSAGE_200) - MESSAGE_200_FS + 3 + /*intlen(size)*/ + strlen(mimeType);
            // message's length - chars occupied by format speificers + version (3 chars) + length of mime type, intlen(size) is added later

            if (fseek(file, 0, SEEK_END)                              ||
                (size = ftell(file)) == -1L                           ||
                fseek(file, 0, SEEK_SET)                              ||

                !(message = malloc(size + (msglen += intlen(size)))))    // intlen(size) IS ADDED TO MSGLEN
            {                                                            // memory allocation for fileContent or message may have failed
                free(message);                                           // free() checks if it's NULL before freeing
                
                fclose(file);
                
                write(clientSocketFD, !strcmp(connection, "1.1") ? ERROR500_1_1 : ERROR500_1_0,
                      strlen(ERROR500_1_1));                             // no retrying even if write fails (intended)
                
                close(clientSocketFD);

                quit(return 1, "fseek, ftell, malloc");
            }
            
            sprintf(message, MESSAGE_200, httpVersion, size, mimeType);

            if (fread(message + msglen, 1, size, file) != size && ferror(file)) 
                quit(return 1, "fread", free(message));

            fclose(file);                                                // 

            if (write(clientSocketFD, message, size + msglen) == -1)     // same as send() without flags
                quit(continue, "write", free(message), puts("Try again"))
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages/

            free(message);
            message = NULL;

            if ((!strcmp(httpVersion, "1.0") &&
                reg(readbuf, "Connection: ?(.*)\\\r\\\n", REG_TO_LOWER, REG_NO_FULL, 1, connection, 256)) ||
                strstr(connection, "close"))
            {
                close(clientSocketFD);
                return 0;
            }
        }
    }
}

int reg(const char *string, const char *regex, int casecnvrt, int nofull, int varamount, ...)
/*
    1. the string to extract from
    2. regex
    3. to uppercase or lowercase
    4. capture the whole group or not
    5. the amount of variables provided in ...
        (if nofull, varamount does not count the variable which stores the full capture group, ... does not contain the variable to store full either. vice versa)
    6. the sizes of the variables in ... are expected to be sufficient to store the substring and a null terminator
*/
{
    int totalGroups = varamount + (nofull ? 1 : 0);
    regex_t    r;                                                        // the compiled regex
    regmatch_t regcapture[totalGroups];                                  // capture totalGroups groups

    if (regcomp(&r, regex, REG_EXTENDED)) return 1;
    if (regexec(&r, string, totalGroups, regcapture, 0))                 // REG_NOMATCH or unexpected errors only (wrong regexes can also cause errors (I tested my regexes and they are syntatically correct))
    {
        regfree(&r);
        return 1;
    }

    regfree(&r);

    int status = 0;
    va_list dests;
    va_start(dests, varamount);

    for (int i = nofull; i < varamount + nofull; ++i)
    {
        char *des = va_arg(dests, char*);
        int   len = va_arg(dests, int) - 1;                              // length - 1 (reserved for \0)

        regoff_t end = regcapture[i].rm_eo - regcapture[i].rm_so;
        if (end > len)
        {
            end = len;
            status = 2;                                                  // the matched group is cut
        }

        strncpy(des, string + regcapture[i].rm_so, end);
        
        if (des[end - 1] != '\0') des[end] = '\0';

        if (!casecnvrt) continue;

        for (int j = 0; des[j] != '\0'; ++j)
        {
            if (des[j] > casecnvrt && des[j] < casecnvrt + 27) des[j] += -2 * casecnvrt + 160;
            /*
                casecnvrt should be either REG_NO_CNVRT, REG_TO_LOWER, or REG_TO_UPPER
                the equation of straight lines is applied in dest[j] += ...
            */
        }
    }

    va_end(dests);

    return status;
}

void decode(char *url)
{
    char *pos = strchr(url, '%');
    if (!pos) return;

    size_t len = strlen(url),
           cur = pos - url;

    char substr[strlen(url) + 1];

    strncpy(substr, url, cur);

    while (pos < url + len)
    {
        if (pos + 2 < url + len && *pos == '%' && ishex(*(pos + 1)) && ishex(*(pos + 2)))
        {
            char hex[] = { *(pos + 1), *(pos + 2), '\0' };
            long val   = strtol(hex, NULL, 16);                          // error is only possible if the number is out of range, but that's impossible in this case
            
            substr[cur++] = val;
            pos += 3;

            continue;
        }
        
        substr[cur++] = *(pos++);
    }

    substr[cur] = '\0';
    strncpy(url, substr, cur + 1);
}

const char *mime2_5_6[][2] = {
    { "js"    , "text/javascript"                                                           },
    { "xhtml" , "application/xhtml+xml"                                                     },
    { "jsonld", "application/ld+json"                                                       },
    { "woff2" , "font/woff2"                                                                },
    { NULL    , NULL                                                                        }
};

const char *mime3[][2] = {
    { "txt"   , "text/plain"                                                                },
    { "htm"   , "text/html"                                                                 },
    { "css"   , "text/css"                                                                  },
    { "mjs"   , "text/javascript"                                                           },
    { "csv"   , "text/csv"                                                                  },

    { "svg"   , "image/svg+xml"                                                             },
    { "png"   , "image/png"                                                                 },
    { "jpg"   , "image/jpeg"                                                                },
    { "gif"   , "image/gif"                                                                 },
    { "ico"   , "image/vnd.microsoft.icon"                                                  },
    { "bmp"   , "image/bmp"                                                                 },
    { "tif"   , "image/tiff"                                                                },

    { "mp4"   , "video/mp4"                                                                 },
    { "avi"   , "video/x-msvideo"                                                           },
    { "ogv"   , "video/ogg"                                                                 },
    { "3gp"   , "video/3gpp"                                                                },
    { "3g2"   , "video/3gpp2"                                                               },

    { "mp3"   , "audio/mpeg"                                                                },
    { "wav"   , "audio/wav"                                                                 },
    { "mid"   , "audio/midi"                                                                },
    { "oga"   , "audio/ogg"                                                                 },
    { "aac"   , "audio/aac"                                                                 },

    { "ttf"   , "font/ttf"                                                                  },
    { "otf"   , "font/otf"                                                                  },
    { "eot"   , "application/vnd.ms-fontobject"                                             },
    
    { "pdf"   , "application/pdf"                                                           },
    { "doc"   , "application/msword"                                                        },
    { "xls"   , "application/vnd.ms-excel"                                                  },
    { "xml"   , "application/xml"                                                           },
    { "php"   , "application/x-httpd-php"                                                   },
    { "ppt"   , "application/vnd.ms-powerpoint"                                             },
    { "bin"   , "application/octet-stream"                                                  },
    { NULL    , NULL                                                                        }
};

const char *mime4[][2] = {
    { "html"  , "text/html"                                                                 },

    { "webp"  , "image/webp"                                                                },
    { "apng"  , "image/apng"                                                                },
    { "jpeg"  , "image/jpeg"                                                                },
    { "avif"  , "image/avif"                                                                },
    { "tiff"  , "image/tiff"                                                                },

    { "webm"  , "video/webm"                                                                },
    { "mpeg"  , "video/mpeg"                                                                },

    { "midi"  , "audio/midi"                                                                },
    { "weba"  , "audio/webm"                                                                },
    { "opus"  , "audio/ogg"                                                                 },

    { "woff"  , "font/woff"                                                                 },
    
    { "json"  , "application/json"                                                          },
    { "docx"  , "application/vnd.openxmlformats-officedocument.wordprocessingml.document"   },
    { "xlsx"  , "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"         },
    { "pptx"  , "application/vnd.openxmlformats-officedocument.presentationml.presentation" },
    { NULL    , NULL                                                                        }
};

const char *getMime(char *path)
{ 
    char *dotpos = strrchr(path, '.');                                   // typical path: /a/b/c/d/source.c
    if (!dotpos) return "application/octet-stream";

    int extlen = strlen(++dotpos);                                       // extension length
    const char *(*array)[2] = NULL;

    array = ((extlen == 2 || extlen == 5 || extlen == 6) ? mime2_5_6 : extlen == 3 ? mime3 : extlen == 4 ? mime4 : NULL);

    if (array)
    {
        size_t i = 0;

        while (array[i][0] != NULL)
        {
            for (int j = 0; j < extlen; ++j)
            {
                char curchar = *(dotpos + j);

                if (((curchar > 64 && curchar < 91) ? curchar + 32 : curchar) != array[i][0][j]) break;

                if (j == extlen - 1) return array[i][1];
            }

            ++i;
        }
    }

    return "null";                                                       // stolen from a response sent by NPM http-server
}

int intlen(int x)
{
    char y[11] = { 0 };
    return sprintf(y, "%d", x);
}