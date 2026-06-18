#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a val buffer of item_size bytes */
    void *val = malloc(list.item_size);
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
        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* The first element should match val */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)val, list.item_size);
    } else {
        /* On failure: length unchanged */
        assert(list.length == old_list.length);
        /* item_size unchanged */
        assert(list.item_size == old_list.item_size);
        /* alloc unchanged */
        assert(list.alloc == old_list.alloc);
        /* current_size unchanged */
        assert(list.current_size == old_list.current_size);
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
