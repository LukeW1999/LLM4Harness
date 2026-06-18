#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 64

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;
    list.data = NULL;

    /* nondet initialize fields */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    list.current_size = list.length * list.item_size;

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    struct store_byte_from_buffer old_byte;
    if (list.length > 0 && list.item_size > 0) {
        size_t old_buf_len = list.length * list.item_size;
        size_t idx = nondet_size_t();
        __CPROVER_assume(idx < old_buf_len);
        save_byte_from_array((uint8_t *)list.data + idx, 1, &old_byte);
    }

    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    (void)aws_array_list_pop_front_n(&list, n);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }

    if (list.length > 0 && list.item_size > 0) {
        if (old.length > n) {
            size_t max_valid_idx = (old.length - n) * old.item_size;
            size_t idx = old_byte.buffer_offset;
            __CPROVER_assume(idx < max_valid_idx);
            assert_byte_from_buffer_matches((uint8_t *)list.data + idx, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
