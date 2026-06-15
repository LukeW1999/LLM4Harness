#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    /* Non-deterministic data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic indices */
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Call function */
    aws_array_list_swap(&list, a, b);

    /* Postconditions */
    /* Fields unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* Elements at a and b are swapped */
    size_t item_size = list.item_size;
    uint8_t *ptr_old_a = (uint8_t *)old_list.data + a * item_size;
    uint8_t *ptr_old_b = (uint8_t *)old_list.data + b * item_size;
    uint8_t *ptr_new_a = (uint8_t *)list.data + a * item_size;
    uint8_t *ptr_new_b = (uint8_t *)list.data + b * item_size;
    assert_bytes_match(ptr_new_a, ptr_old_b, item_size);
    assert_bytes_match(ptr_new_b, ptr_old_a, item_size);

    /* List remains valid */
    assert(aws_array_list_is_valid(&list));
}
