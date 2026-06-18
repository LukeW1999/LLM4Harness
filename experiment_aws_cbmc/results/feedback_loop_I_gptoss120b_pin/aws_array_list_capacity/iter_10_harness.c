#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_array_list_capacity_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet initial allocation and item size within reasonable bounds */
    size_t max_initial = nondet_size_t();
    __CPROVER_assume(max_initial > 0);
    __CPROVER_assume(max_initial <= 10);
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= 64);

    struct aws_array_list list;
    int init_res = aws_array_list_init(&list, allocator, max_initial, item_sz);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocate a dummy element to use for pushes */
    uint8_t *elem = malloc(item_sz);
    __CPROVER_assume(elem != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(elem, item_sz));

    /* nondet number of pushes, bounded by the initial allocation */
    size_t pushes = nondet_size_t();
    __CPROVER_assume(pushes <= max_initial);
    for (size_t i = 0; i < pushes; ++i) {
        int push_res = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_res == AWS_OP_SUCCESS);
    }

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    size_t cap = aws_array_list_capacity(&list);

    /* list must remain unchanged */
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (old.data != NULL && old.current_size > 0) {
        assert_byte_from_buffer_matches(old.data, &old_byte);
    }

    /* capacity definition */
    if (list.item_size > 0) {
        assert(cap == list.current_size / list.item_size);
    }

    free(elem);
    aws_array_list_clean_up(&list);
}
