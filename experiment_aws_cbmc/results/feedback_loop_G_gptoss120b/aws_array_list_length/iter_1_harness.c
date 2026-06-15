#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

/* Assume these bounds are defined in the Makefile or test harness */
#ifndef MAX_INITIAL_ITEM_ALLOCATION
#   define MAX_INITIAL_ITEM_ALLOCATION 8
#endif
#ifndef MAX_ITEM_SIZE
#   define MAX_ITEM_SIZE 16
#endif

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare and bound the input value */
    /* Ensure the item size is within our static buffer limits */
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = (const void *)val_buf;

    /* Save a copy of the input value for later comparison */
    uint8_t old_val[MAX_ITEM_SIZE];
    memcpy(old_val, val_buf, list.item_size);

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ---- Success path assertions ---- */

        /* The element at the given index must now equal the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           old_val,
                           list.item_size);

        /* Length handling */
        if (index >= old.length) {
            /* Length should have grown to index+1 */
            assert(list.length == index + 1);
        } else {
            /* Length must be unchanged */
            assert(list.length == old.length);
        }

        /* Fields that must not change regardless of index */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size may change due to reallocation, so we do not assert equality */
    } else {
        /* ---- Failure path assertions ---- */

        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* Additionally, the contents of the existing elements must be unchanged */
        if (old.length > 0) {
            size_t bytes = old.length * old.item_size;
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               bytes);
        }
    }

    /* 7. Final invariant: the list is still valid */
    assert(aws_array_list_is_valid(&list));
}
