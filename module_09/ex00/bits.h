#ifndef BITS_H
#define BITS_H

#define bits_clear(value, bit) (value & ~(1 << bit))
#define bits_toggle(value, bit) (value ^ (1 << bit))
#define bits_set(value, bit) (value | (1 << bit))
#define bits_is_set(value, bit) (value & (1 << bit))

#endif