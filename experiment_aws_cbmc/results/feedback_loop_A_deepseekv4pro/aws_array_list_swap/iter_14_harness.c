#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_INITIAL_ITEM_ALLOCATION 2
#define MAX_ITEM_SIZE 8

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    /* Allocate data for the list */
    list.data = bounded_malloc(list.current_size * list.item_size);
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];

    if (list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE) {
        for (size_t i = 0; i < list.item_size; i++) {
            old_a[i] = ((uint8_t *)list.data)[a * list.item_size + i];
            old_b[i] = ((uint8_t *)list.data)[b * list.item_size + i];
        }
    }

    int ret = aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE) {
        if (ret == AWS_OP_SUCCESS) {
            for (size_t i = 0; i < list.item_size; i++) {
                assert(((uint8_t *)list.data)[a * list.item_size + i] == old_b[i]);
                assert(((uint8_t *)list.data)[b * list.item_size + i] == old_a[i]);
            }
        } else {
            for (size_t i = 0; i < list.item_size; i++) {
                assert(((uint8_t *)list.data)[a * list.item_size + i] == old_a[i]);
                assert(((uint8_t *)list.data)[b * list.item_size + i] == old_b[i]);
            }
        }
    }

    assert(aws_array_list_is_valid(&list));
}
