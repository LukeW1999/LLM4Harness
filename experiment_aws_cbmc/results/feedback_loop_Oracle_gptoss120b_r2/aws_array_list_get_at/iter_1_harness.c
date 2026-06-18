#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 64U
#define MAX_CAPACITY   32U

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    struct aws_array_list old_list;
    uint8_t *old_data = NULL;

    /* nondeterministic but bounded item size */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* nondeterministic initial allocation count (items) */
    size_t init_items = nondet_size_t();
    __CPROVER_assume(init_items <= MAX_CAPACITY);

    /* initialize dynamic array list */
    int init_ret = aws_array_list_init_dynamic(
        &list,
        aws_default_allocator(),
        init_items,
        item_size);

    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Determine capacity (in items) from current_size */
    size_t capacity = (list.item_size == 0) ? 0 : (list.current_size / list.item_size);
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* Choose a nondeterministic valid length */
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Fill the allocated buffer with nondeterministic bytes */
    if (list.current_size > 0) {
        uint8_t *buf = (uint8_t *)list.data;
        for (size_t i = 0; i < list.current_size; ++i) {
            buf[i] = nondet_uint8_t();
        }
    }

    /* Preserve a copy of the list structure and its data */
    old_list = list;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        assert(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* nondeterministic index to read */
    size_t index = nondet_size_t();

    /* output buffer */
    uint8_t *out_buf = malloc(item_size);
    assert(out_buf != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        out_buf[i] = nondet_uint8_t();
    }

    /* Call the function under test */
    int ret = aws_array_list_get_at(&list, out_buf, index);

    /* ==== Post‑conditions ==== */

    /* 1. Return value / error code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* success implies index was within bounds */
        assert(index < list.length);
    } else {
        /* failure must be the specific error and index out of bounds */
        assert(ret == AWS_ERROR_INVALID_INDEX);
        assert(index >= list.length);
    }

    /* 2. Output buffer contains the expected element on success */
    if (ret == AWS_OP_SUCCESS) {
        size_t offset = index * list.item_size;
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(out_buf[i] == old_data[offset + i]);
        }
    }

    /* 3. Memory frame conditions – the list and its storage are unchanged */
    assert(old_list.alloc == list.alloc);
    assert(old_list.item_size == list.item_size);
    assert(old_list.current_size == list.current_size);
    assert(old_list.length == list.length);
    assert(old_list.data == list.data);

    if (list.current_size > 0) {
        for (size_t i = 0; i < list.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }

    /* List must still satisfy its invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    if (old_data) {
        free(old_data);
    }
    free(out_buf);
    aws_array_list_clean_up(&list);

    return 0;
}
