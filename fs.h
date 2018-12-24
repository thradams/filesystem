#pragma once

#include <stdbool.h>

struct error_code
{
    unsigned int code;
};

//See https://en.cppreference.com/w/cpp/filesystem

bool fs_create_directory(const char* path, struct error_code* ec);
bool fs_copy_file(const char* pathfrom, const char* pathto, struct error_code* ec);
bool fs_exists(const char* path, struct error_code* ec);
bool fs_remove(const char* path, struct error_code* ec);


struct directory_iterator
{
    void* handle;
    char fileName[256];
    bool bIsDir;
};

bool directory_iterator_init(struct directory_iterator* di, const char* path);
bool directory_iterator_next(struct directory_iterator* di);
void directory_iterator_destroy(struct directory_iterator* di);
