#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Bound the list and ensure data member is allocated if current_size > 0 */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Assume val is a readable pointer of size item_size */
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL) {
        save_byte_from_array((uint8_t *)old.data, old.current_size, &old_byte);
    }

    /* Ensure both success and failure paths are reachable: index is nondet */
    __CPROVER_assume(index < old.length || index >= old.length);

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, index must be < old.length */
        assert(index < old.length);
        /* The element at index must equal val */
        uint8_t *element = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(element, (const uint8_t *)val, list.item_size);
        /* Length remains the same */
        assert(list.length == old.length);
        /* Other fields unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure, the list must be completely unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* Unchanged fields regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
