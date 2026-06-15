#include <aws/common/common.h>
#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    void *a = (void *)nondet_uint64_t();
    void *b = (void *)nondet_uint64_t();

    bool result = aws_ptr_eq(a, b);

    assert(result == (a == b));
}

void aws_hash_table_init_harness() {
    struct aws_hash_table map;
    struct aws_allocator *alloc = &aws_default_allocator();
    size_t size = nondet_size_t();
    aws_hash_fn *hash_fn = (aws_hash_fn *)nondet_uint64_t();
    aws_hash_callback_eq_fn *equals_fn = (aws_hash_callback_eq_fn *)nondet_uint64_t();
    aws_hash_callback_destroy_fn *destroy_key_fn = (aws_hash_callback_destroy_fn *)nondet_uint64_t();
    aws_hash_callback_destroy_fn *destroy_value_fn = (aws_hash_callback_destroy_fn *)nondet_uint64_t();

    __CPROVER_assume(size > 0);
    __CPROVER_assume(hash_fn != NULL);
    __CPROVER_assume(equals_fn != NULL);

    int result = aws_hash_table_init(&map, alloc, size, hash_fn, equals_fn, destroy_key_fn, destroy_value_fn);

    if (result == AWS_OP_SUCCESS) {
        assert(map.p_impl != NULL);
    } else {
        assert(map.p_impl == NULL);
    }

    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_clean_up_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    aws_hash_table_clean_up(&map);

    assert(map.p_impl == NULL);
    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_swap_harness() {
    struct aws_hash_table a;
    struct aws_hash_table b;
    ensure_hash_table_has_allocated_data_member(&a);
    ensure_hash_table_has_allocated_data_member(&b);
    __CPROVER_assume(aws_hash_table_is_valid(&a));
    __CPROVER_assume(aws_hash_table_is_valid(&b));

    struct aws_hash_table old_a = a;
    struct aws_hash_table old_b = b;

    aws_hash_table_swap(&a, &b);

    assert(a.p_impl == old_b.p_impl);
    assert(b.p_impl == old_a.p_impl);
    assert(aws_hash_table_is_valid(&a));
    assert(aws_hash_table_is_valid(&b));
}

void aws_hash_table_move_harness() {
    struct aws_hash_table to;
    struct aws_hash_table from;
    ensure_hash_table_has_allocated_data_member(&to);
    ensure_hash_table_has_allocated_data_member(&from);
    __CPROVER_assume(aws_hash_table_is_valid(&to));
    __CPROVER_assume(aws_hash_table_is_valid(&from));

    struct aws_hash_table old_to = to;
    struct aws_hash_table old_from = from;

    aws_hash_table_move(&to, &from);

    assert(to.p_impl == old_from.p_impl);
    assert(from.p_impl == old_to.p_impl);
    assert(aws_hash_table_is_valid(&to));
    assert(aws_hash_table_is_valid(&from));
}

void aws_hash_table_get_entry_count_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    size_t count = aws_hash_table_get_entry_count(&map);

    assert(count >= 0);
    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_iter_begin_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    struct aws_hash_iter iter = aws_hash_iter_begin(&map);

    assert(aws_hash_iter_is_valid(&iter));
    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_iter_done_harness() {
    struct aws_hash_iter iter;
    ensure_hash_iter_has_allocated_data_member(&iter);
    __CPROVER_assume(aws_hash_iter_is_valid(&iter));

    bool done = aws_hash_iter_done(&iter);

    assert(done == (iter.status == AWS_HASH_ITER_STATUS_DONE));
    assert(aws_hash_iter_is_valid(&iter));
}

void aws_hash_iter_next_harness() {
    struct aws_hash_iter iter;
    ensure_hash_iter_has_allocated_data_member(&iter);
    __CPROVER_assume(aws_hash_iter_is_valid(&iter));

    aws_hash_iter_next(&iter);

    assert(aws_hash_iter_is_valid(&iter));
}

void aws_hash_iter_delete_harness() {
    struct aws_hash_iter iter;
    ensure_hash_iter_has_allocated_data_member(&iter);
    __CPROVER_assume(aws_hash_iter_is_valid(&iter));

    bool destroy_contents = nondet_bool();

    aws_hash_iter_delete(&iter, destroy_contents);

    assert(aws_hash_iter_is_valid(&iter));
}

void aws_hash_table_find_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    const void *key = (const void *)nondet_uint64_t();
    struct aws_hash_element *p_elem = (struct aws_hash_element *)nondet_uint64_t();

    int result = aws_hash_table_find(&map, key, &p_elem);

    if (result == AWS_OP_SUCCESS) {
        assert(p_elem != NULL);
    } else {
        assert(p_elem == NULL);
    }

    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_create_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    const void *key = (const void *)nondet_uint64_t();
    struct aws_hash_element *p_elem = (struct aws_hash_element *)nondet_uint64_t();
    int *was_created = (int *)nondet_uint64_t();

    int result = aws_hash_table_create(&map, key, &p_elem, was_created);

    if (result == AWS_OP_SUCCESS) {
        assert(p_elem != NULL);
    } else {
        assert(p_elem == NULL);
    }

    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_put_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    const void *key = (const void *)nondet_uint64_t();
    void *value = (void *)nondet_uint64_t();
    int *was_created = (int *)nondet_uint64_t();

    int result = aws_hash_table_put(&map, key, value, was_created);

    if (result == AWS_OP_SUCCESS) {
        assert(was_created != NULL);
    } else {
        assert(was_created == NULL);
    }

    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_remove_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    const void *key = (const void *)nondet_uint64_t();
    struct aws_hash_element *p_value = (struct aws_hash_element *)nondet_uint64_t();
    int *was_present = (int *)nondet_uint64_t();

    int result = aws_hash_table_remove(&map, key, p_value, was_present);

    assert(result == AWS_OP_SUCCESS);
    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_remove_element_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    struct aws_hash_element *p_value = (struct aws_hash_element *)nondet_uint64_t();

    int result = aws_hash_table_remove_element(&map, p_value);

    assert(result == AWS_OP_SUCCESS);
    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_foreach_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    int (*callback)(void *, struct aws_hash_element *) = (int (*)(void *, struct aws_hash_element *))nondet_uint64_t();
    void *context = (void *)nondet_uint64_t();

    int result = aws_hash_table_foreach(&map, callback, context);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(aws_hash_table_is_valid(&map));
}

void aws_hash_table_eq_harness() {
    struct aws_hash_table a;
    struct aws_hash_table b;
    ensure_hash_table_has_allocated_data_member(&a);
    ensure_hash_table_has_allocated_data_member(&b);
    __CPROVER_assume(aws_hash_table_is_valid(&a));
    __CPROVER_assume(aws_hash_table_is_valid(&b));

    aws_hash_callback_eq_fn *value_eq = (aws_hash_callback_eq_fn *)nondet_uint64_t();

    bool result = aws_hash_table_eq(&a, &b, value_eq);

    assert(result == true || result == false);
    assert(aws_hash_table_is_valid(&a));
    assert(aws_hash_table_is_valid(&b));
}

void aws_hash_table_clear_harness() {
    struct aws_hash_table map;
    ensure_hash_table_has_allocated_data_member(&map);
    __CPROVER_assume(aws_hash_table_is_valid(&map));

    aws_hash_table_clear(&map);

    assert(aws_hash_table_is_valid(&map));
}
