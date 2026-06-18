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
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    __CPROVER_assume(list.length * list.item_size <= sizeof(old_data));
    memcpy(old_data, list.data, list.length * list.item_size);

    aws_array_list_swap(&list, a, b);

    /* List structure must not change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Verify element contents after swap */
    if (a == b) {
        assert_bytes_match(list.data, old_data, list.length * list.item_size);
    } else {
        size_t min_idx = (a < b) ? a : b;
        size_t max_idx = (a < b) ? b : a;

        /* Bytes before the first swapped element unchanged */
        assert_bytes_match((uint8_t *)list.data, old_data, min_idx * list.item_size);

        /* Element at a now contains old element at b */
        assert_bytes_match((uint8_t *)list.data + a * list.item_size,
                           old_data + b * list.item_size,
                           list.item_size);

        /* Element at b now contains old element at a */
        assert_bytes_match((uint8_t *)list.data + b * list.item_size,
                           old_data + a * list.item_size,
                           list.item_size);

        /* Bytes between the two indices (if any) unchanged */
        if (max_idx > min_idx + 1) {
            assert_bytes_match((uint8_t *)list.data + (min_idx + 1) * list.item_size,
                               old_data + (min_idx + 1) * list.item_size,
                               (max_idx - min_idx - 1) * list.item_size);
        }

        /* Bytes after the last swapped element unchanged */
        assert_bytes_match((uint8_t *)list.data + (max_idx + 1) * list.item_size,
                           old_data + (max_idx + 1) * list.item_size,
                           (list.length - max_idx - 1) * list.item_size);
    }

    /* Validity must be preserved */
    assert(aws_array_list_is_valid(&list));
}
