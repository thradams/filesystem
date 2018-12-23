#include <stdio.h>

//To debug linux
//Run WSL
//sudo service ssh start
//More details at https://www.hanselman.com/blog/WritingAndDebuggingLinuxCApplicationsFromVisualStudioUsingTheWindowsSubsystemForLinux.aspx

//to run inside WSL
// cd /mnt/c/Users/thiago/source/repos/WindowsLinux2/bin/x64/Debug
// ./Linux.out

#include "fs.h"

int main()
{
    //fs_create_directories("sandbox/1/2/a");
    if (fs_create_directory("sandbox"))
    {
        printf("OK");
    }
    else
    {
        printf("FAILED");
    }
    //fs::permissions("sandbox/1/2/b", fs::perms::remove_perms | fs::perms::others_all);
    //fs::create_directory("sandbox/1/2/c", "sandbox/1/2/b");
    //std::system("ls -l sandbox/1/2");
    //fs::remove_all("sandbox");

    return 0;
}