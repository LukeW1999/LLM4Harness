#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "aws/common/array_list.h"

void aws_array_list_back_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    int result = aws_array_list_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(val, (uint8_t *)list.data + last_item_offset, list.item_size);
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(list.length == 0);
        assert(aws_array_list_is_valid(&list));
    }

    free(val);
}
