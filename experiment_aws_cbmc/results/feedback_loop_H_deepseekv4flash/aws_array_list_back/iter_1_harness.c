#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Prepare val pointer with writable memory of size list->item_size */
    size_t val_capacity = list->item_size;  /* bounded by MAX_ITEM_SIZE due to is_bounded */
    uint8_t *val = malloc(val_capacity);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, val_capacity));

    /* 3. Save old state — copy entire list */
    struct aws_array_list old = list;

    /* If list has elements, save the last element for later comparison */
    uint8_t *saved_last_element = NULL;
    size_t last_element_offset;
    if (list.length > 0) {
        last_element_offset = list.item_size * (list.length - 1);
        saved_last_element = malloc(list.item_size);
        __CPROVER_assume(saved_last_element != NULL);
        memcpy(saved_last_element, (uint8_t *)list.data + last_element_offset, list.item_size);
    }

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* List must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* All fields of list must be unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Distinguish success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, so val now contains the last element */
        assert(list.length > 0);
        // Assert that the memory at val matches the saved last element
        assert_bytes_match(val, saved_last_element, list.item_size);
        // Also free saved buffer if allocated (optional in harness, but for completeness)
        free(saved_last_element);
    } else {
        /* Failure: list was empty, error returned, list unchanged (already asserted above) */
        assert(list.length == 0);
        // val is not touched; we cannot assert its contents without saving, but it is safe to ignore.
    }

    /* 6. Cleanup */
    free(val);
}
