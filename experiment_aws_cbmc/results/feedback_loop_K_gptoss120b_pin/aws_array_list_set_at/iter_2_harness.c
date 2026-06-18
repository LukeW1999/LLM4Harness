/*=== Harness for aws_array_list_set_at ===*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_array_list_set_at_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* list */
    struct aws_array_list list;
    size_t item_size;
    size_t initial_allocation;

    /* nondet item size and initial allocation */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024);
    __CPROVER_assume(initial_allocation <= 10);

    /* initialize list */
    if (aws_array_list_init_dynamic(&list, alloc, initial_allocation, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    /* capture old state */
    size_t old_length = list.length;
    size_t old_capacity = aws_array_list_capacity(&list);
    uint8_t *orig_data = list.data;
    uint8_t *old_data = NULL;
    if (orig_data != NULL) {
        old_data = malloc(old_capacity * item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, orig_data, old_capacity * item_size);
    }

    /* nondet index (allow out‑of‑bounds) */
    size_t index;
    __CPROVER_assume(index <= old_capacity * 2);

    /* nondet value to set */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* make val readable and nondet‑initialized */
    __CPROVER_assume(__CPROVER_is_fresh(val, item_size));
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* call function under test */
    int ret = aws_array_list_set_at(&list, val, index);

    /* postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* postcondition: validity */
    if (ret == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
    }

    /* postcondition: length */
    if (ret == AWS_OP_SUCCESS) {
        size_t expected_length = (index >= old_length) ? index + 1 : old_length;
        assert(list.length == expected_length);
        assert(list.length <= aws_array_list_capacity(&list));
    }

    /* frame: allocator and item size unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);

    /* frame: data buffer contents when not reallocated */
    if (list.data == orig_data && old_data != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            uint8_t *elem_ptr = (uint8_t *)list.data + i * item_size;
            if (i == index) {
                assert(memcmp(elem_ptr, val, item_size) == 0);
            } else if (i < old_length) {
                assert(memcmp(elem_ptr, old_data + i * item_size, item_size) == 0);
            }
        }
    }

    /* clean up */
    if (old_data) {
        free(old_data);
    }
    if (val) {
        free(val);
    }
    aws_array_list_clean_up(&list);
}
