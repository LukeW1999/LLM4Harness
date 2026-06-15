#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Allocate a readable input value of the appropriate size */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call the function under test */
    int err = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (err == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);
        /* fields that must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* on failure the whole structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
