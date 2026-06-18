#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* the malloc'ed memory is readable/writable by construction */
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();
    /* avoid overflow in pointer arithmetic */
    if (list.item_size > 0) {
        __CPROVER_assume(index <= SIZE_MAX / list.item_size);
    }

    /* 4. Save old state */
    struct aws_array_list old = list;
    uint8_t *old_bytes = NULL;
    if (list.item_size > 0 && index < old.length) {
        old_bytes = malloc(list.item_size);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes,
               (uint8_t *)old.data + (index * old.item_size),
               old.item_size);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* the bytes at the target location now match the source */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* length updates */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* data pointer may change in dynamic mode, so we do not assert it */
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    free(old_bytes);
}
