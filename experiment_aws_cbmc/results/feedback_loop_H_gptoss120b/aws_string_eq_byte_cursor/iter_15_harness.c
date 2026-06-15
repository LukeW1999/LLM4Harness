#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/* CBMC nondeterministic helpers */
bool nondet_bool(void);
size_t nondet_size_t(void);

/* AWS types and prototypes (provided elsewhere) */
struct aws_allocator;
struct aws_string {
    struct aws_allocator *allocator;
    size_t len;
    const uint8_t *bytes;
};
struct aws_byte_cursor {
    const uint8_t *ptr;
    size_t len;
};

struct aws_allocator *aws_default_allocator(void);
bool aws_string_eq_byte_cursor(const struct aws_string *str,
                               const struct aws_byte_cursor *cur);
bool aws_string_is_valid(const struct aws_string *str);
bool aws_byte_cursor_is_valid(const struct aws_byte_cursor *cur);
bool aws_byte_cursor_is_bounded(const struct aws_byte_cursor *cur,
                                size_t max_len);

/* Harness */
void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondet allocation of aws_string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        static uint8_t buf[256];
        static struct aws_string s;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= sizeof(buf));
        struct aws_allocator *alloc = aws_default_allocator();
        __CPROVER_assume(alloc != NULL);
        s.allocator = alloc;
        s.len = len;
        s.bytes = buf;
        str = &s;
    }

    /* nondet allocation of aws_byte_cursor */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        static uint8_t buf2[256];
        static struct aws_byte_cursor c;
        size_t len2 = nondet_size_t();
        __CPROVER_assume(len2 <= sizeof(buf2));
        c.ptr = buf2;
        c.len = len2;
        cur = &c;
    }

    /* Assume preconditions required by the function under test */
    __CPROVER_assume(!str || aws_string_is_valid(str));
    __CPROVER_assume(!cur || aws_byte_cursor_is_valid(cur));

    /* Preserve old values for later checks */
    struct aws_string old_str;
    if (str) {
        old_str = *str;
    }
    struct aws_byte_cursor old_cur;
    if (cur) {
        old_cur = *cur;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Compute expected result according to the reference semantics */
    bool expected;
    if (!str || !cur) {
        expected = false;
    } else {
        expected = (str->len == cur->len) &&
                   (memcmp(str->bytes, cur->ptr, str->len) == 0);
    }

    /* Post‑condition checks */
    assert(result == expected);

    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
    }
    if (cur) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
    }
}

/* Entry point for CBMC */
int main(void) {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
