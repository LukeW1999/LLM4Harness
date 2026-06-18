#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t item_sz = list.item_size ? list.item_size : 1;
    uint8_t *val = (uint8_t *)malloc(item_sz);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            __CPROVER_assert(list.length == index + 1,
                             "length updated when index beyond old length");
        } else {
            __CPROVER_assert(list.length == old.length,
                             "length unchanged when index within old length");
        }

        __CPROVER_assert(
            memcmp((uint8_t *)list.data + (index * list.item_size),
                   val,
                   list.item_size) == 0,
            "value stored correctly");

        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    } else {
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data pointer unchanged on failure");
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    free(val);
}
