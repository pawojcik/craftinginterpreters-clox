#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include "codes.h"

static void repl() {
    char line[1024];
    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(CLOX_CANNOT_READ_SOURCE_FILE);
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // extra byte for string terminator
    char* buffer = (char*) malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(CLOX_CANNOT_READ_SOURCE_FILE);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read fie \"%s\".\n", path);
        exit(CLOX_CANNOT_READ_SOURCE_FILE);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {
    char* source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) {
        exit(CLOX_COMPILE_ERROR);
    }
    if (result == INTERPRET_RUNTIME_ERROR) {
        exit(CLOX_RUNTIME_ERROR);
    }
}

int main(int argc, const char* argv[]) {
    initVM();
    
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]");
        exit(CLOX_CANNOT_READ_SOURCE_FILE);
    }

    freeVM();
    return 0;
}