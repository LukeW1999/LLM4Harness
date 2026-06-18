#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_swap_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t len = aws_array_list_length(&list);
    __CPROVER_assume(len > 0);
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    memcpy(old_a, (uint8_t *)list.data + (a * list.item_size), list.item_size);
    memcpy(old_b, (uint8_t *)list.data + (b * list.item_size), list.item_size);

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);

    assert_bytes_match((uint8_t *)list.data + (a * list.item_size), old_b, list.item_size);
    assert_bytes_match((uint8_t *)list.data + (b * list.item_size), old_a, list.item_size);

    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                               (uint8_t *)old.data + (i * old.item_size),
                               list.item_size);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
