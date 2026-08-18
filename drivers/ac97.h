#ifndef AC97_H
#define AC97_H

int  ac97_init(void);
void ac97_play_loop(const unsigned char *data, unsigned int len);

#endif
