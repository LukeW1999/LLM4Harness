#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
void *nondet_pointer(void);

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src = nondet_pointer();

    /* Snapshot variables for frame condition */
    size_t pre_len = 0;
    uint8_t pre_bytes[1024];

    if (src) {
        /* Allocate a plausible aws_string with flexible array member on the stack */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024U);               /* bound for tractability */

        struct {
            struct aws_string s;
            uint8_t bytes[1024];
        } wrapper;

        src = &wrapper.s;
        src->len = len;
        src->allocator = aws_default_allocator();     /* may be NULL */

        /* Initialize the byte contents nondeterministically */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(src);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* Record snapshot for frame check */
        pre_len = src->len;
        for (size_t i = 0; i < len; ++i) {
            pre_bytes[i] = bytes[i];
        }
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* Postcondition: validity */
    if (src == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == aws_string_bytes(src));
        assert(cur.len == src->len);
    }

    /* Frame condition: src must remain unchanged */
    if (src) {
        assert(src->len == pre_len);
        uint8_t *bytes = (uint8_t *)aws_string_bytes(src);
        for (size_t i = 0; i < pre_len; ++i) {
            assert(bytes[i] == pre_bytes[i]);
        }
    }
}
