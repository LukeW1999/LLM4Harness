#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    uint8_t val_buf[MAX_ITEM_SIZE];

    int result = aws_array_list_push_front(&list, val_buf);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        } else {
            assert(list.current_size >= old.current_size);
        }
        assert_bytes_match((const uint8_t *)list.data, val_buf, list.item_size);
        for (size_t i = 0; i < old.length; ++i) {
            assert_bytes_match(
                (const uint8_t *)list.data + (i + 1) * list.item_size,
                (const uint8_t *)old.data + i * old.item_size,
                list.item_size
            );
        }
    } else {
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
