#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t old_length = aws_array_list_length(&list);
    size_t old_item_size = list.item_size;
    size_t old_capacity = list.current_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Save a copy of the entire allocated data region before the call */
    uint8_t old_data_copy[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    memcpy(old_data_copy, list.data, old_capacity * old_item_size);

    /* Prepare the value to push (guaranteed readable by construction) */
    uint8_t val_buffer[MAX_ITEM_SIZE];
    const void *val = val_buffer;

    int result = aws_array_list_push_front(&list, val);

    /* Validity invariant must hold regardless of outcome */
    assert(aws_array_list_is_valid(&list));

    /* These fields never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Length increases by one */
        assert(list.length == old_length + 1);
        /* Capacity must be sufficient */
        assert(list.current_size >= list.length);

        /* The first element equals the pushed value */
        uint8_t first_elem[MAX_ITEM_SIZE];
        int get_err = aws_array_list_get_at(&list, first_elem, 0);
        assert(get_err == AWS_OP_SUCCESS);
        assert(memcmp(first_elem, val, list.item_size) == 0);

        /* All previous elements are shifted right by one position */
        for (size_t i = 1; i <= old_length; i++) {
            uint8_t elem[MAX_ITEM_SIZE];
            get_err = aws_array_list_get_at(&list, elem, i);
            assert(get_err == AWS_OP_SUCCESS);
            const uint8_t *old_elem = &old_data_copy[(i - 1) * old_item_size];
            assert(memcmp(elem, old_elem, old_item_size) == 0);
        }
    } else {
        /* On failure the list must remain completely unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_capacity);
        assert(list.data == old_data);
        assert(memcmp(list.data, old_data_copy, old_length * old_item_size) == 0);
    }
}
