#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

/* Helper to allocate a buffer of at least `size` bytes */
static uint8_t *allocate_bytes(size_t size) {
    if (size == 0) {
        return NULL;
    }
    return (uint8_t *)malloc(size);
}

/* Helper to create a nondeterministic aws_string (or NULL) */
static struct aws_string *make_nondet_string(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= SIZE_MAX - sizeof(struct aws_string));
    struct aws_string *s = NULL;
    if (nondet_bool()) {
        s = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(s != NULL);
        s->allocator = aws_default_allocator();
        s->len = len;
        if (len > 0) {
            uint8_t *bytes = (uint8_t *)s->bytes;
            for (size_t i = 0; i < len; ++i) {
                bytes[i] = nondet_uint8_t();
            }
        }
    }
    return s;
}

/* Helper to create a nondeterministic aws_byte_cursor (or NULL) */
static struct aws_byte_cursor *make_nondet_cursor(void) {
    struct aws_byte_cursor *c = NULL;
    if (nondet_bool()) {
        c = (struct aws_byte_cursor *)malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(c != NULL);
        c->len = nondet_size_t();
        c->ptr = allocate_bytes(c->len);
        __CPROVER_assume(c->len == 0 || c->ptr != NULL);
        if (c->len > 0) {
            for (size_t i = 0; i < c->len; ++i) {
                ((uint8_t *)c->ptr)[i] = nondet_uint8_t();
            }
        }
    }
    return c;
}

/* Compute the expected result according to the specification */
static bool expected_eq(const struct aws_string *str, const struct aws_byte_cursor *cur) {
    if (str == NULL && cur == NULL) {
        return true;
    }
    if (str == NULL || cur == NULL) {
        return false;
    }
    if (str->len != cur->len) {
        return false;
    }
    if (str->len == 0) {
        return true;
    }
    return (memcmp(str->bytes, cur->ptr, str->len) == 0);
}

/* Snapshot helpers */
static void snapshot_string(const struct aws_string *src, struct aws_string **dst) {
    if (src == NULL) {
        *dst = NULL;
        return;
    }
    size_t total = sizeof(struct aws_string) + src->len;
    *dst = (struct aws_string *)malloc(total);
    __CPROVER_assume(*dst != NULL);
    memcpy(*dst, src, total);
}

static void snapshot_cursor(const struct aws_byte_cursor *src, struct aws_byte_cursor **dst) {
    if (src == NULL) {
        *dst = NULL;
        return;
    }
    *dst = (struct aws_byte_cursor *)malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(*dst != NULL);
    memcpy(*dst, src, sizeof(struct aws_byte_cursor));
    if (src->len > 0) {
        uint8_t *buf = allocate_bytes(src->len);
        __CPROVER_assume(buf != NULL);
        memcpy(buf, src->ptr, src->len);
        (*dst)->ptr = buf;
    } else {
        (*dst)->ptr = NULL;
    }
}

void aws_string_eq_byte_cursor_harness(void) {
    /* Arrange */
    struct aws_string *str = make_nondet_string();
    struct aws_byte_cursor *cur = make_nondet_cursor();

    /* Take snapshots of the inputs */
    struct aws_string *str_snapshot = NULL;
    struct aws_byte_cursor *cur_snapshot = NULL;
    snapshot_string(str, &str_snapshot);
    snapshot_cursor(cur, &cur_snapshot);

    /* Act */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Assert postconditions */
    bool expected = expected_eq(str, cur);
    assert(result == expected);

    /* Assert frame conditions */
    if (str_snapshot != NULL) {
        size_t total = sizeof(struct aws_string) + str_snapshot->len;
        assert(memcmp(str, str_snapshot, total) == 0);
    } else {
        assert(str == NULL);
    }

    if (cur_snapshot != NULL) {
        assert(cur->len == cur_snapshot->len);
        if (cur->len > 0) {
            assert(memcmp(cur->ptr, cur_snapshot->ptr, cur->len) == 0);
        } else {
            assert(cur->ptr == cur_snapshot->ptr);
        }
    } else {
        assert(cur == NULL);
    }

    /* Clean up */
    if (str != NULL) {
        free((void *)str);
    }
    if (cur != NULL) {
        free((void *)cur->ptr);
        free(cur);
    }
    if (str_snapshot != NULL) {
        free(str_snapshot);
    }
    if (cur_snapshot != NULL) {
        free((void *)cur_snapshot->ptr);
        free(cur_snapshot);
    }
}
