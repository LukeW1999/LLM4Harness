/* CBMC harness for aws_string_eq_c_str */

#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STR_LEN 64
#define MAX_CSTR_LEN 64

/* Bounded nondeterministic strlen */
static size_t cbmc_strlen(const char *s) {
    size_t i = 0;
    while (i < MAX_CSTR_LEN && s[i] != '\0') {
        i++;
    }
    __CPROVER_assume(i < MAX_CSTR_LEN);
    return i;
}

/* Expected equality between aws_string bytes and C string */
static bool expected_eq(const uint8_t *bytes, size_t len, const char *c_str) {
    size_t c_len = cbmc_strlen(c_str);
    if (len != c_len) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        if (bytes[i] != (uint8_t)c_str[i]) {
            return false;
        }
    }
    return true;
}

/* Allocate a nondeterministic aws_string (or NULL) */
static struct aws_string *make_nondet_string(void) {
    struct aws_string *s = NULL;
    if (__CPROVER_nondet_bool()) {
        size_t len = (size_t)__CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_STR_LEN);
        size_t total = sizeof(struct aws_string) + (len + 1) * sizeof(uint8_t);
        s = (struct aws_string *)malloc(total);
        __CPROVER_assume(s != NULL);
        s->allocator = aws_default_allocator();
        s->len = len;
        for (size_t i = 0; i < len; ++i) {
            s->bytes[i] = (uint8_t)__CPROVER_nondet_uchar();
        }
        s->bytes[len] = (uint8_t)'\0';
    }
    return s;
}

/* Allocate a nondeterministic C string (or NULL) */
static char *make_nondet_c_str(void) {
    char *c = NULL;
    if (__CPROVER_nondet_bool()) {
        size_t len = (size_t)__CPROVER_nondet_uint();
        __CPROVER_assume(len < MAX_CSTR_LEN);
        c = (char *)malloc(len + 1);
        __CPROVER_assume(c != NULL);
        for (size_t i = 0; i < len; ++i) {
            c[i] = (char)__CPROVER_nondet_uchar();
        }
        c[len] = '\0';
    }
    return c;
}

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str = make_nondet_string();
    char *c_str = make_nondet_c_str();

    /* Snapshot for frame condition */
    struct aws_allocator *alloc_snapshot = NULL;
    size_t len_snapshot = 0;
    uint8_t *bytes_snapshot = NULL;

    if (str != NULL) {
        alloc_snapshot = str->allocator;
        len_snapshot = str->len;
        bytes_snapshot = (uint8_t *)malloc(len_snapshot);
        __CPROVER_assume(bytes_snapshot != NULL);
        for (size_t i = 0; i < len_snapshot; ++i) {
            bytes_snapshot[i] = str->bytes[i];
        }
    }

    char *c_str_snapshot = NULL;
    size_t c_str_len = 0;
    if (c_str != NULL) {
        c_str_len = cbmc_strlen(c_str);
        c_str_snapshot = (char *)malloc(c_str_len + 1);
        __CPROVER_assume(c_str_snapshot != NULL);
        for (size_t i = 0; i <= c_str_len; ++i) {
            c_str_snapshot[i] = c_str[i];
        }
    }

    bool result = aws_string_eq_c_str(str, c_str);

    /* Postcondition */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = expected_eq(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* Frame condition for str */
    if (str != NULL) {
        assert(str->allocator == alloc_snapshot);
        assert(str->len == len_snapshot);
        for (size_t i = 0; i < len_snapshot; ++i) {
            assert(str->bytes[i] == bytes_snapshot[i]);
        }
    }

    /* Frame condition for c_str */
    if (c_str != NULL) {
        for (size_t i = 0; i <= c_str_len; ++i) {
            assert(c_str[i] == c_str_snapshot[i]);
        }
    }
}
