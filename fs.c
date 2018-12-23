#include "fs.h"

#ifdef _WIN32
#include <windows.h>
#include "Shlwapi.h"
#include <string.h>

#pragma comment(lib, "Shlwapi.lib")

bool fs_remove(const char* path, struct error_code* ec)
{
    return RemoveDirectoryA(path);
}

bool fs_exists(const char* path, struct error_code* ec)
{
    return PathFileExistsA(path);
}

bool fs_create_directory(const char* path, struct error_code* ec)
{
    if (!CreateDirectoryA(path, NULL))
    {
        //GetLastError();
        return false;
    }
    return true;
}


bool fs_copy_file(const char* pathfrom,
    const char* pathto,
    struct error_code* ec)
{
    return CopyFileA(
        pathfrom,
        pathto,
        0 /*bFailIfExists*/
    );
}

bool directory_iterator_init(struct directory_iterator* di, const char* path)
{
    WIN32_FIND_DATAA fdFile;

    char sPath[MAX_PATH] = { 0 };
    strcat(sPath, path);
    strcat(sPath, "\\*.*");

    di->handle = FindFirstFileA(sPath, &fdFile);

    di->bIsDir = fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY;
    strcpy(di->fileName, fdFile.cFileName);
    return di->handle != INVALID_HANDLE_VALUE;
}

bool directory_iterator_next(struct directory_iterator* di)
{
    WIN32_FIND_DATAA fdFile;
    bool b = FindNextFileA(di->handle, &fdFile);
    di->bIsDir = fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY;
    strcpy(di->fileName, fdFile.cFileName);
    return b;
}

void directory_iterator_destroy(struct directory_iterator* di)
{
    FindClose((HANDLE)di->handle);
}


#else
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
bool fs_create_directory(const char* path, struct error_code* ec)
{
    //http://pubs.opengroup.org/onlinepubs/009695399/functions/mkdir.html
    //http://www.gnu.org/software/coreutils/mkdir
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool fs_copy_file(const char* pathfrom,
    const char* pathto,
    struct error_code* ec)
{

    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(pathfrom, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(pathto, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do
        {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return true;
    }

out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return false;
}

bool fs_exists(const char* path, struct error_code* ec)
{
    struct stat   buffer;
    return (stat(path, &buffer) == 0);
}

bool fs_remove(const char* path, struct error_code* ec)
{
    return rmdir(path);
}


bool directory_iterator_init(struct directory_iterator* di, const char* path)
{
    di->handle = opendir(path);
    if (di->handle)
    {
        directory_iterator_next(di);
    }
    return di->handle != 0;
}

bool directory_iterator_next(struct directory_iterator* di)
{

    struct dirent *dir = readdir((DIR*)di->handle);
    if (dir != NULL)
    {
        di->bIsDir = dir->d_type == 4;
        strcpy(di->fileName, dir->d_name);
    }
    return dir != NULL;
}

void directory_iterator_destroy(struct directory_iterator* di)
{
    closedir((DIR*)di->handle);
}

#endif

