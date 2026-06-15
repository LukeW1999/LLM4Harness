#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    uint8_t *val;

    /* Ensure the list has a positive current_size so that data is non-null */
    __CPROVER_assume(list.current_size > 0);
    /* Ensure item_size is positive to avoid trivial issues */
    __CPROVER_assume(list.item_size > 0);

    /* Bound the list to avoid state explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate a buffer for val that is readable for item_size bytes */
    val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state for comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (index < old.length) {
        save_byte_from_array((uint8_t *)old.data + (index * old.item_size), old.item_size, &old_byte);
    }

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index must match val */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size), val, list.item_size);
        /* Length must be at least index + 1 */
        assert(list.length >= index + 1);
        if (index >= old.length) {
            /* Length increased to index + 1 */
            assert(list.length == index + 1);
        } else {
            /* Length unchanged */
            assert(list.length == old.length);
        }
        /* Fields that must not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* current_size may change due to reallocation, so we do not assert it */
    } else {
        /* On failure, the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (index < old.length) {
            assert_byte_from_buffer_matches((uint8_t *)list.data + (index * list.item_size), &old_byte);
        }
    }

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
