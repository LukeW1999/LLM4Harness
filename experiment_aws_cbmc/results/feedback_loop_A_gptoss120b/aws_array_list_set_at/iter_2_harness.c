#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* 1. Set up allocator and bounds */
    struct aws_allocator *alloc = aws_default_allocator();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 2. Initialise a bounded, valid array list */
    struct aws_array_list list;
    int init_res = aws_array_list_init(&list, alloc, capacity, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Prepare a readable input value */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    uint8_t *val_bytes = (uint8_t *)val;
    for (size_t i = 0; i < item_size; ++i) {
        val_bytes[i] = nondet_uint8_t();
    }

    /* Keep a copy for later comparison */
    uint8_t *saved_val = malloc(item_size);
    __CPROVER_assume(saved_val != NULL);
    memcpy(saved_val, val, item_size);

    /* 5. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 6. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must be at least the old length and at least index+1 */
        assert(list.length >= old.length);
        assert(list.length >= index + 1);

        /* The element at the given index now matches the input value */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index),
                      saved_val,
                      list.item_size) == 0);
    } else {
        /* On failure the length must not have changed */
        assert(list.length == old.length);
    }

    /* 8. Fields that must remain unchanged regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 9. The list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 10. Clean up */
    aws_array_list_clean_up(&list);
    free(val);
    free(saved_val);
}
