#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* 1. Set up the list */
    struct aws_array_list list;

    /* Bound the list to keep the state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Output buffer: readable/writable buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Snapshot old state */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Assert postconditions */

    /* --- Invariant: list is always valid after the call --- */
    assert(aws_array_list_is_valid(&list));

    /* --- Unchanged fields regardless of result --- */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         *   - index must have been within bounds
         *   - val now contains the bytes from list.data[index * item_size]
         */
        assert(index < list.length);
        assert(list.data != NULL);

        /* The bytes at the read position match val */
        uint8_t *src = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(src, (uint8_t *)val, list.item_size);

    } else {
        /* On failure:
         *   - index must have been out of bounds
         */
        assert(index >= list.length);
    }
}
