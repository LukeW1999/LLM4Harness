#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

size_t nondet_size_t(void);

bool aws_array_list_is_bounded(const struct aws_array_list *list,
                               size_t max_items,
                               size_t max_item_size);
void ensure_array_list_has_allocated_data_member(struct aws_array_list *list);
void save_byte_from_array(const void *array, size_t len,
                          struct store_byte_from_buffer *store);
void assert_byte_from_buffer_matches(const void *array,
                                     const struct store_byte_from_buffer *store);

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    int result = aws_array_list_clear(&list);

    assert(aws_array_list_is_valid(&list));
    assert(result == AWS_OP_SUCCESS);
    assert(list.length == 0);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (list.data) {
        assert_byte_from_buffer_matches(list.data, &old_byte);
    }
}
