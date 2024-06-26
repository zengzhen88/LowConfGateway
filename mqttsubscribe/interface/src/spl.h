#ifndef __SPL_MBLE_H__
#define __SPL_MBLE_H__ 


typedef void (*mble_data_receive_callback_t)(void *priv, void *mble_data);

void *mble_data_loop_run(void *args);
void mble_data_receive_callback_set(mble_data_receive_callback_t mcallback);
void mble_data_init(char *spi_dev, uint16_t irq_gpio_num);

#endif /*__SPL_MBLE_H__*/
