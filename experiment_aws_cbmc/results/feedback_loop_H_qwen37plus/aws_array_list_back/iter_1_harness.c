#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;

    uint8_t *expected_val = NULL;
    if (list.length > 0) {
        expected_val = malloc(list.item_size);
        __CPROVER_assume(expected_val != NULL);
        size_t last_item_offset = list.item_size * (list.length - 1);
        for (size_t i = 0; i < list.item_size; ++i) {
            expected_val[i] = ((uint8_t *)list.data)[last_item_offset + i];
        }
    }

    int result = aws_array_list_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert_bytes_match((uint8_t *)val, expected_val, list.item_size);
    } else {
        assert(list.length == 0);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
