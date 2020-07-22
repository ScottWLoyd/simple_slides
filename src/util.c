#include "util.h"

#include <assert.h>

#ifdef PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#endif

ReadFileResult read_file(const char* path) {
#ifdef PLATFORM_WINDOWS
    ReadFileResult result = { 0 };

    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (INVALID_HANDLE_VALUE == file) {
        DWORD error = GetLastError();
        char buf[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, error, 0, buf, 256, 0);
        printf("Failed to open file '%s'. Error: %s\n", path, buf);
        assert(0);
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file, &file_size)) {
        printf("Failed to determine file size '%s'\n", path);
        assert(0);
    }
    char* buffer = (char*)malloc(file_size.QuadPart);

    unsigned long bytes_read;
    ReadFile(file, buffer, (DWORD)file_size.QuadPart, &bytes_read, 0);
    if (bytes_read != file_size.QuadPart) {
        printf("Read the wrong number of bytes\n");
        assert(0);
    }

    result.data_len = bytes_read;
    result.data = buffer;
    result.success = 1;

    return result;
#else
    ALBA_ASSERT(false, "not currently implemented for non-windows systems");
#endif
}
