#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;

    /* Use ensure_array_list_is_allocated or just set up manually */
    /* Simple setup: nondeterministic length */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 128);

    size_t length;
    __CPROVER_assume(length <= 16);

    size_t current_size;
    __CPROVER_assume(current_size >= length * item_size);
    __CPROVER_assume(current_size <= 2048);

    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;

    if (current_size == 0) {
        list.data = NULL;
    } else {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    }

    list.alloc = aws_default_allocator();

    /* Snapshot state before call */
    size_t old_length       = list.length;
    size_t old_item_size    = list.item_size;
    size_t old_current_size = list.current_size;
    void  *old_data         = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* Postcondition: return value equals list->length */
    assert(result == old_length);

    /* Postcondition: frame — no fields modified */
    assert(list.length       == old_length);
    assert(list.item_size    == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);
}
