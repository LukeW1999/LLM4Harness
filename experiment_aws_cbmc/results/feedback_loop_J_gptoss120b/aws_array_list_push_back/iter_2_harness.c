#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to push */
    size_t item_sz = list.item_size;
    uint8_t *val = NULL;
    if (item_sz > 0) {
        val = malloc(item_sz);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));
    }

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    int result = aws_array_list_push_back(&list, val);

    /* 5. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by exactly one */
        assert(list.length == old.length + 1);

        /* item_size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* The stored bytes at the new last index must match the source value */
        if (list.data != NULL && item_sz > 0) {
            size_t last_index = list.length - 1;
            assert_bytes_match(
                (uint8_t *)list.data + (last_index * list.item_size),
                val,
                list.item_size);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 6. Clean up */
    free(val);
}
