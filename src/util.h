#ifndef UTIL_H
#define UTIL_H

typedef struct ReadFileResult {
    int success;
    size_t data_len;
    unsigned char* data;
} ReadFileResult;

ReadFileResult read_file(const char* path);

#endif
