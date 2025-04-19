#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strrplce(char *string, char *find, char *replace)
{
    size_t strilen = strlen(string),
           findlen = strlen(find),
           repllen = strlen(replace);

    char *findptrs[strilen];
    char *str, *strptr;
    char *stringptr = string;
    int cur = 0;
    char *prevptr = string;

    if (findlen > strilen) return string;
    if (findlen == strilen && !strcmp(find, string))
    {
        strncpy(string, replace, repllen);
        return string;
    }

    for (; (findptrs[cur] = strstr(stringptr, find)) != NULL; stringptr = (findptrs[cur++] + findlen)) {}

    strptr = str = malloc(strilen + cur * (repllen - findlen) + 1);
    if (!str)
    {
        perror("malloc");
        return NULL;
    }

    stringptr = string;

    for (int i = 0; i < cur; ++i)
    {
        int ignored = findptrs[i] - prevptr;

        strncpy(strptr, stringptr, ignored);
        
        strptr += ignored;

        strncpy(strptr, replace, repllen);

        strptr += repllen;

        prevptr = findptrs[i] + findlen;
        stringptr += (ignored + findlen);
    }

    strcpy(strptr, stringptr);

    if (repllen > findlen)
    {
        return str; // let the user free it
    }
    else
    {
        strcpy(string, str);
        free(str);
        return string;
    }
}


int main(void)
{
    char helloworld[] = "HELLO WOrlD HELLO WORLD HELLO WORD HELLO WORLD";
    char worldhello[] = "WORLD HElLO WORLD HELO WORLD HELLO HELLO WORLD";
    
    char *s = strrplce(helloworld, "WORLD", "hello world hello");
    puts(s);
    free(s);
    s = NULL;

    strrplce(worldhello, "HELLO", "Hi");
    puts(worldhello);
    
    return 0;
}
