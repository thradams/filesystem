#include "fs.h"

#ifdef _WIN32

bool fs_create_directory(const char* path)
{
    //http://www.gnu.org/software/coreutils/mkdir
    return _mkdir(path) == 0;
}


#else
#include <sys/stat.h>

bool fs_create_directory(const char* path)
{
    //http://pubs.opengroup.org/onlinepubs/009695399/functions/mkdir.html
    //http://www.gnu.org/software/coreutils/mkdir
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

#endif

