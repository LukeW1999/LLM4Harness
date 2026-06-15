#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* nondet helpers provided by CBMC */
extern size_t nondet_size_t(void);
extern char nondet_char(void);
extern bool nondet_bool(void);

void aws_byte_cursor_from_c_str_harness(void) {
    /* -------------------------------------------------------------
       Create a nondeterministic C string (may be NULL) with a bounded
       length and a guaranteed null‑terminator somewhere within the
       allocated buffer.
       ------------------------------------------------------------- */
    size_t max_len = nondet_size_t();
    __CPROVER_assume(max_len < 256);               /* bound allocation size */

    char *c_str = malloc(max_len + 1);
    __CPROVER_assume(c_str != NULL);               /* allocation must succeed */

    /* fill the buffer with nondeterministic characters */
    for (size_t i = 0; i < max_len; ++i) {
        c_str[i] = nondet_char();
    }

    /* ensure there is at least one null terminator within the buffer */
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos <= max_len);
    c_str[null_pos] = '\0';
    /* also terminate the buffer at the very end for safety */
    c_str[max_len] = '\0';

    /* -------------------------------------------------------------
       Snapshot the input memory to check frame conditions later.
       ------------------------------------------------------------- */
    char *c_str_snapshot = malloc(max_len + 1);
    __CPROVER_assume(c_str_snapshot != NULL);
    memcpy(c_str_snapshot, c_str, max_len + 1);

    /* -------------------------------------------------------------
       Call the function under verification.
       ------------------------------------------------------------- */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* -------------------------------------------------------------
       Post‑condition checks.
       ------------------------------------------------------------- */
    /* 1. Validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 2. Length / pointer invariants */
    if (c_str != NULL) {
        size_t expected_len = strlen(c_str);
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == expected_len);
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* 3. Frame condition: input string memory unchanged */
    for (size_t i = 0; i < max_len + 1; ++i) {
        assert(c_str[i] == c_str_snapshot[i]);
    }

    /* clean up */
    free(c_str);
    free(c_str_snapshot);
    return 0;
}
