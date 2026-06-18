#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    size_t item_size;

    /* Bound the list and ensure it is valid */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic item size (must match list->item_size for val to be readable) */
    item_size = list.item_size;

    /* Allocate val with readable memory of item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index is set to val */
        /* The length may increase if index >= old length */
        if (index >= old.length) {
            /* Length should be index + 1 */
            assert(list.length == index + 1);
        } else {
            /* Length unchanged */
            assert(list.length == old.length);
        }
        /* The data at index should match val */
        assert_bytes_match((uint8_t *)list.data + (item_size * index), (uint8_t *)val, item_size);
    } else {
        /* On failure, the list should be unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
    }

    /* Unchanged fields regardless of success/failure */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    /* data pointer itself should not change (though contents may) */
    assert(list.data == old.data);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
