#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Output pointer - must be a valid pointer to a void* */
    void **val = malloc(sizeof(void *));
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index < length, val points to element at index */
        assert(index < old_length);
        assert(*val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: index >= length */
        assert(index >= old_length);
    }

    /* 7. List fields must not change regardless of result */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);

    /* 8. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
