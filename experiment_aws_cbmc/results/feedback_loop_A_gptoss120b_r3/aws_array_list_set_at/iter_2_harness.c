#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for val */
    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = (uint8_t *)aws_default_allocator()->malloc(
            aws_default_allocator(), list.item_size);
        __CPROVER_assume(val_buf != NULL);
    }
    const void *val = (const void *)val_buf;

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            /* on success, length must have grown to index+1 */
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* validity invariant */
        assert(aws_array_list_is_valid(&list));
    }

    /* clean up allocated buffer */
    if (val_buf) {
        aws_default_allocator()->free(aws_default_allocator(), val_buf);
    }
}
