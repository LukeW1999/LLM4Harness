#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

/* Helper to capture a snapshot of the list's data region */
static void snapshot_data(const struct aws_array_list *list,
                          uint8_t *buf,
                          size_t bufsize)
{
    if (list->data && list->current_size > 0) {
        size_t copy = list->current_size < bufsize ? list->current_size : bufsize;
        memcpy(buf, list->data, copy);
    }
}

/* CBMC harness */
void aws_array_list_push_back_harness(void) {
    /* Allocate a list structure */
    struct aws_array_list list;

    /* Non‑deterministic item size, must be > 0 */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* Non‑deterministic initial allocation count (may be zero) */
    size_t init_count = nondet_size_t();

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Initialise the list as a dynamic array */
    int init_rc = aws_array_list_init_dynamic(&list, alloc, init_count, item_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Capture pre‑state */
    size_t old_len = aws_array_list_length(&list);
    size_t old_cap = aws_array_list_capacity(&list);
    size_t old_current_size = list.current_size;

    /* Snapshot the existing data region (up to current_size) */
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        snapshot_data(&list, old_data, list.current_size);
    }

    /* Allocate a non‑deterministic value to push */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Call the function under test */
    int ret = aws_array_list_push_back(&list, val);

    /* ---- Postconditions ---- */

    /* List must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Length / return value relationship */
    if (ret == AWS_OP_SUCCESS) {
        assert(aws_array_list_length(&list) == old_len + 1);
    } else {
        assert(aws_array_list_length(&list) == old_len);
    }

    /* Frame condition: memory outside the list's data region is unchanged */
    if (old_data) {
        size_t unchanged_bytes = old_len * item_size;
        if (unchanged_bytes > 0) {
            assert(memcmp(old_data, list.data, unchanged_bytes) == 0);
        }
    }

    /* If the call succeeded, the newly added element must equal the input value */
    if (ret == AWS_OP_SUCCESS) {
        uint8_t *new_elem = (uint8_t *)list.data + old_len * item_size;
        assert(memcmp(new_elem, val, item_size) == 0);
    }

    /* Clean up */
    if (old_data) free(old_data);
    free(val);
    aws_array_list_clean_up(&list);
}
