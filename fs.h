#pragma once

#include <stdbool.h>

#include "error_code.h"

//See https://en.cppreference.com/w/cpp/filesystem

bool fs_create_directory(const char* path, struct error_code* ec);
bool fs_copy_file(const char* pathfrom, const char* pathto, struct error_code* ec);
bool fs_exists(const char* path, struct error_code* ec);
bool fs_remove(const char* path, struct error_code* ec);