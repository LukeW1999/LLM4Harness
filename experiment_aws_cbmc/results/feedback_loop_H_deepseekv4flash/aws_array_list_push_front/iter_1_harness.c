#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_push_front_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    const void *val;

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data member */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume preconditions: list is valid and val is readable */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state for postcondition checks */
    struct aws_array_list old = list;

    /* Copy old data content to a separate buffer to avoid aliasing issues with realloc */
    uint8_t *old_data_copy = malloc(old.current_size);
    __CPROVER_assume(old_data_copy != NULL);
    memcpy(old_data_copy, old.data, old.current_size);

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Postcondition: validity invariant */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Success case: length increased by 1 */
        assert(list.length == old.length + 1);

        /* First element in list matches val */
        assert_bytes_match((const uint8_t *)list.data, (const uint8_t *)val, list.item_size);

        /* Remaining elements are shifted by item_size; compare with old data */
        size_t shift = list.item_size;
        size_t count = old.length * list.item_size;
        assert_bytes_match((const uint8_t *)list.data + shift, old_data_copy, count);

        /* Allocator and item_size unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* Failure case: list is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Data content unchanged */
        assert_bytes_match((const uint8_t *)list.data, old_data_copy, old.current_size);
    }

    /* Clean up copy */
    free(old_data_copy);
}
