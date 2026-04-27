#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdarg.h>

void aws_byte_buf_cat_harness() {
    /* data structure */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* parameters */
    size_t number_of_args;
    __CPROVER_assume(number_of_args > 0 && number_of_args <= MAX_NUMBER_OF_ARGS);

    /* create and initialize the array of aws_byte_buf pointers */
    struct aws_byte_buf *args[number_of_args];
    for (size_t i = 0; i < number_of_args; ++i) {
        args[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(args[i] != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(args[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(args[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(args[i]));
    }

    /* save old state of dest before calling */
    struct aws_byte_buf old_dest = dest;

    /* call the function under test */
    va_list ap;
    va_start(ap, number_of_args);
    for (size_t i = 0; i < number_of_args; ++i) {
        va_arg(ap, struct aws_byte_buf *);
    }
    va_end(ap);

    va_start(ap, number_of_args);
    int result = aws_byte_buf_cat(&dest, number_of_args, ap);
    va_end(ap);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            expected_len += args[i]->len;
        }
        assert(dest.len == expected_len);
        assert(dest.capacity >= expected_len);
        for (size_t i = 0; i < number_of_args; ++i) {
            assert(AWS_MEM_IS_READABLE(args[i]->buffer, args[i]->len));
        }
    } else {
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.allocator == old_dest.allocator);
    }

    /* unchanged fields */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity >= old_dest.capacity); /* capacity can only increase or stay the same */
    assert(dest.buffer == old_dest.buffer); /* buffer pointer should not change */

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < number_of_args; ++i) {
        assert(aws_byte_buf_is_valid(args[i]));
        free(args[i]);
    }
}
