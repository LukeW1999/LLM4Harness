#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    list.alloc = alloc;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    __CPROVER_assume(a <= SIZE_MAX / list.item_size);
    __CPROVER_assume(b <= SIZE_MAX / list.item_size);

    struct aws_array_list old = list;

    uint8_t *old_a = NULL;
    uint8_t *old_b = NULL;
    if (list.item_size > 0) {
        old_a = malloc(list.item_size);
        old_b = malloc(list.item_size);
        __CPROVER_assume(old_a);
        __CPROVER_assume(old_b);
        memcpy(old_a, (uint8_t *)list.data + a * list.item_size, list.item_size);
        memcpy(old_b, (uint8_t *)list.data + b * list.item_size, list.item_size);
    }

    int ret = aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (list.item_size > 0) {
        if (ret == AWS_OP_SUCCESS) {
            size_t i;
            for (i = 0; i < list.item_size; i++) {
                assert(((uint8_t *)list.data)[a * list.item_size + i] == old_b[i]);
                assert(((uint8_t *)list.data)[b * list.item_size + i] == old_a[i]);
            }
        } else {
            size_t i;
            for (i = 0; i < list.item_size; i++) {
                assert(((uint8_t *)list.data)[a * list.item_size + i] == old_a[i]);
                assert(((uint8_t *)list.data)[b * list.item_size + i] == old_b[i]);
            }
        }
    }

    assert(aws_array_list_is_valid(&list));

    if (old_a) free(old_a);
    if (old_b) free(old_b);
}
