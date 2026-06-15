#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_push_front_harness() {
    /* nondet input */
    struct aws_array_list list;
    size_t max_initial_item_allocation = MAX_INITIAL_ITEM_ALLOCATION;
    size_t max_item_size = MAX_ITEM_SIZE;

    /* ensure boundedness */
    __CPROVER_assume(aws_array_list_is_bounded(&list, &max_initial_item_allocation, &max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    /* val: nondet pointer to readable memory of correct size */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* save val contents for later assertion */
    struct store_byte_from_buffer val_storage;
    save_byte_from_array((uint8_t *)val, list.item_size, &val_storage);

    /* call function */
    int result = aws_array_list_push_front(&list, val);

    /* postconditions */
    assert(aws_array_list_is_valid(&list));
    if (result == AWS_OP_SUCCESS) {
        /* success: length increases by 1 */
        assert(list.length == old.length + 1);
        /* first element is the pushed value */
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)val, list.item_size);
        /* allocator and item_size unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* capacity may have grown, but must be at least old capacity */
        assert(list.current_size >= old.current_size);
    } else {
        /* failure: list unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, old.length * old.item_size);
        }
    }
}
