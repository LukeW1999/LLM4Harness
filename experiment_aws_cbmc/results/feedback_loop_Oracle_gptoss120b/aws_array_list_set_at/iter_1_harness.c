#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* Set up a nondeterministic but bounded array list */
    struct aws_array_list list;
    make_array_list(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* Allocate a nondeterministic value buffer */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
    }

    /* Ground‑truth preconditions (copy exactly) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state for post‑condition checks */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    uint8_t *old_data = NULL;
    if (old_current_size > 0) {
        old_data = malloc(old_current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_current_size);
    }
    void *old_val = NULL;
    if (list.item_size > 0) {
        old_val = malloc(list.item_size);
        __CPROVER_assume(old_val != NULL);
        memcpy(old_val, val, list.item_size);
    }

    /* Nondeterministic index */
    size_t index = nondet_size_t();

    /* Call the function under verification */
    int ret = aws_array_list_set_at(&list, val, index);

    /* 1. Return value must be a valid error code */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. List must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 3. Length / capacity invariants */
    if (ret == AWS_OP_SUCCESS) {
        size_t expected_len = old_length;
        if (index + 1 > old_length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);
        assert(aws_array_list_capacity(&list) >= list.length);
    } else {
        /* On error the length must be unchanged */
        assert(list.length == old_length);
    }

    /* 4. Data at the target index must be updated on success */
    if (ret == AWS_OP_SUCCESS && list.item_size > 0) {
        assert(memcmp((uint8_t *)list.data + (index * list.item_size), val, list.item_size) == 0);
    }

    /* 5. Frame condition: memory outside the written element must be unchanged */
    if (old_data != NULL) {
        for (size_t i = 0; i < old_current_size; ++i) {
            bool in_written_region = false;
            if (list.item_size > 0) {
                size_t start = index * list.item_size;
                size_t end   = start + list.item_size;
                in_written_region = (i >= start && i < end);
            }
            if (!in_written_region) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* 6. The allocator, item size, and other structural fields (except possibly current_size) must be unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.current_size >= old_current_size); /* may grow in dynamic mode */

    /* 7. The input value buffer must not be modified */
    if (old_val != NULL) {
        assert(memcmp(val, old_val, list.item_size) == 0);
    }

    /* Clean up */
    free(old_data);
    free(old_val);
    free(val);
    aws_array_list_clean_up(&list);
    return 0;
}
