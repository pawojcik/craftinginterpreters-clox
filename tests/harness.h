// Minimal self-registering test harness.
//
// Usage (in a single tests/*.c file):
//
//     #include "harness.h"
//
//     TEST(something_works) {
//         assert(1 + 1 == 2);
//     }
//
// Every TEST() registers itself before main() runs (via a constructor
// function), and this header provides main(), so test files never list
// their tests by hand. Each test runs in a forked child process, so a
// failing assert() or a crash is reported and the remaining tests still run.
//
// Run a subset by passing name substrings: ./build/test_scanner numbers

#ifndef clox_test_harness_h
#define clox_test_harness_h

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    const char* name;
    void (*fn)(void);
    int line;
} TestCase;

#define MAX_TESTS 512

static TestCase harness_tests[MAX_TESTS];
static int harness_count = 0;

static void harness_register(const char* name, void (*fn)(void), int line) {
    if (harness_count == MAX_TESTS) {
        fprintf(stderr, "harness: too many tests (max %d)\n", MAX_TESTS);
        exit(2);
    }
    harness_tests[harness_count++] = (TestCase){name, fn, line};
}

#define TEST(name)                                                    \
    static void name(void);                                           \
    __attribute__((constructor)) static void register_##name(void) {  \
        harness_register(#name, name, __LINE__);                      \
    }                                                                 \
    static void name(void)

// Constructor order isn't guaranteed, so run tests in source order.
static int harness_by_line(const void* a, const void* b) {
    return ((const TestCase*)a)->line - ((const TestCase*)b)->line;
}

static int harness_selected(const TestCase* t, int argc, char** argv) {
    if (argc < 2) return 1;
    for (int i = 1; i < argc; i++) {
        if (strstr(t->name, argv[i]) != NULL) return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    qsort(harness_tests, harness_count, sizeof(TestCase), harness_by_line);

    int passed = 0, failed = 0;
    for (int i = 0; i < harness_count; i++) {
        TestCase* t = &harness_tests[i];
        if (!harness_selected(t, argc, argv)) continue;

        fflush(stdout);
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 2;
        }
        if (pid == 0) {
            t->fn();
            fflush(stdout);
            _exit(0);
        }

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("  ✓ %s\n", t->name);
            passed++;
        } else if (WIFSIGNALED(status)) {
            printf("  ✗ %s (signal %d)\n", t->name, WTERMSIG(status));
            failed++;
        } else {
            printf("  ✗ %s (exit %d)\n", t->name, WEXITSTATUS(status));
            failed++;
        }
    }

    printf("  %d passed, %d failed\n", passed, failed);
    return failed == 0 ? 0 : 1;
}

#endif
