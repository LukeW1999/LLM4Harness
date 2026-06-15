#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* nondet inputs */
    struct aws_array_list list;
    size_t index;
    void *val;

    /* assume valid list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* assume val is readable for item_size bytes */
    __CPROVER_assume(val != NULL && AWS_MEM_IS_READABLE(val, list.item_size));

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* call function */
    int result = aws_array_list_set_at(&list, val, index);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* success: element at index is set */
        size_t new_len;
        if (aws_add_size_checked(index, 1, &new_len) == AWS_OP_SUCCESS) {
            /* if no overflow, length should be max(old.length, index+1) */
            if (index >= old.length) {
                assert(list.length == new_len);
            } else {
                assert(list.length == old.length);
            }
        }
        /* data at index should equal val */
        uint8_t *elem_ptr = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(elem_ptr, (const uint8_t *)val, list.item_size);
        /* fields that can change: current_size, data ptr (if realloc) */
        /* fields that must not change: alloc, item_size */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* failure: list unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* validity invariant must hold on both paths */
    assert(aws_array_list_is_valid(&list));
}
