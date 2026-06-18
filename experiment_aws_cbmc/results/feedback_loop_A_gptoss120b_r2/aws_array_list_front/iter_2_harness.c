#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and initialize the array list with nondeterministic parameters */
    struct aws_array_list list;
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);
    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    aws_array_list_init(&list, allocator, initial_capacity, item_sz);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer */
    uint8_t *val = aws_mem_acquire(allocator, item_sz);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.length == old.length);

        /* the element at the given index must now equal the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    aws_mem_release(allocator, val);
    aws_array_list_clean_up(&list);
}
