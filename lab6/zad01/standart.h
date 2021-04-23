#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

const char *HOME;

if ((HOME = getenv("HOME")) == NULL)
{
    HOME = getpwuid(getuid())->pw_dir;
}