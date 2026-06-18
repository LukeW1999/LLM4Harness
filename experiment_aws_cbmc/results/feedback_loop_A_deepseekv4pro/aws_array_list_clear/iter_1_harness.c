#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index;

    /* Bound the list to typical limits */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Bound index to avoid excessively large values */
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2 + 1));

    /* Create a readable buffer for val, of size list.item_size */
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL); /* ensure allocation success */
    /* CBMC treats malloc'd memory as both readable and writable by default */

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions: always valid */
    assert(aws_array_list_is_valid(&list));

    /* Unchanged fields: item_size and alloc must not change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* Value at index should match val */
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, (uint8_t *)val, list.item_size);

        /* Length update logic: if index >= old.length, length becomes index+1, else unchanged */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure, all fields must remain unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        /* item_size and alloc already checked above */
    }

    free(val);
}
