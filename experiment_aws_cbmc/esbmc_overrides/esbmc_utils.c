/**
 * ESBMC-compatible utils.c — minimal version for byte buffer proofs.
 *
 * Does NOT include proof_helpers/utils.h (which has CBMC-specific
 * __CPROVER_uninterpreted_hasher declarations that crash ESBMC).
 * Instead, only the struct and functions needed by byte_buf harnesses are
 * defined here, using __ESBMC_assume in place of __CPROVER_assume.
 */

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>

/* Redeclare nondet helpers (ESBMC treats undeclared externs as nondet) */
extern size_t nondet_size_t(void);

/* Store one byte from an array for later equivalence checking */
struct store_byte_from_buffer {
    size_t index;
    uint8_t byte;
};

#ifndef MAX_MALLOC
#  define MAX_MALLOC (SIZE_MAX >> 9)
#endif

void assert_bytes_match(const uint8_t *const a, const uint8_t *const b, const size_t len) {
    assert(len == 0 || !a == !b);
    if (len > 0 && a != NULL && b != NULL) {
        size_t i;
        __ESBMC_assume(i < len && len < MAX_MALLOC);
        assert(a[i] == b[i]);
    }
}

void assert_all_bytes_are(const uint8_t *const a, const uint8_t c, const size_t len) {
    if (len > 0 && a != NULL) {
        size_t i;
        __ESBMC_assume(i < len);
        assert(a[i] == c);
    }
}

void assert_all_zeroes(const uint8_t *const a, const size_t len) {
    assert_all_bytes_are(a, 0, len);
}

void assert_byte_from_buffer_matches(const uint8_t *const buffer, const struct store_byte_from_buffer *const b) {
    if (buffer && b) {
        assert(*(buffer + b->index) == b->byte);
    }
}

void save_byte_from_array(const uint8_t *const array, const size_t size, struct store_byte_from_buffer *const storage) {
    if (size > 0 && array && storage) {
        storage->index = nondet_size_t();
        __ESBMC_assume(storage->index < size);
        storage->byte = array[storage->index];
    }
}

void assert_byte_buf_equivalence(
    const struct aws_byte_buf *const lhs,
    const struct aws_byte_buf *const rhs,
    const struct store_byte_from_buffer *const rhs_byte) {
    if (lhs == rhs) {
        return;
    } else {
        assert(lhs && rhs);
    }
    assert(lhs->len == rhs->len);
    assert(lhs->capacity == rhs->capacity);
    assert(lhs->allocator == rhs->allocator);
    if (lhs->len > 0) {
        assert_byte_from_buffer_matches(lhs->buffer, rhs_byte);
    }
}

void assert_byte_cursor_equivalence(
    const struct aws_byte_cursor *const lhs,
    const struct aws_byte_cursor *const rhs,
    const struct store_byte_from_buffer *const rhs_byte) {
    assert(!lhs == !rhs);
    if (lhs && rhs) {
        assert(lhs->len == rhs->len);
        if (lhs->len > 0) {
            assert_byte_from_buffer_matches(lhs->ptr, rhs_byte);
        }
    }
}

void assert_array_list_equivalence(
    const struct aws_array_list *const lhs,
    const struct aws_array_list *const rhs,
    const struct store_byte_from_buffer *const rhs_byte) {
    if (lhs == rhs) {
        return;
    } else {
        assert(lhs && rhs);
    }
    assert(lhs->alloc == rhs->alloc);
    assert(lhs->current_size == rhs->current_size);
    assert(lhs->length == rhs->length);
    assert(lhs->item_size == rhs->item_size);
    if (lhs->current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)lhs->data, rhs_byte);
    }
}
