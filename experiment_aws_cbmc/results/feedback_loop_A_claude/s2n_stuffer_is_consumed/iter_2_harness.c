#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Minimal s2n struct definitions needed for the harness */
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

bool s2n_stuffer_is_consumed(struct s2n_stuffer *stuffer);

/* Nondet helpers */
extern bool nondet_bool(void);
extern uint32_t nondet_uint32_t(void);

void s2n_stuffer_is_consumed_harness(void) {
    struct s2n_stuffer stuffer;

    /* Initialize fields non-deterministically */
    stuffer.blob.data = NULL;
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Save old state before calling */
    struct s2n_stuffer old = stuffer;

    /* Call function under test */
    bool result = s2n_stuffer_is_consumed(&stuffer);

    /* Assert fields are unchanged (function is read-only) */
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);
    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.read_cursor == old.read_cursor);
    assert(stuffer.write_cursor == old.write_cursor);
    assert(stuffer.high_water_mark == old.high_water_mark);
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);
    assert(stuffer.tainted == old.tainted);
}

void s2n_stuffer_is_consumed_harness(void) {
    s2n_stuffer_is_consumed_harness();
    return 0;
}
