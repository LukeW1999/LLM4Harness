#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.item_size > 0);

    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save structural fields before swap */
    void *old_data_ptr = list.data;
    size_t old_current_size = list.current_size;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Save copies of the items at positions a and b */
    uint8_t *item_a_copy = malloc(list.item_size);
    uint8_t *item_b_copy = malloc(list.item_size);
    __CPROVER_assume(item_a_copy != NULL);
    __CPROVER_assume(item_b_copy != NULL);

    uint8_t *base = (uint8_t *)list.data;
    memcpy(item_a_copy, base + a * list.item_size, list.item_size);
    memcpy(item_b_copy, base + b * list.item_size, list.item_size);

    /* Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* Structural invariants must hold */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data_ptr);

    /* Verify the swap: item at position a is now what was at b, and vice versa */
    uint8_t *new_base = (uint8_t *)list.data;

    /* Check that item at position b now matches what was at position a */
    assert_bytes_match(new_base + b * list.item_size, item_a_copy, list.item_size);

    /* Check that item at position a now matches what was at position b */
    assert_bytes_match(new_base + a * list.item_size, item_b_copy, list.item_size);
}
