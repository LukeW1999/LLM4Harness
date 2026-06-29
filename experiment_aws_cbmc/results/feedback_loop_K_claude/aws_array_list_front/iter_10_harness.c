#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(list.item_size > 0 && list.item_size <= 8);

    size_t max_items;
    __CPROVER_assume(max_items > 0 && max_items <= 4);
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(max_items, list.item_size, &current_size));
    list.current_size = current_size;

    __CPROVER_assume(list.length <= max_items);
    __CPROVER_assume(list.length > 0);

    uint8_t *data_buf = malloc(list.current_size);
    __CPROVER_assume(data_buf != NULL);
    list.data = data_buf;

    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* val must be large enough to hold item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    size_t old_length       = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    void  *old_data         = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    int result = aws_array_list_front(&list, val);

    assert(aws_array_list_is_valid(&list));

    assert(list.length       == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size    == old_item_size);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);

    if (old_length > 0) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }
}
