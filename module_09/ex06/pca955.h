#ifndef PCA955_H
#define PCA955_H
#include "bits.h"
#include "twi.h"

#include <stdint.h>
#define PCA955_RAW_SLAVE_ADDR (0x40 >> 1)

typedef enum PCA955_COMMANND
{
    PCA955_COMMANND_READ_IP0   = 0,
    PCA955_COMMANND_READ_IP1   = 1,
    PCA955_COMMANND_WRITE_OP0  = 2,
    PCA955_COMMANND_WRITE_OP1  = 3,
    PCA955_COMMANND_POL_INV_P0 = 4,
    PCA955_COMMANND_POL_INV_P6 = 5,
    PCA955_COMMANND_CONF_P0    = 6,
    PCA955_COMMANND_CONF_P1    = 7,
} PCA955_COMMANND;

static inline void pca955_start_read()
{
    twi_start();
    twi_set_read_address(PCA955_RAW_SLAVE_ADDR);
}

static inline void pca955_start_write()
{
    twi_start();
    twi_set_write_address(PCA955_RAW_SLAVE_ADDR);
}

#define PCA955_GEN_ROUTINE_PORT(PORT)                                                                                                                                                                  \
    static inline uint8_t pca955_read_p##PORT##_pin_state()                                                                                                                                            \
    {                                                                                                                                                                                                  \
        pca955_start_write();                                                                                                                                                                          \
        twi_write_data(PCA955_COMMANND_READ_IP##PORT);                                                                                                                                                 \
        pca955_start_read();                                                                                                                                                                           \
        uint8_t p;                                                                                                                                                                                     \
        twi_read_byte_nack(&p);                                                                                                                                                                        \
        return p;                                                                                                                                                                                      \
    }                                                                                                                                                                                                  \
    static inline uint8_t pca955_read_p##PORT##_pin_cfg()                                                                                                                                              \
    {                                                                                                                                                                                                  \
        pca955_start_write();                                                                                                                                                                          \
        twi_write_data(PCA955_COMMANND_CONF_P##PORT);                                                                                                                                                  \
        pca955_start_read();                                                                                                                                                                           \
        uint8_t p;                                                                                                                                                                                     \
        twi_read_byte_nack(&p);                                                                                                                                                                        \
        return p;                                                                                                                                                                                      \
    }                                                                                                                                                                                                  \
    static inline uint8_t pca955_read_p##PORT##_output()                                                                                                                                               \
    {                                                                                                                                                                                                  \
        pca955_start_write();                                                                                                                                                                          \
        twi_write_data(PCA955_COMMANND_WRITE_OP##PORT);                                                                                                                                                \
        pca955_start_read();                                                                                                                                                                           \
        uint8_t p;                                                                                                                                                                                     \
        twi_read_byte_nack(&p);                                                                                                                                                                        \
        return p;                                                                                                                                                                                      \
    }                                                                                                                                                                                                  \
    static inline void pca955_set_p##PORT##_pin_state(uint8_t p##PORT)                                                                                                                                 \
    {                                                                                                                                                                                                  \
        pca955_start_write();                                                                                                                                                                          \
        twi_write_data(PCA955_COMMANND_WRITE_OP##PORT);                                                                                                                                                \
        twi_write_data(p##PORT);                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    static inline void pca955_set_p##PORT##_pin_cfg(uint8_t p##PORT)                                                                                                                                   \
    {                                                                                                                                                                                                  \
        pca955_start_write();                                                                                                                                                                          \
        twi_write_data(PCA955_COMMANND_CONF_P##PORT);                                                                                                                                                  \
        twi_write_data(p##PORT);                                                                                                                                                                       \
    }

PCA955_GEN_ROUTINE_PORT(0)
PCA955_GEN_ROUTINE_PORT(1)

#undef PCA955_GEN_ROUTINE_READ_PORT

#define PCA955_GEN_ROUTINE_PIN(PORT, PIN)                                                                                                                                                              \
    static inline void pca955_set_port##PORT##_##PIN##_state_high()                                                                                                                                    \
    {                                                                                                                                                                                                  \
        uint8_t p##PORT = pca955_read_p##PORT##_output();                                                                                                                                              \
        pca955_set_p##PORT##_pin_state(bits_clear(p##PORT, PIN));                                                                                                                                      \
    }                                                                                                                                                                                                  \
    static inline void pca955_set_port##PORT##_##PIN##_state_low()                                                                                                                                     \
    {                                                                                                                                                                                                  \
        uint8_t p##PORT = pca955_read_p##PORT##_output();                                                                                                                                              \
        pca955_set_p##PORT##_pin_state(bits_set(p##PORT, PIN));                                                                                                                                        \
    }                                                                                                                                                                                                  \
    static inline void pca955_set_port##PORT##_##PIN##_as_input()                                                                                                                                      \
    {                                                                                                                                                                                                  \
        uint8_t p##PORT = pca955_read_p##PORT##_pin_cfg();                                                                                                                                             \
        pca955_set_p##PORT##_pin_cfg(bits_set(p##PORT, PIN));                                                                                                                                          \
    }                                                                                                                                                                                                  \
    static inline void pca955_set_port##PORT##_##PIN##_as_output()                                                                                                                                     \
    {                                                                                                                                                                                                  \
        uint8_t p##PORT = pca955_read_p##PORT##_pin_cfg();                                                                                                                                             \
        pca955_set_p##PORT##_pin_cfg(bits_clear(p##PORT, PIN));                                                                                                                                        \
    }                                                                                                                                                                                                  \
    static inline uint8_t pca955_read_port##PORT##_##PIN()                                                                                                                                             \
    {                                                                                                                                                                                                  \
        uint8_t p##PORT = pca955_read_p##PORT##_pin_state();                                                                                                                                           \
        return bits_is_set(p##PORT, PIN) >> PIN;                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    static inline void pca955_toggle_port##PORT##_##PIN()                                                                                                                                              \
    {                                                                                                                                                                                                  \
        uint8_t p##PORT = pca955_read_p##PORT##_output();                                                                                                                                              \
        pca955_set_p##PORT##_pin_state(bits_toggle(p##PORT, PIN));                                                                                                                                     \
    }

PCA955_GEN_ROUTINE_PIN(0, 0)
PCA955_GEN_ROUTINE_PIN(0, 1)
PCA955_GEN_ROUTINE_PIN(0, 2)
PCA955_GEN_ROUTINE_PIN(0, 3)
PCA955_GEN_ROUTINE_PIN(0, 4)
PCA955_GEN_ROUTINE_PIN(0, 5)
PCA955_GEN_ROUTINE_PIN(0, 6)
PCA955_GEN_ROUTINE_PIN(0, 7)

PCA955_GEN_ROUTINE_PIN(1, 0)
PCA955_GEN_ROUTINE_PIN(1, 1)
PCA955_GEN_ROUTINE_PIN(1, 2)
PCA955_GEN_ROUTINE_PIN(1, 3)
PCA955_GEN_ROUTINE_PIN(1, 4)
PCA955_GEN_ROUTINE_PIN(1, 5)
PCA955_GEN_ROUTINE_PIN(1, 6)
PCA955_GEN_ROUTINE_PIN(1, 7)

#undef PCA955_GEN_ROUTINE_PIN

#endif