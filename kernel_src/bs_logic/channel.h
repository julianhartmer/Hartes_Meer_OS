#ifndef CHANNEL_H
#define CHANNEL_H

int open_channel(int id);
int send_channel(int id, uint8_t *data, uint32_t length);
int read_channel(int id, uint8_t *data, uint32_t length);

#endif
