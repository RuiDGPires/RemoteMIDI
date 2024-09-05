#ifndef __MIDI_H__
#define __MIDI_H__

#include <portmidi/pm_common/portmidi.h>

typedef int32_t Message;
typedef void (*MidiProcess)(PmEvent, void*);

typedef struct {
    int midi_input_device_id;
    MidiProcess midi_process;
    void *connection_context;
} midi_context_t;

void midi_initialize(int output_device);
void midi_send(Message);
void midi_finalize(void);

#endif