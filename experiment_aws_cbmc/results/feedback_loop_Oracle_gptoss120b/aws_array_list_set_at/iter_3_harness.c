#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_array_list_set_at_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic bounded array list */
    struct aws_array_list list;
    make_array_list(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* allocate a nondeterministic value buffer */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* pre‑conditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);
    if (val) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* save old state */
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
    if (val) {
        old_val = malloc(list.item_size);
        __CPROVER_assume(old_val != NULL);
        memcpy(old_val, val, list.item_size);
    }

    /* nondeterministic index */
    size_t index = nondet_size_t();

    /* call function under verification */
    int ret = aws_array_list_set_at(&list, val, index);

    /* 1. return value must be a valid error code */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 3. length / capacity invariants */
    if (ret == AWS_OP_SUCCESS) {
        size_t expected_len = old_length;
        if (index + 1 > old_length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);
        assert(aws_array_list_capacity(&list) >= list.length);
    } else {
        assert(list.length == old_length);
    }

    /* 4. data at target index must be updated on success */
    if (ret == AWS_OP_SUCCESS && list.item_size > 0) {
        assert(memcmp((uint8_t *)list.data + (index * list.item_size), val, list.item_size) == 0);
    }

    /* 5. frame condition: memory outside written element unchanged */
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

    /* 6. allocator, item size, and structural fields (except possibly current_size) unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.current_size >= old_current_size); /* may grow */

    /* 7. input value buffer must not be modified */
    if (old_val != NULL) {
        assert(memcmp(val, old_val, list.item_size) == 0);
    }

    /* clean up */
    free(old_data);
    free(old_val);
    free(val);
    aws_array_list_clean_up(&list);
}
