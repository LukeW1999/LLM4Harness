#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* Allocate and initialize an allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Create a nondeterministic array list */
    struct aws_array_list list;
    /* Nondeterministic item size (must be > 0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    /* Nondeterministic initial capacity (may be zero) */
    size_t initial_capacity;
    __CPROVER_assume(initial_capacity <= 64); /* bound for tractability */
    /* Initialize list in dynamic mode */
    int init_result = aws_array_list_init_dynamic(&list, alloc, initial_capacity, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    /* Ensure the list is valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondeterministically set the length to a value within capacity */
    size_t length;
    __CPROVER_assume(length <= list.current_size);
    list.length = length;

    /* Prepare the output pointer variable */
    void *old_val = NULL;
    void *val = old_val; /* initial value */
    void **val_ptr = &val;

    /* Nondeterministic index */
    size_t index;
    /* No assumption on index; it can be any size_t */

    /* Save a snapshot of the list's memory for later comparison */
    struct aws_array_list list_snapshot = list;
    void *data_snapshot = NULL;
    if (list.data != NULL) {
        data_snapshot = malloc(list.current_size * item_size);
        __CPROVER_assume(data_snapshot != NULL);
        memcpy(data_snapshot, list.data, list.current_size * item_size);
    }

    /* Call the function under verification */
    int ret = aws_array_list_get_at_ptr(&list, val_ptr, index);

    /* Postcondition checks */
    if (index < list.length) {
        /* Success case */
        assert(ret == AWS_OP_SUCCESS);
        assert(*val_ptr == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure case */
        assert(ret == AWS_ERROR_INVALID_INDEX);
        assert(*val_ptr == old_val);
    }

    /* Frame conditions: list structure must be unchanged */
    assert(list.alloc == list_snapshot.alloc);
    assert(list.item_size == list_snapshot.item_size);
    assert(list.current_size == list_snapshot.current_size);
    assert(list.length == list_snapshot.length);
    assert(list.data == list_snapshot.data);

    /* If data buffer exists, its contents must be unchanged */
    if (list.data != NULL) {
        assert(memcmp(list.data, data_snapshot, list.current_size * item_size) == 0);
        free(data_snapshot);
    }

    /* Clean up */
    aws_array_list_clean_up(&list);
}
