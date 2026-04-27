#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* parameters */
    struct aws_string *src;

    /* allocate and assume bounds for src */
    if (nondet_bool()) {
        src = NULL;
    } else {
        src = malloc(sizeof(*src) + nondet_size_t());
        __CPROVER_assume(src != NULL);
        src->allocator = (struct aws_allocator *)nondet_ptr();
        src->len = nondet_size_t();
        __CPROVER_assume(AWS_MEM_IS_READABLE(src->bytes, src->len));
    }

    /* save old state */
    struct aws_string old_src = *src;

    /* operation under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* assertions */
    assert(aws_byte_cursor_is_valid(&cur));
    if (src == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == src->bytes);
        assert(cur.len == src->len);
    }

    /* unchanged fields */
    if (src != NULL) {
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        assert(AWS_MEM_IS_READABLE(src->bytes, src->len));
    }

    /* validity invariants */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
