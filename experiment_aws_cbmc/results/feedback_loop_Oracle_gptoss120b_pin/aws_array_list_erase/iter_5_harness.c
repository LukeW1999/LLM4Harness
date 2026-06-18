#include <aws/common/array_list.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic allocation and item size */
    size_t init_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(init_allocation <= 10U);
    __CPROVER_assume(item_size > 0 && item_size <= 16U);

    /* initialize list in dynamic mode */
    int init_res = aws_array_list_init_dynamic(&list, allocator, init_allocation, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_bounded(&list, 10U, 16U));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* populate list with a nondeterministic number of elements */
    size_t max_push = aws_array_list_capacity(&list);
    size_t push_count = nondet_size_t();
    __CPROVER_assume(push_count <= max_push);
    for (size_t i = 0; i < push_count; ++i) {
        uint8_t *elem = malloc(item_size);
        __CPROVER_assume(elem != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = nondet_uint8_t();
        }
        int push_res = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_res == AWS_OP_SUCCESS);
        free(elem);
    }

    /* snapshot before erase */
    size_t old_length = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_sz = list.item_size;
    void *old_data = list.data;

    uint8_t *old_buffer = NULL;
    if (old_data != NULL && old_capacity > 0) {
        old_buffer = malloc(old_capacity * old_item_sz);
        __CPROVER_assume(old_buffer != NULL);
        for (size_t i = 0; i < old_capacity * old_item_sz; ++i) {
            old_buffer[i] = ((uint8_t *)old_data)[i];
        }
    }

    /* nondeterministic index to erase */
    size_t index = nondet_size_t();

    /* call function under verification */
    int rv = aws_array_list_erase(&list, index);

    /* post‑condition checks */
    if (index >= old_length) {
        assert(rv == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    } else {
        assert(rv == AWS_OP_SUCCESS);
    }

    if (index < old_length) {
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }

    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(list.data == old_data);

    if (old_buffer != NULL && old_data != NULL) {
        size_t used_bytes_before = old_length * old_item_sz;
        size_t total_bytes = old_capacity * old_item_sz;
        for (size_t i = used_bytes_before; i < total_bytes; ++i) {
            assert(((uint8_t *)old_data)[i] == old_buffer[i]);
        }
        free(old_buffer);
    }

    aws_array_list_clean_up(&list);
}
