#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 64

/* Nondeterministic helpers */
uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);

/* Assertion helper */
void assert_bytes_match(const uint8_t *a, const uint8_t *b, size_t len);

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t item_sz = list.item_size;
    uint8_t *val = (uint8_t *)malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    size_t index = nondet_size_t();
    if (item_sz != 0) {
        size_t max_index = (size_t)-1 / item_sz;
        __CPROVER_assume(index <= max_index);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");

        if (index >= old.length) {
            __CPROVER_assert(list.length == index + 1, "length updated");
        } else {
            __CPROVER_assert(list.length == old.length, "length unchanged");
        }

        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (uint8_t *)val,
                           list.item_size);

        for (size_t i = 0; i < old.length; ++i) {
            if (i != index) {
                assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                                   (uint8_t *)old.data + (i * old.item_size),
                                   list.item_size);
            }
        }
    } else {
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged on failure");
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data pointer unchanged on failure");
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               list.current_size);
        }
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    free(val);
}
