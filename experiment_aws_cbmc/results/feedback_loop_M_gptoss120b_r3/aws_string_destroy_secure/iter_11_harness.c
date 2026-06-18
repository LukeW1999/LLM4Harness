#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

/* Dummy allocator that does nothing on release */
static void *dummy_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    (void)size;
    return NULL;
}
static void dummy_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    (void)ptr;
}
static void *dummy_mem_realloc(struct aws_allocator *allocator, void *ptr, size_t oldsize, size_t newsize) {
    (void)allocator;
    (void)ptr;
    (void)oldsize;
    (void)newsize;
    return NULL;
}
static void *dummy_mem_calloc(struct aws_allocator *allocator, size_t num, size_t size) {
    (void)allocator;
    (void)num;
    (void)size;
    return NULL;
}
static size_t dummy_mem_page_size(struct aws_allocator *allocator) {
    (void)allocator;
    return 0;
}
static struct aws_allocator dummy_allocator = {
    dummy_mem_acquire,
    dummy_mem_release,
    dummy_mem_realloc,
    dummy_mem_calloc,
    dummy_mem_page_size,
    NULL,
    NULL
};

void aws_string_destroy_secure_harness(void) {
    _Bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024);

        struct {
            struct aws_string s;
            uint8_t bytes[1024];
        } storage;

        str = &storage.s;
        str->allocator = &dummy_allocator;
        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* Preserve original bytes for later comparison */
        uint8_t old_bytes[1024];
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = bytes[i];
        }

        aws_string_destroy_secure(str);

        /* After secure destroy, the string's metadata should remain unchanged */
        assert(str->len == len);
        assert(str->allocator == &dummy_allocator);
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        aws_string_destroy_secure(str);
        assert(1);
    }
}
