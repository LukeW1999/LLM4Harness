#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a val pointer with readable memory of item_size bytes */
    /* item_size is already bounded by MAX_ITEM_SIZE via aws_array_list_is_bounded */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_list.length + 1);
        /* item_size unchanged */
        assert(list.item_size == old_list.item_size);
        /* alloc unchanged */
        assert(list.alloc == old_list.alloc);
        /* current_size must be >= new length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* The first element should now be val (data was copied) */
        assert(list.data != NULL);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: list should remain valid */
        /* length should not have changed (or may be unchanged) */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    }

    /* Fields that must not change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
