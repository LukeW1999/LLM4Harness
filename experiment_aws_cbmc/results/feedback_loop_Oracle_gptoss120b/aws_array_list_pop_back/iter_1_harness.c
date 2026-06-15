#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/error.h>
#include <aws/common/memory.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers */
extern size_t nondet_size_t(void);
extern bool nondet_bool(void);

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Choose dynamic or static allocation */
    bool use_dynamic = nondet_bool();

    if (use_dynamic) {
        /* Dynamic list */
        size_t item_size = nondet_size_t();
        __CPROVER_assume(item_size > 0);
        __CPROVER_assume(item_size < 1024); /* reasonable bound */

        size_t init_items = nondet_size_t();
        __CPROVER_assume(init_items <= 1024);

        size_t init_bytes;
        __CPROVER_assume(!aws_mul_size_checked(init_items, item_size, &init_bytes));

        int init_res = aws_array_list_init_dynamic(&list, alloc, init_items, item_size);
        __CPROVER_assume(init_res == AWS_OP_SUCCESS);

        size_t capacity = list.current_size / list.item_size;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= capacity);
        list.length = len;
    } else {
        /* Static list */
        size_t item_size = nondet_size_t();
        __CPROVER_assume(item_size > 0);
        __CPROVER_assume(item_size < 1024);

        size_t item_count = nondet_size_t();
        __CPROVER_assume(item_count > 0);
        __CPROVER_assume(item_count <= 1024);

        size_t total_bytes;
        __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &total_bytes));

        void *raw = malloc(total_bytes);
        __CPROVER_assume(raw != NULL);
        aws_array_list_init_static(&list, raw, item_count, item_size);

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= item_count);
        list.length = len;
    }

    /* Snapshot pre‑state */
    size_t old_len = list.length;
    size_t old_capacity = (list.item_size == 0) ? 0 : list.current_size / list.item_size;

    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Call the function under test */
    int ret = aws_array_list_pop_back(&list);

    /* Post‑conditions */

    /* 1. List must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (old_len > 0) {
        /* Successful pop */
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_len - 1);
        assert(list.current_size == old_capacity * list.item_size);

        /* The popped element must be zeroed */
        size_t popped_offset = list.item_size * (old_len - 1);
        uint8_t *popped_ptr = (uint8_t *)list.data + popped_offset;
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(popped_ptr[i] == 0);
        }

        /* Memory before the popped element must be unchanged */
        if (popped_offset > 0) {
            assert(memcmp(old_data, list.data, popped_offset) == 0);
        }
        /* No memory after the popped element exists (it was the last item) */
    } else {
        /* Pop on empty list */
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(list.length == 0);
        if (list.current_size > 0) {
            assert(memcmp(old_data, list.data, list.current_size) == 0);
        }
    }

    /* Clean up */
    if (old_data) {
        free(old_data);
    }
    if (!use_dynamic && list.data) {
        free(list.data);
    }
    aws_array_list_clean_up(&list);

    return 0;
}
