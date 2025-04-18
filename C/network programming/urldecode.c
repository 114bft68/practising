#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ishex(x) ((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))

int decode(char *url)
{
    char *pos = strchr(url, '%');
    if (!pos) return 0;

    size_t len = strlen(url),
           i   = pos - url,
           cur = i;

    char substr[strlen(url) + 1];

    strncpy(substr, url, cur);

    while (i < len)
    {
        if (i + 2 < len && url[i] == '%' && ishex(url[i + 1]) && ishex(url[i + 2]))
        {
            char hex[] = { url[i + 1], url[i + 2], '\0' };
            long val   = strtol(hex, NULL, 16);                          // error is only possible if the number is out of range, but that's impossible in this case
            
            substr[cur++] = val;
            i += 3;

            continue;
        }
        
        substr[cur++] = url[i++];
    }

    substr[cur] = '\0';
    strncpy(url, substr, cur + 1);

    return 0;
}

int main(void)
{
    return 0;
}
