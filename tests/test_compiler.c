#include "harness.h"

#include "../chunk.h"
#include "../compiler.h"

// Compiles `source` and asserts the emitted bytecode equals `expected`.
static void assertCompiles(const char* source, const uint8_t* expected, int count) {
    Chunk chunk;
    initChunk(&chunk);
    assert(compile(source, &chunk));
    assert(chunk.count == count);
    for (int i = 0; i < count; i++) {
        assert(chunk.code[i] == expected[i]);
    }
    freeChunk(&chunk);
}

#define ASSERT_COMPILES(source, ...)                                  \
    do {                                                              \
        uint8_t expected[] = {__VA_ARGS__};                           \
        assertCompiles(source, expected, (int)sizeof(expected));      \
    } while (0)

static void assertCompileError(const char* source) {
    Chunk chunk;
    initChunk(&chunk);
    assert(!compile(source, &chunk));
    freeChunk(&chunk);
}

TEST(should_compile_number_literal) {
    Chunk chunk;
    initChunk(&chunk);
    assert(compile("42.5", &chunk));
    assert(chunk.count == 3);
    assert(chunk.code[0] == OP_CONSTANT);
    assert(chunk.constants.values[chunk.code[1]] == 42.5);
    assert(chunk.code[2] == OP_RETURN);
    freeChunk(&chunk);
}

TEST(should_compile_binary_operators) {
    ASSERT_COMPILES("1 + 2", OP_CONSTANT, 0, OP_CONSTANT, 1, OP_ADD, OP_RETURN);
    ASSERT_COMPILES("1 - 2", OP_CONSTANT, 0, OP_CONSTANT, 1, OP_SUBTRACT, OP_RETURN);
    ASSERT_COMPILES("1 * 2", OP_CONSTANT, 0, OP_CONSTANT, 1, OP_MULTIPLY, OP_RETURN);
    ASSERT_COMPILES("1 / 2", OP_CONSTANT, 0, OP_CONSTANT, 1, OP_DIVIDE, OP_RETURN);
}

TEST(should_bind_factor_tighter_than_term) {
    // 1 + (2 * 3)
    ASSERT_COMPILES("1 + 2 * 3",
        OP_CONSTANT, 0, OP_CONSTANT, 1, OP_CONSTANT, 2,
        OP_MULTIPLY, OP_ADD, OP_RETURN);
}

TEST(should_be_left_associative) {
    // (1 - 2) - 3
    ASSERT_COMPILES("1 - 2 - 3",
        OP_CONSTANT, 0, OP_CONSTANT, 1, OP_SUBTRACT,
        OP_CONSTANT, 2, OP_SUBTRACT, OP_RETURN);
}

TEST(should_respect_grouping) {
    ASSERT_COMPILES("(1 + 2) * 3",
        OP_CONSTANT, 0, OP_CONSTANT, 1, OP_ADD,
        OP_CONSTANT, 2, OP_MULTIPLY, OP_RETURN);
}

TEST(should_compile_unary_negation) {
    ASSERT_COMPILES("-1", OP_CONSTANT, 0, OP_NEGATE, OP_RETURN);
    // (-1) * 2, not -(1 * 2)
    ASSERT_COMPILES("-1 * 2",
        OP_CONSTANT, 0, OP_NEGATE, OP_CONSTANT, 1, OP_MULTIPLY, OP_RETURN);
}

TEST(should_reject_empty_source) {
    assertCompileError("");
}

TEST(should_reject_unclosed_group) {
    assertCompileError("(1 + 2");
}

TEST(should_reject_missing_operand) {
    assertCompileError("1 +");
}

TEST(should_reject_trailing_tokens) {
    assertCompileError("1 2");
}
