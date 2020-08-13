#define CHANNEL_AMOUNT 8
#define CHANNEL_SIZE (4096)
#define CHANNEL_RING_INC(X) (((X+1)<CHANNEL_SIZE)?(X+1):0)

// used as ring buffers
struct r_buffer {
	int read;
	int write;
	char b[CHANNEL_SIZE];
};

struct r_buffer channel[CHANNEL_AMOUNT];

int open_channel(int id) {
	if (id >= CHANNEL_AMOUNT) return;
	for (int i = 0; i < CHANNEL_SIZE; i++) {
		channel[i].b = 0;
		channel[i].read = 0;
		channel[i].write = 0;
	}
}

int msg_fits(struct r_buffer *buf, uint32_t length) {
	if (buf->write + 

int send_channel(int id, uint8_t *data, uint32_t length) {
	if (id >= CHANNEL_AMOUNT || length < 1) return -1;
	if (msg_fits(&channel[id], length)) {
		
		return legnth;
	} else {
		return 0;
	}
}

int read_channel(int id, uint8_t *data, uint32_t length);
