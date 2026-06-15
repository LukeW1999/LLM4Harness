#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_from_string_harness(void) {
    /* nondeterministic but valid aws_string */
    struct aws_string *src = make_aws_string();

    /* structural validity assumption */
    __CPROVER_assume(aws_string_is_valid(src));

    /* save pre‑state for frame condition checks */
    struct aws_allocator *pre_allocator = NULL;
    size_t pre_len = 0;
    uint8_t *pre_bytes = NULL;

    if (src != NULL) {
        pre_allocator = src->allocator;
        pre_len = src->len;
        if (pre_len > 0) {
            pre_bytes = (uint8_t *)malloc(pre_len);
            __CPROVER_assume(pre_bytes != NULL);
            memcpy(pre_bytes, aws_string_bytes(src), pre_len);
        }
    }

    /* call the function under verification */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* post‑condition 1: return value correctness */
    if (src == NULL) {
        /* empty cursor expected */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }
