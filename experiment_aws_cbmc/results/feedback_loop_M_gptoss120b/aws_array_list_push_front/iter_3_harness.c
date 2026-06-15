#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum bounds for nondeterministic values */
#define MAX_ITEM_SIZE               64U
#define MAX_INITIAL_ITEM_ALLOCATION 8U

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    /* item_size > 0 and bounded */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    /* current_size bounded and a multiple of item_size */
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * list.item_size);
    __CPROVER_assume(list.current_size % list.item_size == 0);
    /* length bounded and consistent with current_size */
    __CPROVER_assume(list.length <= list.current_size / list.item_size);
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    /* Use the default allocator */
    list.alloc = aws_default_allocator();
    /* Allocate the internal data buffer */
    ensure_array_list_has_allocated_data_member(&list);
    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Prepare a valid input value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* Save a copy of the input value for later comparison */
    uint8_t *val_copy = malloc(list.item_size);
    __CPROVER_assume(val_copy != NULL);
    memcpy(val_copy, val, list.item_size);

    /* 3. Save old state before the call */
    struct aws_array_list old = list;

    uint8_t *old_first = NULL;
    if (old.length > 0) {
        old_first = malloc(old.item_size);
        __CPROVER_assume(old_first != NULL);
        memcpy(old_first,
               (const uint8_t *)old.data,
               old.item_size);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by one */
        assert(list.length == old.length + 1);
        /* The first element must now be the value we pushed */
        assert(memcmp(list.data, val_copy, list.item_size) == 0);
        /* If there was a previous first element, it must now be at index 1 */
        if (old.length > 0) {
            const uint8_t *new_second = (const uint8_t *)list.data + list.item_size;
            assert(memcmp(new_second, old_first, list.item_size) == 0);
        }
        /* Allocator and item_size must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* 6. Invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
    free(val_copy);
    if (old_first) {
        free(old_first);
    }
}
