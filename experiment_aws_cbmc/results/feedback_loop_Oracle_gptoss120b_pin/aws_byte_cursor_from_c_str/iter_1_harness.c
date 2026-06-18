#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/byte_order.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 20

void aws_byte_cursor_from_c_str_harness(void) {
    /* nondeterministic input string (may be NULL) */
    char *c_str;
    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_unsigned_int();
        __CPROVER_assume(len < MAX_STRING_LEN);
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = __CPROVER_nondet_char();
        }
        c_str[len] = '\0';
    } else {
        c_str = NULL;
    }

    /* make a copy of the input string to check frame condition */
    char *c_str_copy = NULL;
    size_t copy_len = 0;
    if (c_str != NULL) {
        copy_len = strlen(c_str);
        c_str_copy = malloc(copy_len + 1);
        __CPROVER_assume(c_str_copy != NULL);
        memcpy(c_str_copy, c_str, copy_len + 1);
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* postcondition: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cur));

    /* postcondition: pointer and length correspond to the input string */
    if (c_str != NULL) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* frame condition: input string memory must be unchanged */
    if (c_str != NULL) {
        assert(memcmp(c_str, c_str_copy, copy_len + 1) == 0);
    }

    return 0;
}
