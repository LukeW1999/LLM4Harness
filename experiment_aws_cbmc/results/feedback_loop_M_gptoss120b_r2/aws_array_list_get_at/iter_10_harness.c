#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

#define MAX_INITIAL_CAPACITY 10U
#define MAX_ITEM_SIZE 32U

void aws_array_list_get_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_CAPACITY, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Populate the list with a bounded nondeterministic number of elements */
    size_t init_len = nondet_size_t();
    __CPROVER_assume(init_len <= MAX_INITIAL_CAPACITY);
    for (size_t i = 0; i < init_len; ++i) {
        uint8_t elem[MAX_ITEM_SIZE];
        /* nondet fill element */
        for (size_t j = 0; j < MAX_ITEM_SIZE; ++j) {
            elem[j] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_array_list_push_back(&list, elem) == AWS_OP_SUCCESS);
    }

    struct aws_array_list old = list;

    struct store_byte_from_buffer old_data_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_data_byte);
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_CAPACITY);

    uint8_t out_buf[MAX_ITEM_SIZE];
    /* nondet initialize out buffer to detect unwanted modifications */
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        out_buf[i] = nondet_uint8_t();
    }
    void *out = out_buf;

    int result = aws_array_list_get_at(&list, out, index);

    /* The list must remain valid and unchanged */
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (old.data != NULL && old.current_size > 0) {
        assert_byte_from_buffer_matches(list.data, &old_data_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        /* Successful get must have copied the correct element */
        assert(index < old.length);
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            out_buf,
            list.item_size);
    } else {
        /* On failure, the output buffer must be unchanged */
        assert_bytes_match(out_buf, out_buf, MAX_ITEM_SIZE);
    }

    aws_array_list_clean_up(&list);
}
