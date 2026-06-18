#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#define MAX_ITEM_SIZE 256
#define MAX_INITIAL_ITEM_ALLOCATION 4

void aws_array_list_get_at_harness() {
    /* Non-deterministically create a valid, bounded array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set up output buffer */
    size_t index;
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* Call function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* val must contain the correct element */
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)list.data + (index * list.item_size),
                           list.item_size);
        /* List must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        /* On failure, list must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* Invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
