#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_erase_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* list under test */
    struct aws_array_list list;
    aws_array_list_init_dynamic(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet fill the list with a length between 0 and capacity */
    size_t fill_len = nondet_size_t();
    __CPROVER_assume(fill_len <= MAX_INITIAL_ITEM_ALLOCATION);
    for (size_t i = 0; i < fill_len; ++i) {
        uint8_t elem[MAX_ITEM_SIZE];
        for (size_t j = 0; j < MAX_ITEM_SIZE; ++j) {
            elem[j] = nondet_uint8_t();
        }
        int push_ret = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_ret == AWS_OP_SUCCESS);
    }

    /* capture pre‑state */
    size_t old_length       = aws_array_list_length(&list);
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    uint8_t *old_data       = NULL;
    if (list.data != NULL && old_current_size > 0) {
        old_data = malloc(old_current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_current_size);
    }

    /* nondet index */
    size_t index = nondet_size_t();

    /* call the function under verification */
    int ret = aws_array_list_erase(&list, index);

    /* 1. return value / error code correctness */
    if (index >= old_length) {
        assert(ret != AWS_OP_SUCCESS);
    } else {
        assert(ret == AWS_OP_SUCCESS);
    }

    /* 2. length / capacity invariants */
    if (index >= old_length) {
        assert(aws_array_list_length(&list) == old_length);
    } else {
        assert(aws_array_list_length(&list) == old_length - 1);
    }
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == alloc);

    /* 3. frame condition – memory outside the modified region unchanged */
    if (old_data != NULL) {
        if (old_length > 0 && index < old_length) {
            /* after erase, the tail region starting at (old_length-1)*item_size stays unchanged */
            size_t start = (old_length - 1) * old_item_size;
            for (size_t i = start; i < old_current_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        } else {
            /* no erase performed – whole buffer unchanged */
            for (size_t i = 0; i < old_current_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* clean up */
    if (list.data != NULL) {
        aws_mem_release(alloc, list.data);
    }
    aws_array_list_clean_up(&list);
    if (old_data != NULL) {
        free(old_data);
    }
}
