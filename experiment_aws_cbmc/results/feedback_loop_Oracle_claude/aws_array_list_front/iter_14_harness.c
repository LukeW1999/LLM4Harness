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

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 8);

    size_t length;
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 4);
    __CPROVER_assume(length <= capacity);
    __CPROVER_assume(length > 0);

    list.item_size = item_size;
    list.length = length;

    __CPROVER_assume(capacity <= SIZE_MAX / item_size);
    size_t current_size = capacity * item_size;
    list.current_size = current_size;

    uint8_t *data = malloc(current_size);
    __CPROVER_assume(data != NULL);
    list.data = data;

    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate output buffer with exactly item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;

    int result = aws_array_list_front(&list, val);

    if (old_length > 0) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }

    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(aws_array_list_is_valid(&list));
}
