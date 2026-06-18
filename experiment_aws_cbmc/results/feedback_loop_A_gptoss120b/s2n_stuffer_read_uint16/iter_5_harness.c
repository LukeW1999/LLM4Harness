#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/* Nondeterministic helpers */
extern uint32_t nondet_uint32_t(void);
extern uint8_t nondet_uint8_t(void);
extern _Bool nondet_bool(void);

/* AWS allocator stub */
struct aws_allocator {
    void *(*malloc)(struct aws_allocator *, size_t);
    void (*release)(struct aws_allocator *, void *);
};

static void *aws_default_malloc(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    return malloc(size);
}

static void aws_default_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    free(ptr);
}

static struct aws_allocator default_allocator = {
    aws_default_malloc,
    aws_default_release
};

static struct aws_allocator *aws_default_allocator(void) {
    return &default_allocator;
}

/* Constants (normally provided by the Makefile) */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

/* s2n structures (copied from the library header) */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* Return codes used by s2n */
#define S2N_SUCCESS 0
#define S2N_ERR    -1

/* Stub for s2n_stuffer_read_bytes – models the real behaviour needed for the proof */
int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer, uint8_t *buf, uint32_t len)
{
    if (stuffer == NULL || buf == NULL) {
        return S2N_ERR;
    }
    if (stuffer->read_cursor + len > stuffer->blob.size) {
        return S2N_ERR;
    }
    for (uint32_t i = 0; i < len; ++i) {
        buf[i] = stuffer->blob.data[stuffer->read_cursor + i];
    }
    stuffer->read_cursor += len;
    return S2N_SUCCESS;
}

/* Declaration of the function under test */
int s2n_stuffer_read_uint16(struct s2n_stuffer *stuffer, uint16_t *u);

/* Harness */
void s2n_stuffer_read_uint16_harness(void)
{
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and nondeterministically initialise a stuffer */
    struct s2n_stuffer stuffer;
    struct s2n_blob *blob = &stuffer.blob;

    /* nondet size for the underlying buffer, bounded */
    blob->size = nondet_uint32_t();
    __CPROVER_assume(blob->size <= MAX_BUFFER_SIZE);

    /* allocate the buffer */
    if (blob->size > 0) {
        blob->data = (uint8_t *)allocator->malloc(allocator, blob->size);
        __CPROVER_assume(blob->data != NULL);
        /* fill with nondet bytes */
        for (uint32_t i = 0; i < blob->size; ++i) {
            blob->data[i] = nondet_uint8_t();
        }
    } else {
        blob->data = NULL;
    }

    /* other blob fields */
    blob->allocated = nondet_uint32_t();
    __CPROVER_assume(blob->allocated >= blob->size);
    blob->growable = nondet_bool();

    /* stuffer cursors – must be within the blob */
    stuffer.read_cursor  = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    __CPROVER_assume(stuffer.read_cursor  <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= blob->size);
    __CPROVER_assume(stuffer.high_water_mark <= blob->size);

    /* other stuffer flags */
    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* 2. Allocate output pointer and give it a nondet initial value */
    uint16_t *u = (uint16_t *)allocator->malloc(allocator, sizeof(uint16_t));
    __CPROVER_assume(u != NULL);
    uint16_t old_u = nondet_uint16_t();
    *u = old_u;

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;
    uint16_t old_u_copy = old_u;

    uint8_t old_byte0 = 0, old_byte1 = 0;
    bool have_two_bytes = false;
    if (old.read_cursor + 1 < blob->size) {
        old_byte0 = blob->data[old.read_cursor];
        old_byte1 = blob->data[old.read_cursor + 1];
        have_two_bytes = true;
    }

    /* 4. Call the function under test */
    int result = s2n_stuffer_read_uint16(&stuffer, u);

    /* 5. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* success: two bytes must have been available */
        assert(have_two_bytes);

        /* cursors */
        assert(stuffer.read_cursor == old.read_cursor + 2);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);

        /* output value */
        uint16_t expected = ((uint16_t)old_byte0 << 8) | (uint16_t)old_byte1;
        assert(*u == expected);
    } else {
        /* failure: state must be unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(*u == old_u_copy);
    }

    /* 6. Fields that must never change */
    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);

    assert(stuffer.alloced   == old.alloced);
    assert(stuffer.growable  == old.growable);
    assert(stuffer.tainted   == old.tainted);

    /* 7. Basic validity invariants */
    assert(stuffer.read_cursor  <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
