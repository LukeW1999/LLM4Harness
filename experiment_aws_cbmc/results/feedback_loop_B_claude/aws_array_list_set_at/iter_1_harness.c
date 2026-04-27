#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a non-deterministic val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Non-deterministic index */
    size_t index;
    /* Bound index to avoid state space explosion */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - data was written at index * item_size
         * - if index >= old length, length = index + 1
         * - if index < old length, length unchanged
         */
        assert(aws_array_list_is_valid(&list));

        /* item_size never changes */
        assert(list.item_size == old_list.item_size);

        /* alloc never changes */
        assert(list.alloc == old_list.alloc);

        /* data pointer may change (realloc), but must be non-null */
        assert(list.data != NULL);

        /* current_size must be >= (index+1)*item_size on success */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* length update: if index >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_list.length);
        }

        /* The value was actually written */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

    } else {
        /* On failure:
         * - aws_array_list_ensure_capacity failed OR aws_add_size_checked failed
         * - In either case the list should still be valid
         * - item_size, alloc unchanged
         */
        assert(aws_array_list_is_valid(&list));
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }
}
