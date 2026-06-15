#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.item_size > 0);

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_bytes);
    }

    void *front = aws_array_list_front(&list);

    if (list.length > 0) {
        assert(front != NULL);
        assert(front == list.data);
        assert_bytes_match((uint8_t *)front,
                           (uint8_t *)list.data,
                           list.item_size);
    } else {
        assert(front == NULL);
    }

    assert_array_list_equivalence(&list, &old, &old_bytes);
    assert(aws_array_list_is_valid(&list));
}
