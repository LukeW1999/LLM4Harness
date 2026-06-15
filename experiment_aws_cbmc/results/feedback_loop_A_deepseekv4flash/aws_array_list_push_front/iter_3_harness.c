#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_push_front_harness() {
    /* nondet input */
    struct aws_array_list list;
    size_t item_size;
    size_t initial_capacity;
    bool is_valid;

    /* nondet parameters */
    item_size = nondet_size_t();
    initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity <= MAX_CAPACITY);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* initialize list with nondet state */
    __CPROVER_assume(aws_array_list_is_bounded(&list, &initial_capacity, &item_size));
    /* allocate data if length > 0 */
    if (list.length > 0) {
        list.data = bounded_malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
        list.current_size = 0;
    }
    list.alloc = nondet_bool() ? NULL : nondet_allocator(); /* allow nondet allocator */
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
