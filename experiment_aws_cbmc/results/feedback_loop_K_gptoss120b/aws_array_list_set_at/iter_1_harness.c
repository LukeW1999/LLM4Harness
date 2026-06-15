/*=== Contract for aws_array_list_set_at ==============================
Preconditions:
  - list is a pointer to a valid, initialized aws_array_list.
  - list->item_size > 0.
  - val is a non‑NULL pointer to readable memory of size list->item_size.
  - index is any size_t value.

Postconditions (return value):
  - The function returns either AWS_OP_SUCCESS or AWS_OP_ERR.

Postconditions (length):
  - If the function returns AWS_OP_SUCCESS then
        list->length == (old_length > index ? old_length : index + 1)
    where old_length is the length of the list before the call.
  - If the function returns AWS_OP_ERR then
        list->length == old_length.

Postconditions (frame):
  - list->alloc, list->item_size remain unchanged.
  - Elements of the list at positions other than 'index' (if index < old_length)
    remain unchanged.
  - The element at position 'index' (if the call succeeded) is equal to the
    contents of *val.
  - The capacity of the list (aws_array_list_capacity) is never decreased.
====================================================================*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* Allocate and initialize the list */
    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);               /* bound for tractability */

    size_t init_alloc = nondet_size_t();
    __CPROVER_assume(init_alloc > 0);
    __CPROVER_assume(init_alloc <= 10);              /* bound for tractability */

    int init_rc = aws_array_list_init_dynamic(&list, aws_default_allocator(), init_alloc, item_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of old state */
    size_t old_length = list.length;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* Copy old data for later comparison */
    uint8_t *old_data = NULL;
    if (list.data != NULL && old_capacity > 0) {
        old_data = malloc(old_capacity * item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_capacity * item_size);
    }

    /* Allocate and initialize val */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* make val nondet readable */
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* Choose an index */
    size_t index = nondet_size_t();

    /* Call the function under test */
    int rc = aws_array_list_set_at(&list, val, index);

    /* ---- Postcondition checks ---- */
    /* Return value must be success or error */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);

    /* allocator, item_size must stay the same */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    /* current_size must be length * item_size (or zero if data is NULL) */
    if (list.data != NULL) {
        assert(list.current_size == list.length * list.item_size);
    } else {
        assert(list.current_size == 0);
    }

    /* Capacity must never shrink */
    assert(aws_array_list_capacity(&list) >= old_capacity);

    if (rc == AWS_OP_SUCCESS) {
        /* Length must be max(old_length, index+1) */
        size_t expected_len = (index >= old_length) ? (index + 1) : old_length;
        assert(list.length == expected_len);

        /* Verify unchanged elements */
        if (old_data != NULL) {
            size_t min_len = (old_length < list.length) ? old_length : list.length;
            for (size_t i = 0; i < min_len; ++i) {
                if (i != index) {
                    assert(
                        memcmp(
                            (uint8_t *)list.data + i * item_size,
                            old_data + i * item_size,
                            item_size) == 0);
                }
            }
        }

        /* Verify element at index matches val (if index is within capacity) */
        if (list.data != NULL && index < aws_array_list_capacity(&list)) {
            assert(
                memcmp(
                    (uint8_t *)list.data + index * item_size,
                    val,
                    item_size) == 0);
        }
    } else {
        /* On error, length and data must be unchanged */
        assert(list.length == old_length);
        if (old_data != NULL && list.data != NULL) {
            assert(memcmp(list.data, old_data, old_capacity * item_size) == 0);
        }
    }

    /* Clean up */
    if (list.data != NULL) {
        aws_array_list_clean_up(&list);
    }
    free(val);
    free(old_data);

    return 0;
}
