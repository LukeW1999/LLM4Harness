#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a valid val pointer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - length increases by 1
         * - item_size unchanged
         * - alloc unchanged
         * - data pointer unchanged (same buffer, just shifted contents)
         * - current_size unchanged or possibly grown (ensure_capacity may grow)
         * - first element equals val
         */
        assert(list.length == old_list.length + 1);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        /* current_size may have changed if capacity was grown */
        assert(list.current_size >= old_list.current_size);
        /* The data pointer may have changed if realloc occurred */
        assert(list.data != NULL);
        /* The first element should match val */
        assert(memcmp(list.data, val, list.item_size) == 0);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure:
         * - length unchanged
         * - item_size unchanged
         * - alloc unchanged
         * - current_size unchanged
         * - data unchanged
         */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    }

    /* 6. Fields that never change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
}
