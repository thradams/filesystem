#include "fs.h"

#ifdef _WIN32
#include <windows.h>
#include "Shlwapi.h"
#include <string.h>
#include <assert.h>

#pragma comment(lib, "Shlwapi.lib")

void fs_path_split(const char* Path,
    char* Drive,
    char* Directory,
    char* Filename,
    char* Extension)
{
    _splitpath(Path, Drive, Directory, Filename, Extension);
}

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
    bool b = CreateDirectoryA(path, NULL);
    if (!b)
    {
        ec->code = GetLastError();
        switch (ec->code)
        {
        case ERROR_ALREADY_EXISTS:
            b = true;
            break;
        case ERROR_PATH_NOT_FOUND:
            break;
        }

    }
    return b;
}


bool fs_copy_file(const char* pathfrom,
    const char* pathto,
    struct error_code* ec)
{
    bool b = CopyFileA(
        pathfrom,
        pathto,
        0 /*bFailIfExists*/
    ) != 0;

    if (!b)
    {
        ec->code = GetLastError();
    }
    return b;
}

bool directory_iterator_init(struct directory_iterator* di, const char* path, struct error_code* ec)
{
    WIN32_FIND_DATAA fdFile;

    char sPath[MAX_PATH] = { 0 };
    strcat(sPath, path);
    strcat(sPath, "\\*.*");

    di->handle = FindFirstFileA(sPath, &fdFile);

    bool b = di->handle != INVALID_HANDLE_VALUE;
    if (b)
    {
        di->bIsDir = fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY;
        strcpy(di->fileName, fdFile.cFileName);
    }
    else
    {
        ec->code = GetLastError();
    }
    return b;
}

bool directory_iterator_next(struct directory_iterator* di)
{
    WIN32_FIND_DATAA fdFile;
    bool b = FindNextFileA(di->handle, &fdFile);
    if (b)
    {
        di->bIsDir = fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY;
        strcpy(di->fileName, fdFile.cFileName);
    }
    else
    {

    }
    return b;
}

void directory_iterator_destroy(struct directory_iterator* di)
{
    FindClose((HANDLE)di->handle);
}
bool fs_current_path(char* pathOut)
{
    /*
    If the function succeeds, the return value specifies the number
    of characters that are written to the buffer, not including the
    terminating null character.
    If the function fails, the return value is zero.To get extended error
    information, call GetLastError.
    If the buffer that is pointed to by lpBuffer is not large enough,
    the return value specifies the required size of the buffer,
    in characters, including the null - terminating character.
    */
    DWORD r = GetCurrentDirectoryA(MAX_PATH, pathOut);
    if (r == 0)
    {
        //GetLastError();
    }
    return r != 0;
}

#else
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

bool fs_create_directory(const char* path, struct error_code* ec)
{
    //http://pubs.opengroup.org/onlinepubs/009695399/functions/mkdir.html
    //http://www.gnu.org/software/coreutils/mkdir
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool fs_current_path(char* pathOut)
{
    return getcwd(pathOut, 256);
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


bool directory_iterator_init(struct directory_iterator* di, const char* path, struct error_code* ec)
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



void fs_path_split(const char* Path,
    char* Drive,
    char* Directory,
    char* Filename,
    char* Extension)
{
    char* CopyOfPath = (char*)Path;
    int Counter = 0;
    int Last = 0;
    int Rest = 0;

    // no drives available in linux .
    // extensions are not common in linux
    // but considered anyway
    Drive = NULL;

    while (*CopyOfPath != '\0')
    {
        // search for the last slash
        while (*CopyOfPath != '/' && *CopyOfPath != '\0')
        {
            CopyOfPath++;
            Counter++;
        }
        if (*CopyOfPath == '/')
        {
            CopyOfPath++;
            Counter++;
            Last = Counter;
        }
        else
            Rest = Counter - Last;
    }
    // directory is the first part of the path until the
    // last slash appears
    strncpy(Directory, Path, Last);
    // strncpy doesnt add a '\0'
    Directory[Last] = '\0';
    // Filename is the part behind the last slahs
    strcpy(Filename, CopyOfPath -= Rest);
    // get extension if there is any
    while (*Filename != '\0')
    {
        // the part behind the point is called extension in windows systems
        // at least that is what i thought apperantly the '.' is used as part
        // of the extension too .
        if (*Filename == '.')
        {
            while (*Filename != '\0')
            {
                *Extension = *Filename;
                Extension++;
                Filename++;
            }
        }
        if (*Filename != '\0')
        {
            Filename++;
        }
    }
    *Extension = '\0';
    return;
}

#endif




//https://en.cppreference.com/w/cpp/experimental/fs/copy
bool  fs_copy(const char* source, const char* dest, struct error_code* ec)
{
    assert(ec->code == 0);


    struct directory_iterator di;
    if (directory_iterator_init(&di, source, ec) &&
        fs_create_directory(dest, ec))
    {
        do
        {
            if (di.bIsDir)
            {
                if (strcmp(di.fileName, ".") != 0 &&
                    strcmp(di.fileName, "..") != 0)
                {
                    char origFile[FS_MAX_PATH] = { 0 };
                    strcpy(origFile, source);
                    strcat(origFile, "/");
                    strcat(origFile, di.fileName);

                    char destFile[FS_MAX_PATH] = { 0 };
                    strcpy(destFile, dest);
                    strcat(destFile, "/");
                    strcat(destFile, di.fileName);


                    if (!fs_copy(origFile, destFile, ec))
                    {
                        break;
                    }
                }
            }
            else
            {
                char origFile[FS_MAX_PATH] = { 0 };
                strcpy(origFile, source);
                strcat(origFile, "/");
                strcat(origFile, di.fileName);


                char destFile[FS_MAX_PATH] = { 0 };
                strcpy(destFile, dest);
                strcat(destFile, "/");
                strcat(destFile, di.fileName);

                if (!fs_copy_file(origFile, destFile, ec))
                {
                    break;
                }
            }

        } while (directory_iterator_next(&di));

        directory_iterator_destroy(&di);
    }


    return ec->code == 0;
}
