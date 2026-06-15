#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic helpers */
extern _Bool nondet_bool(void);
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

/* Define a reasonable maximum for the string length */
#define MAX_STR_LEN 256

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string (may be NULL) */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STR_LEN);
        __CPROVER_assume(len < SIZE_MAX); /* avoid overflow in malloc */

        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);   /* allocation must succeed for the harness */

        for (size_t i = 0; i < len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[len] = '\0';
        c_str = tmp;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */
    assert(buf.allocator == NULL);

    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}

int main(void) {
    aws_byte_buf_from_c_str_harness();
    return 0;
}
