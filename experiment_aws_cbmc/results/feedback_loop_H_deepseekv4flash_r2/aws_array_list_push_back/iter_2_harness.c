#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    const void *val;
    size_t index;

    /* Bound and initialize list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Bound and initialize val */
    size_t item_size = list.item_size;
    __CPROVER_assume(item_size > 0);
    uint8_t *val_bytes = malloc(item_size);
    __CPROVER_assume(val_bytes != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val_bytes, item_size));
    val = (const void *)val_bytes;

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_data);
    }

    /* Call function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = (index >= old.length) ? index + 1 : old.length;
        assert(list.length == expected_length);
        if (index < list.length) {
            uint8_t *stored = (uint8_t *)list.data + (index * list.item_size);
            assert_bytes_match(stored, (const uint8_t *)val, list.item_size);
        }
        assert(list.current_size >= index * list.item_size + list.item_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (list.current_size == old.current_size && list.data != NULL && old.data != NULL) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_data);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(val_bytes);
    free(list.data);
}
