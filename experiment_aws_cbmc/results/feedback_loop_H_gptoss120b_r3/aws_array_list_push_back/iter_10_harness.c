#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    struct aws_array_list list;
    aws_array_list_init_dynamic(&list, allocator, 0, item_size);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    size_t index = nondet_size_t();

    struct aws_array_list old = list;
    size_t old_length = old.length;
    size_t old_current_size = old.current_size;
    void *old_data = old.data;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = old_length;
        if (index >= old_length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);
        assert(memcmp((uint8_t *)list.data + (index * list.item_size),
                      val,
                      list.item_size) == 0);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
