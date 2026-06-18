#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save original state */
    struct aws_array_list old = list;
    size_t item_size = list.item_size;
    size_t length = list.length;
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    __CPROVER_assume(length * item_size <= sizeof(old_data));
    memcpy(old_data, list.data, length * item_size);

    aws_array_list_swap(&list, a, b);

    /* List structure must not change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Verify element contents after swap using memcmp */
    if (a == b) {
        /* All bytes unchanged */
        assert(memcmp(list.data, old_data, length * item_size) == 0);
    } else {
        /* For each index k except a and b, element k must be unchanged */
        for (size_t k = 0; k < length; k++) {
            if (k != a && k != b) {
                assert(memcmp((uint8_t *)list.data + k * item_size,
                               old_data + k * item_size, item_size) == 0);
            }
        }
        /* Check swapped elements */
        assert(memcmp((uint8_t *)list.data + a * item_size,
                       old_data + b * item_size, item_size) == 0);
        assert(memcmp((uint8_t *)list.data + b * item_size,
                       old_data + a * item_size, item_size) == 0);
    }

    /* Validity must be preserved */
    assert(aws_array_list_is_valid(&list));
}
