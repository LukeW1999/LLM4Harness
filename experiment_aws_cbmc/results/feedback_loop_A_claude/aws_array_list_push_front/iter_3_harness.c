#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Non-deterministically choose item_size and current_size */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;

    size_t initial_item_count;
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    list.current_size = initial_item_count * item_size;

    size_t length;
    __CPROVER_assume(length <= initial_item_count);
    list.length = length;

    /* Set up allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    list.alloc = alloc;

    /* Allocate data buffer */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Ensure the list is valid before proceeding */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Prepare val: a non-deterministic buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state BEFORE calling */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        /* item_size must not change */
        assert(list.item_size == old_item_size);
        /* allocator must not change */
        assert(list.alloc == old_alloc);
        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* data pointer must be non-null (we have at least one element) */
        assert(list.data != NULL);
    } else {
        /* On failure: length should not have changed */
        assert(list.length == old_length);
        /* item_size must not change */
        assert(list.item_size == old_item_size);
        /* allocator must not change */
        assert(list.alloc == old_alloc);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
