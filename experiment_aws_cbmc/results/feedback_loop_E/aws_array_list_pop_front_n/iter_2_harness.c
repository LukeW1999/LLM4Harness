#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_n_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    size_t n;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(n <= list.length); // Ensure n does not exceed the length of the list

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* perform operation under verification */
    aws_array_list_pop_front_n(&list, n);

    /* assertions */
    if (n >= old.length) {
        assert(list.length == 0);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        size_t popping_bytes = old.item_size * n;
        size_t remaining_items = old.length - n;
        size_t remaining_bytes = remaining_items * old.item_size;
        assert(list.length == remaining_items);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data + popping_bytes, remaining_bytes);
#ifdef DEBUG_BUILD
        assert_bytes_constant((uint8_t *)list.data + remaining_bytes, AWS_ARRAY_LIST_DEBUG_FILL, popping_bytes);
#endif
    }

    /* unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
    assert_byte_from_buffer_matches((uint8_t *)old.data, &old_byte);
}
