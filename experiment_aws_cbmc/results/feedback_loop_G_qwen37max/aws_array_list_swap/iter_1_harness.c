#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    void *buf_a = NULL;
    void *buf_b = NULL;
    if (list.item_size > 0) {
        buf_a = malloc(list.item_size);
        buf_b = malloc(list.item_size);
        __CPROVER_assume(buf_a != NULL);
        __CPROVER_assume(buf_b != NULL);
        assert(aws_array_list_get_at(&list, buf_a, a) == AWS_OP_SUCCESS);
        assert(aws_array_list_get_at(&list, buf_b, b) == AWS_OP_SUCCESS);
    }

    aws_array_list_swap(&list, a, b);

    if (list.item_size > 0) {
        void *new_buf_a = malloc(list.item_size);
        void *new_buf_b = malloc(list.item_size);
        __CPROVER_assume(new_buf_a != NULL);
        __CPROVER_assume(new_buf_b != NULL);
        assert(aws_array_list_get_at(&list, new_buf_a, a) == AWS_OP_SUCCESS);
        assert(aws_array_list_get_at(&list, new_buf_b, b) == AWS_OP_SUCCESS);
        
        assert_bytes_match((const uint8_t *)new_buf_a, (const uint8_t *)buf_b, list.item_size);
        assert_bytes_match((const uint8_t *)new_buf_b, (const uint8_t *)buf_a, list.item_size);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
