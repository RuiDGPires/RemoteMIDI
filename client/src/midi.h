#ifndef __MIDI_H__
#define __MIDI_H__

#include <pm_common/portmidi.h>

typedef void (*MidiProcess)(PmEvent, void*);

typedef struct {
    int midi_input_device_id;
    MidiProcess midi_process;
    void *connection_context;
} midi_context_t;

void midi_process_main(midi_context_t*);
void midi_finalize(void);

#endif
