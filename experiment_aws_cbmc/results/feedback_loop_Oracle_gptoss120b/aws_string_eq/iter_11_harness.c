#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/allocator.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 16

void aws_string_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct {
        struct aws_string base;
        uint8_t bytes[MAX_LEN];
    } str_a_storage;
    struct aws_string *str_a = NULL;

    struct {
        struct aws_string base;
        uint8_t bytes[MAX_LEN];
    } str_b_storage;
    struct aws_string *str_b = NULL;

    if (__CPROVER_nondet_uint()) {
        size_t len_a = (size_t)__CPROVER_nondet_uint();
        __CPROVER_assume(len_a <= MAX_LEN);
        str_a = &str_a_storage.base;
        str_a->allocator = allocator;
        str_a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            str_a->bytes[i] = __CPROVER_nondet_uchar();
        }
    }

    if (__CPROVER_nondet_uint()) {
        size_t len_b = (size_t)__CPROVER_nondet_uint();
        __CPROVER_assume(len_b <= MAX_LEN);
        str_b = &str_b_storage.base;
        str_b->allocator = allocator;
        str_b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            str_b->bytes[i] = __CPROVER_nondet_uchar();
        }
    }

    __CPROVER_assume(!str_a || aws_string_is_valid(str_a));
    __CPROVER_assume(!str_b || aws_string_is_valid(str_b));

    size_t len_a_before = 0, len_b_before = 0;
    uint8_t *bytes_a_before = NULL, *bytes_b_before = NULL;

    if (str_a) {
        len_a_before = str_a->len;
        if (len_a_before) {
            bytes_a_before = malloc(len_a_before);
            __CPROVER_assume(bytes_a_before != NULL);
            memcpy(bytes_a_before, str_a->bytes, len_a_before);
        }
    }
    if (str_b) {
        len_b_before = str_b->len;
        if (len_b_before) {
            bytes_b_before = malloc(len_b_before);
            __CPROVER_assume(bytes_b_before != NULL);
            memcpy(bytes_b_before, str_b->bytes, len_b_before);
        }
    }

    bool result = aws_string_eq(str_a, str_b);

    if (str_a == str_b) {
        assert(result == true);
    } else if (str_a == NULL || str_b == NULL) {
        assert(result == false);
    } else {
        bool expected = (str_a->len == str_b->len) &&
                        (memcmp(str_a->bytes, str_b->bytes, str_a->len) == 0);
        assert(result == expected);
    }

    if (str_a) {
        assert(str_a->len == len_a_before);
        if (len_a_before) {
            assert(memcmp(str_a->bytes, bytes_a_before, len_a_before) == 0);
        }
    }
    if (str_b) {
        assert(str_b->len == len_b_before);
        if (len_b_before) {
            assert(memcmp(str_b->bytes, bytes_b_before, len_b_before) == 0);
        }
    }

    free(bytes_a_before);
    free(bytes_b_before);
}
