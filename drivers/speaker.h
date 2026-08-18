
#ifndef SPEAKER_H
#define SPEAKER_H
#include <stdint.h>
void speaker_stop(void);
void speaker_request(uint32_t freq, uint32_t ms);
void speaker_ui_click(void);
void speaker_chime(void);
void speaker_service(void);
#endif

