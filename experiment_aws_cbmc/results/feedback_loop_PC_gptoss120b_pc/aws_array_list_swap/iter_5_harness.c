#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length > 0);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *old_elem_a = aws_mem_acquire(allocator, list.item_size);
    uint8_t *old_elem_b = aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(old_elem_a != NULL);
    __CPROVER_assume(old_elem_b != NULL);
    memcpy(old_elem_a,
           (uint8_t *)old.data + a * old.item_size,
           old.item_size);
    memcpy(old_elem_b,
           (uint8_t *)old.data + b * old.item_size,
           old.item_size);

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    for (size_t i = 0; i < list.length; ++i) {
        uint8_t *cur = (uint8_t *)list.data + i * list.item_size;
        uint8_t *old_cur = (uint8_t *)old.data + i * old.item_size;
        if (i == a && i == b) {
            assert_bytes_match(cur, old_cur, list.item_size);
        } else if (i == a) {
            assert_bytes_match(cur, old_elem_b, list.item_size);
        } else if (i == b) {
            assert_bytes_match(cur, old_elem_a, list.item_size);
        } else {
            assert_bytes_match(cur, old_cur, list.item_size);
        }
    }

    assert(aws_array_list_is_valid(&list));

    aws_mem_release(allocator, old_elem_a);
    aws_mem_release(allocator, old_elem_b);
}
