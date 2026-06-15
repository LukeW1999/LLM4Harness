#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* nondet list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet index */
    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    /* nondet val pointer: must be readable if non-null */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    save_byte_from_array(val, list.item_size, &storage);

    /* call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* length increased if index >= old.length, else unchanged */
        size_t expected_length;
        if (index >= old.length) {
            __CPROVER_assume(aws_add_size_checked(index, 1, &expected_length) == AWS_OP_SUCCESS);
        } else {
            expected_length = old.length;
        }
        assert(list.length == expected_length);

        /* element at index matches val */
        size_t offset;
        if (aws_mul_size_checked(index, list.item_size, &offset) == AWS_OP_SUCCESS) {
            uint8_t *dst = (uint8_t *)list.data + offset;
            assert_byte_from_buffer_matches(dst, &storage);
        }

        /* capacity sufficient */
        size_t required_size;
        if (aws_mul_size_checked(list.length, list.item_size, &required_size) == AWS_OP_SUCCESS) {
            assert(list.current_size >= required_size);
        }

        /* data non-null if size > 0 */
        if (list.current_size > 0) {
            assert(list.data != NULL);
        }
    } else {
        /* on failure, list unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
    }

    /* invariants */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(aws_array_list_is_valid(&list));
}
