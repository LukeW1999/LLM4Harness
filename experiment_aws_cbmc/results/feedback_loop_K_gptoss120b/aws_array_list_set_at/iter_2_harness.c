/*=== Harness for aws_array_list_set_at ==============================
   Checks postconditions of aws_array_list_set_at.
   ====================================================================*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_array_list_set_at_harness(void) {
    /* Allocate and initialize a dynamic array list */
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

    /* Preserve old data for later comparison */
    uint8_t *old_data = NULL;
    if (list.data != NULL && old_capacity > 0) {
        old_data = malloc(old_capacity * item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_capacity * item_size);
    }

    /* Allocate and initialize val */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
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

    /* allocator and item_size must stay unchanged */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    /* current_size must reflect length * item_size (or zero if data is NULL) */
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

        /* Verify element at index matches val (if within capacity) */
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
}
