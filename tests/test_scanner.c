#include <assert.h>
// #include <stdio.h>

#include "../scanner.h"

void should_return_eof_for_empty_source() {
    initScanner("");
    assert(scanToken().type == TOKEN_EOF);
}

void should_handle_whitespace() {
    initScanner("  \t\n! \n\t\r");
    assert(scanToken().type == TOKEN_BANG);
    assert(scanToken().type == TOKEN_EOF);
}

void should_hadle_all_single_char_tokens() {
    initScanner("(){},.-+/*=!<>");
    assert(scanToken().type == TOKEN_LEFT_PAREN);
    assert(scanToken().type == TOKEN_RIGHT_PAREN);
    assert(scanToken().type == TOKEN_LEFT_BRACE);
    assert(scanToken().type == TOKEN_RIGHT_BRACE);
    assert(scanToken().type == TOKEN_COMMA);
    assert(scanToken().type == TOKEN_DOT);
    assert(scanToken().type == TOKEN_MINUS);
    assert(scanToken().type == TOKEN_PLUS);
    assert(scanToken().type == TOKEN_SLASH);
    assert(scanToken().type == TOKEN_STAR);
    assert(scanToken().type == TOKEN_EQUAL);
    assert(scanToken().type == TOKEN_BANG);
    assert(scanToken().type == TOKEN_LESS);
    assert(scanToken().type == TOKEN_GREATER);
}

void should_handle_all_double_tokens() {
    initScanner("!===<=>=");
    assert(scanToken().type == TOKEN_BANG_EQUAL);
    assert(scanToken().type == TOKEN_EQUAL_EQUAL);
    assert(scanToken().type == TOKEN_LESS_EQUAL);
    assert(scanToken().type == TOKEN_GREATER_EQUAL);
}

void should_handle_numbers() {
    initScanner("123 123.123");
    Token t = scanToken();
    assert(t.type == TOKEN_NUMBER);
    assert(t.length == 3);
    t = scanToken();
    assert(t.type == TOKEN_NUMBER);
    assert(t.length == 7);
}

int main() {
    should_return_eof_for_empty_source();
    should_hadle_all_single_char_tokens();
    should_handle_all_double_tokens();
    should_handle_whitespace();
    should_handle_numbers();
    return 0;
}

