#include <stdio.h>

//To debug linux
//Run WSL
//sudo service ssh start
//More details at https://www.hanselman.com/blog/WritingAndDebuggingLinuxCApplicationsFromVisualStudioUsingTheWindowsSubsystemForLinux.aspx

//to run inside WSL
// cd /mnt/c/Users/thiago/source/repos/WindowsLinux2/bin/x64/Debug
// ./Linux.out

#include "fs.h"

void Test1()
{
        struct error_code ec = { 0 };
    if (fs_exists("sandbox", &ec))
    {
        printf("exist\n");
    }
    else
    {
        printf("not exist\n");
    }


    if (fs_create_directory("sandbox", &ec))
    {
        printf("OK\n");
    }
    else
    {
        printf("FAILED\n");
    }

    if (fs_exists("sandbox", &ec))
    {
        printf("exist\n");
    }
    else
    {
        printf("not exist\n");
    }

    fs_remove("sandbox",& ec);

    if (fs_exists("sandbox", &ec))
    {
        printf("exist\n");
    }
    else
    {
        printf("not exist\n");
    }
}

int main()
{
    Test1();
    return 0;
}