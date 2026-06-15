#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Set a valid item size */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 3. Allocate the internal buffer */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure there is room for at least one element so the non‑empty branch can be taken */
    __CPROVER_assume(list.current_size >= list.item_size);

    /* 4. Choose a length that fits in the buffer (may be zero) */
    size_t max_len = list.current_size / list.item_size;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);
    list.length = len;

    /* 5. Initialise the buffer with nondeterministic data */
    for (size_t i = 0; i < list.current_size; ++i) {
        ((uint8_t *)list.data)[i] = nondet_uint8_t();
    }

    /* 6. Save old state for later equivalence checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage; /* for data‑buffer equivalence */

    /* 7. Keep a copy of the first element (if any) for later comparison */
    uint8_t *front_copy = NULL;
    if (list.length > 0) {
        front_copy = (uint8_t *)malloc(list.item_size);
        __CPROVER_assume(front_copy != NULL);
        aws_memcpy(front_copy,
                  (uint8_t *)list.data,
                  list.item_size);
    }

    /* 8. Call the function under test */
    void *ptr = aws_array_list_front(&list);

    /* 9. Post‑condition checks */
    if (list.length > 0) {
        /* The returned pointer must be the address of the first element */
        assert(ptr == list.data);
        /* The contents must be unchanged */
        assert_bytes_match((uint8_t *)ptr, front_copy, list.item_size);
    } else {
        /* Empty list – should return NULL */
        assert(ptr == NULL);
    }

    /* 10. The list must remain unchanged */
    assert_array_list_equivalence(&list, &old, &storage);
    assert(aws_array_list_is_valid(&list));

    /* 11. Clean up */
    free(front_copy);
}
