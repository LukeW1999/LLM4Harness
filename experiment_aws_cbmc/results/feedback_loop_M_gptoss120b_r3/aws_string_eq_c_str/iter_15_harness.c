#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/error.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str = NULL;
    const char *c_str = NULL;

    /* Allocate storage for aws_string on the stack */
    uint8_t str_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string *tmp = (struct aws_string *)str_storage;
        struct aws_allocator *allocator = aws_default_allocator();
        tmp->allocator = allocator;
        tmp->len = len;

        for (size_t i = 0; i < len; ++i) {
            tmp->bytes[i] = nondet_uint8_t();
        }

        str = tmp;
    }

    /* Buffer for C string on the stack */
    char c_buf[MAX_BUFFER_SIZE + 1];

    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
        for (size_t i = 0; i < c_len; ++i) {
            c_buf[i] = (char)nondet_uint8_t();
        }
        c_buf[c_len] = '\0';
        c_str = c_buf;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == false);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }
}
