#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness(void) {
    /* nondeterministic but valid aws_string */
    struct aws_string *src = make_aws_string();

    /* structural validity assumption */
    __CPROVER_assume(aws_string_is_valid(src));

    /* save pre‑state for frame condition checks */
    struct aws_allocator *pre_allocator = NULL;
    size_t pre_len = 0;

    if (src != NULL) {
        pre_allocator = src->allocator;
        pre_len = src->len;
    }

    /* call the function under verification */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* post‑condition 1: return value correctness */
    if (src == NULL) {
        /* empty cursor expected */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* cursor should point to the string's bytes */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
    }

    /* frame condition checks */
    if (src != NULL) {
        assert(src->allocator == pre_allocator);
        assert(src->len == pre_len);
    }
}
