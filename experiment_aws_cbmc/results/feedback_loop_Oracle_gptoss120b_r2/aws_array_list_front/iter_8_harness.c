#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8); /* bound for CBMC */

    if (__CPROVER_nondet_bool()) {
        /* Dynamic list */
        size_t initial_capacity = __CPROVER_nondet_ulong();
        __CPROVER_assume(initial_capacity > 0);
        __CPROVER_assume(initial_capacity <= 8); /* bound for CBMC */

        struct aws_allocator *alloc = aws_default_allocator();

        if (aws_array_list_init_dynamic(&list, alloc, initial_capacity, item_size) != AWS_OP_SUCCESS) {
            return;
        }

        size_t length = __CPROVER_nondet_ulong();
        __CPROVER_assume(length <= initial_capacity);
        for (size_t i = 0; i < length; ++i) {
            uint8_t *elem = malloc(item_size);
            __CPROVER_assume(elem != NULL);
            nondet_fill(elem, item_size);
            int push_ret = aws_array_list_push_back(&list, elem);
            __CPROVER_assume(push_ret == AWS_OP_SUCCESS);
            free(elem);
        }
    } else {
        /* Static list */
        size_t capacity = __CPROVER_nondet_ulong();
        __CPROVER_assume(capacity > 0);
        __CPROVER_assume(capacity <= 8); /* bound for CBMC */

        uint8_t *raw = malloc(capacity * item_size);
        __CPROVER_assume(raw != NULL);
        aws_array_list_init_static(&list, raw, capacity, item_size);

        size_t length = __CPROVER_nondet_ulong();
        __CPROVER_assume(length <= capacity);
        for (size_t i = 0; i < length; ++i) {
            uint8_t *elem = malloc(item_size);
            __CPROVER_assume(elem != NULL);
            nondet_fill(elem, item_size);
            int push_ret = aws_array_list_push_back(&list, elem);
            __CPROVER_assume(push_ret == AWS_OP_SUCCESS);
            free(elem);
        }
    }

    /* Ensure the list is in a valid state */
    assert(aws_array_list_is_valid(&list));

    /* Preserve a copy of the entire list data */
    uint8_t *pre_data = NULL;
    if (list.current_size > 0) {
        pre_data = malloc(list.current_size);
        __CPROVER_assume(pre_data != NULL);
        for (size_t i = 0; i < list.current_size; ++i) {
            pre_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    /* Preserve the first element if it exists */
    uint8_t *expected_first = NULL;
    if (list.length > 0) {
        expected_first = malloc(item_size);
        __CPROVER_assume(expected_first != NULL);
        for (size_t i = 0; i < item_size; ++i) {
            expected_first[i] = ((uint8_t *)list.data)[i];
        }
    }

    uint8_t *out = malloc(item_size);
    __CPROVER_assume(out != NULL);

    int ret = aws_array_list_front(&list, out);

    if (list.length > 0) {
        assert(ret == AWS_OP_SUCCESS);
        for (size_t i = 0; i < item_size; ++i) {
            assert(out[i] == expected_first[i]);
        }
    } else {
        assert(ret != AWS_OP_SUCCESS);
        assert(list.length == 0);
    }

    /* Verify the list data was not modified by aws_array_list_front */
    if (list.current_size > 0) {
        for (size_t i = 0; i < list.current_size; ++i) {
            assert(pre_data[i] == ((uint8_t *)list.data)[i]);
        }
    }

    free(pre_data);
    free(expected_first);
    free(out);
    if (list.alloc && list.data) {
        aws_mem_release(list.alloc, list.data);
    } else {
        free(list.data);
    }
}
