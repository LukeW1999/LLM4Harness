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
    __CPROVER_assume(item_size <= 8);               /* bound for CBMC */

    if (__CPROVER_nondet_bool()) {
        /* ---------- Dynamic list ---------- */
        size_t initial_capacity = __CPROVER_nondet_ulong();
        __CPROVER_assume(initial_capacity > 0);
        __CPROVER_assume(initial_capacity <= 8);   /* bound for CBMC */

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
        /* ---------- Static list ---------- */
        size_t capacity = __CPROVER_nondet_ulong();
        __CPROVER_assume(capacity > 0);
        __CPROVER_assume(capacity <= 8);            /* bound for CBMC */

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

    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *pre_data = NULL;
    if (list.current_size > 0) {
        pre_data = malloc(list.current_size);
        __CPROVER_assume(pre_data != NULL);
        memcpy(pre_data, list.data, list.current_size);
    }

    uint8_t *out = malloc(item_size);
    __CPROVER_assume(out != NULL);

    int ret = aws_array_list_front(&list, out);

    if (ret == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert(memcmp(out, list.data, item_size) == 0);
    } else {
        assert(list.length == 0);
    }

    if (list.current_size > 0) {
        assert(memcmp(pre_data, list.data, list.current_size) == 0);
    }

    free(pre_data);
    free(out);
    if (list.alloc && list.data) {
        aws_mem_release(list.alloc, list.data);
    } else {
        free(list.data);
    }
}
