#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t index = nondet_size_t();

    size_t item_sz = list.item_size;
    uint8_t *src = (uint8_t *)aws_mem_acquire(allocator, item_sz);
    __CPROVER_assume(src != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        src[i] = nondet_uint8_t();
    }

    int result = aws_array_list_set_at(&list, src, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match(
            (const uint8_t *)list.data + (list.item_size * index),
            src,
            list.item_size);

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.current_size >= old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        if (list.data != NULL && old.current_size > 0) {
            assert_bytes_match(
                (const uint8_t *)list.data,
                (const uint8_t *)old.data,
                old.current_size);
        }

        assert(result == AWS_OP_ERR);
    }

    assert(aws_array_list_is_valid(&list));
}
