#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string (may be NULL) */
    const char *c_str;
    size_t nondet_len = 0;

    if (nondet_bool()) {
        c_str = NULL;
    } else {
        nondet_len = nondet_size_t();
        __CPROVER_assume(nondet_len > 0);                     /* avoid zero‑length edge case */
        __CPROVER_assume(nondet_len <= MAX_BUFFER_SIZE);

        struct aws_allocator *alloc = aws_default_allocator();
        uint8_t *tmp = aws_mem_acquire(alloc, nondet_len + 1);
        __CPROVER_assume(tmp != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(tmp, nondet_len + 1));

        /* Fill with non‑zero bytes to avoid early NULs */
        for (size_t i = 0; i < nondet_len; ++i) {
            uint8_t byte = nondet_uint8();
            __CPROVER_assume(byte != 0);
            tmp[i] = byte;
        }
        /* Ensure a single terminating NUL at the end */
        tmp[nondet_len] = '\0';

        c_str = (const char *)tmp;
    }

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition assertions */
    /* allocator is always NULL */
    assert(buf.allocator == NULL);

    /* capacity must equal len */
    assert(buf.capacity == buf.len);

    if (c_str == NULL) {
        /* when input is NULL, buffer is empty and pointer is NULL */
        assert(buf.len == 0);
        assert(buf.buffer == NULL);
    } else {
        /* when input is non‑NULL, len equals the string length we created */
        assert(buf.len == nondet_len);
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
