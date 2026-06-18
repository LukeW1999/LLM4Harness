#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t nondet_uint64_t(void);

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.length > 0);
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    uint8_t *old_a = malloc(list.item_size);
    uint8_t *old_b = malloc(list.item_size);
    __CPROVER_assume(old_a != NULL);
    __CPROVER_assume(old_b != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        old_a[i] = ((uint8_t *)list.data)[a * list.item_size + i];
        old_b[i] = ((uint8_t *)list.data)[b * list.item_size + i];
    }

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == 0);

    void *ptr_a = NULL;
    void *ptr_b = NULL;
    aws_array_list_get_at_ptr(&list, &ptr_a, a);
    aws_array_list_get_at_ptr(&list, &ptr_b, b);
    if (a == b) {
        assert_bytes_match((const uint8_t *)ptr_a, old_a, list.item_size);
        assert_bytes_match((const uint8_t *)ptr_b, old_b, list.item_size);
    } else {
        assert_bytes_match((const uint8_t *)ptr_a, old_b, list.item_size);
        assert_bytes_match((const uint8_t *)ptr_b, old_a, list.item_size);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));

    free(old_a);
    free(old_b);
}
