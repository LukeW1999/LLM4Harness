#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;

    size_t item_size;
    size_t current_size;
    size_t length;

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(current_size % item_size == 0);

    size_t capacity_val = current_size / item_size;
    __CPROVER_assume(length <= capacity_val);

    void *data;
    if (current_size == 0) {
        data = NULL;
    } else {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    list.item_size    = item_size;
    list.current_size = current_size;
    list.length       = length;
    list.data         = data;
    list.alloc        = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    size_t old_length       = list.length;
    void  *old_data         = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    size_t capacity = aws_array_list_capacity(&list);

    assert(capacity == old_current_size / old_item_size);
    assert(aws_array_list_is_valid(&list));
    assert(list.current_size == old_current_size);
    assert(list.item_size    == old_item_size);
    assert(list.length       == old_length);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);
}
