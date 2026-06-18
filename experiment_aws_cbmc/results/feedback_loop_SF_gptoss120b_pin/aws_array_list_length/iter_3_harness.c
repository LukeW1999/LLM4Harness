#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Reasonable bounds to keep CBMC tractable */
    const size_t MAX_ITEM_SIZE = 8;
    const size_t MAX_LENGTH    = 8;
    const size_t MAX_CAPACITY  = 64;

    /* Nondeterministic fields with bounds */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= MAX_LENGTH);

    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= MAX_CAPACITY);

    /* Ensure current_size is sufficient for length * item_size */
    {
        size_t required = 0;
        if (aws_mul_size_checked(list.length, list.item_size, &required) == AWS_OP_SUCCESS) {
            __CPROVER_assume(list.current_size >= required);
        } else {
            __CPROVER_assume(0); /* overflow not allowed */
        }
    }

    /* Allocate data buffer if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Use default allocator */
    list.alloc = allocator;

    /* Assume the list is valid according to its invariant */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of original state */
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* Postconditions */
    assert(result == old_length);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(list.data);
}
