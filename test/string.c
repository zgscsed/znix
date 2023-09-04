#include <znix/types.h>
#include <znix/string.h>

char message[] = "hello znix!!!!";
char buf[1024];


int main()
{
    int res;
    res = strcmp(buf, message);
    strcpy(buf, message);
    res = strcmp(buf, message);
    strcat(buf, message);
    res = strcmp(buf, message);
    res = strlen(message);
    res = sizeof(message);

    char *ptr = strchr(message, '!');
    ptr = strrchr(message, '!');

    memset(buf, 0, sizeof(buf));
    res = memcmp(buf, message, sizeof(message));
    memcpy(buf, message, sizeof(message));
    res = memcmp(buf, message, sizeof(message));
    ptr = memchr(buf, '!', sizeof(message));
    return 0;
}