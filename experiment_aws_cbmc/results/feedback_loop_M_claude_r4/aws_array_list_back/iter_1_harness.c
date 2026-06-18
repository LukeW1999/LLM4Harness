#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_array_list old_list = list;

    /* 3. Allocate val buffer of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* The list itself must not be modified by aws_array_list_back */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, val now contains the last element */
        assert(list.length > 0);
        /* The data copied into val should match the last element in the list */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)list.data + last_item_offset,
                           list.item_size);
    } else {
        /* Failure: list was empty */
        assert(list.length == 0);
    }

    /* 6. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
