#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* nondet list */
    struct aws_array_list list;
    /* bound the list fields using macros (or nondet with bounds) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    /* ensure data is allocated for both dynamic and static cases */
    if (list.alloc != NULL) {
        ensure_array_list_has_allocated_data_member(&list);
    } else {
        /* static list: data must be a writable buffer of current_size */
        if (list.current_size > 0) {
            list.data = malloc(list.current_size);
            __CPROVER_assume(list.data != NULL);
        } else {
            list.data = NULL;
        }
    }
    /* ensure list is valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet val pointer: readable memory of item_size bytes */
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* nondet index */
    size_t index;

    /* save old state */
    struct aws_array_list old = list;

    /* store original content of val for later comparison */
    uint8_t *val_bytes = (uint8_t *)val;
    uint8_t *saved_val = malloc(item_size);
    memcpy(saved_val, val_bytes, item_size);

    /* call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* on success, length may increase: if index >= old length, new length = index+1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        /* item_size must remain unchanged */
        assert(list.item_size == old.item_size);
        /* alloc must remain unchanged */
        assert(list.alloc == old.alloc);
        /* the value at index (in list->data) must equal the bytes pointed to by val */
        uint8_t *target = (uint8_t *)list.data + index * item_size;
        assert_bytes_match(target, val_bytes, item_size);
        /* val's content must not have been modified (RESTRICT) */
        assert_bytes_match(val_bytes, saved_val, item_size);
    } else {
        /* on failure, list must be unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    /* always ensure list is valid */
    assert(aws_array_list_is_valid(&list));

    /* free allocated memory */
    free(val);
    free(saved_val);
    /* if we allocated data for static case, free it here? But it might be reused; for CBMC it's fine to leak */
    if (old.alloc == NULL && old.data != NULL) {
        free(old.data); // but list.data may have been reallocated on success, so we free only old.data
        // better: in CBMC proofs, memory leaks are acceptable; we skip explicit free.
    }
}
