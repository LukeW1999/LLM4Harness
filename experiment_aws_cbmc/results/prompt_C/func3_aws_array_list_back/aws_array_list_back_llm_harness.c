#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <cbmc/model_assert.h>

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1
#define AWS_ERROR_LIST_EMPTY 1

struct aws_allocator {
    // Dummy allocator structure for CBMC
};

struct aws_array_list {
    struct aws_allocator *alloc;
    size_t current_size;
    size_t length;
    size_t item_size;
    void *data;
};

int aws_array_list_back(const struct aws_array_list *AWS_RESTRICT list, void *val);

void ensure_aws_array_list_has_allocated_data_member(struct aws_array_list *list) {
    if (list->current_size > 0) {
        list->data = malloc(list->current_size);
        __CPROVER_assume(list->data != NULL);
    }
}

void save_byte_from_array(const void *array, size_t size, unsigned char *store) {
    if (size > 0) {
        *store = ((unsigned char *)array)[0];
    }
}

void assert_bytes_match(const void *array, size_t size, const unsigned char *expected) {
    if (size > 0) {
        assert(((unsigned char *)array)[0] == *expected);
    }
}

void aws_array_list_back_harness() {
    /* 1. Declare data structures */
    struct aws_array_list list;
    unsigned char val[list.item_size];

    /* 2. Bound and allocate structures */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length <= SIZE_MAX / list.item_size);
    list.current_size = list.length * list.item_size;
    ensure_aws_array_list_has_allocated_data_member(&list);

    /* 3. Assume preconditions */
    __CPROVER_assume(list.alloc != NULL);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state if needed */
    unsigned char old_val;
    if (list.length > 0) {
        save_byte_from_array(list.data, list.item_size, &old_val);
    }

    /* 5. Call function under verification */
    int result = aws_array_list_back(&list, val);

    /* 6. Assert postconditions (both success and failure branches) */
    if (list.length > 0) {
        assert(result == AWS_OP_SUCCESS);
        assert_bytes_match(val, list.item_size, &old_val);
    } else {
        assert(result == AWS_OP_ERR);
    }

    /* 7. Assert invariants (validity predicates) */
    assert(list != NULL);
    assert(list.item_size > 0);
    assert((list.current_size == 0) || (list.data != NULL));
    assert(list.current_size == list.item_size * list.length);
    assert(list.length * list.item_size <= list.current_size);
}
