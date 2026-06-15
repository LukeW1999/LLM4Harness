#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare an output buffer */
    size_t item_sz = list.item_size;               /* item_size > 0 by validity */
    uint8_t *out = malloc(item_sz);
    __CPROVER_assume(out != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        out[i] = nondet_uint8_t();
    }

    /* 4. Capture the element that should be popped (if any) */
    uint8_t *expected = NULL;
    if (old.length > 0) {
        expected = malloc(item_sz);
        __CPROVER_assume(expected != NULL);
        memcpy(expected,
               (uint8_t *)old.data + ((old.length - 1) * old.item_size),
               old.item_size);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_pop_back(&list, out);

    /* 6. Global invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path ----- */
        /* Length must decrease by one */
        assert(list.length == old.length - 1);
        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* Output buffer must contain the popped element */
        assert(expected != NULL);
        assert_bytes_match(out, expected, item_sz);
    } else {
        /* ----- Failure path ----- */
        /* List must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* Clean up */
    free(out);
    free(expected);
}
