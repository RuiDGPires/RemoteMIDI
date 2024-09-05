#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <portmidi/pm_common/portmidi.h>
#include <portmidi/pm_common/pmutil.h>
#include <portmidi/porttime/porttime.h>
#include "tcp.h"
#include "midi.h"

#define MIDI_SYSEX 0xf0
#define MIDI_EOX 0xf7

/* active is set true when midi processing should start */
int active = FALSE;
/* process_midi_exit_flag is set when the timer thread shuts down */
int process_midi_exit_flag;

PmStream *midi_out;

/* shared queues */
#define OUT_QUEUE_SIZE 1024
PmQueue *out_queue;
PmTimestamp current_timestamp = 0;
int thru_sysex_in_progress = FALSE;
int app_sysex_in_progress = FALSE;
PmTimestamp last_timestamp = 0;


/* time proc parameter for Pm_MidiOpen */
PmTimestamp midithru_time_proc(void *info)
{
    return current_timestamp;
}


/* timer interrupt for processing midi data.
   Outgoing data from main program is delivered via out_queue.
   Incoming data from midi_in is copied with low latency to  midi_out.
   Sysex messages from either source block messages from the other.
 */
void process_midi(PtTimestamp timestamp, void *userData)
{
    PmError result;
    PmEvent buffer; /* just one message at a time */

    current_timestamp++; /* update every millisecond */
    /* if (current_timestamp % 1000 == 0) 
        printf("time %d\n", current_timestamp); */

    /* do nothing until initialization completes */
    if (!active) {
        /* this flag signals that no more midi processing will be done */
        process_midi_exit_flag = TRUE;
        return;
    }

    /* see if there is application midi data to process */
    while (!Pm_QueueEmpty(out_queue)) {
        /* see if it is time to output the next message */
        PmEvent *next = (PmEvent *) Pm_QueuePeek(out_queue);
        assert(next); /* must be non-null because queue is not empty */
        if (next->timestamp <= current_timestamp) {
            /* time to send a message, first make sure it's not blocked */
            int status = Pm_MessageStatus(next->message);
            if ((status & 0xF8) == 0xF8) {
                ; /* real-time messages are not blocked */
            } else if (thru_sysex_in_progress) {
                /* maybe sysex has timed out (output becomes unblocked) */
                if (last_timestamp + 5000 < current_timestamp) {
                    thru_sysex_in_progress = FALSE;
                } else break; /* output is blocked, so exit loop */
            }
            Pm_Dequeue(out_queue, &buffer);
            Pm_Write(midi_out, &buffer, 1);

            /* inspect message to update app_sysex_in_progress */
            if (status == MIDI_SYSEX) app_sysex_in_progress = TRUE;
            else if ((status & 0xF8) != 0xF8) {
                /* not MIDI_SYSEX and not real-time, so */
                app_sysex_in_progress = FALSE;
            }
            if (app_sysex_in_progress && /* look for EOX */
                (((buffer.message & 0xFF) == MIDI_EOX) ||
                 (((buffer.message >> 8) & 0xFF) == MIDI_EOX) ||
                 (((buffer.message >> 16) & 0xFF) == MIDI_EOX) ||
                 (((buffer.message >> 24) & 0xFF) == MIDI_EOX))) {
                app_sysex_in_progress = FALSE;
            }
        } else break; /* wait until indicated timestamp */
    }
}

void midi_send(Message message) {
    PmEvent buffer;

    buffer.message = message;
    buffer.timestamp = current_timestamp;
    Pm_Enqueue(out_queue, &buffer);
}

void exit_with_message(char *msg) {
#define STRING_MAX 80
    char line[STRING_MAX];
    printf("%s\nType ENTER...", msg);
    fgets(line, STRING_MAX, stdin);
    exit(1);
}


void initialize(int output_device) {

    const PmDeviceInfo *info;
    int id;

    out_queue = Pm_QueueCreate(OUT_QUEUE_SIZE, sizeof(PmEvent));
    assert(out_queue != NULL);

    /* always start the timer before you start midi */
    Pt_Start(1, &process_midi, 0); /* start a timer with millisecond accuracy */
    /* the timer will call our function, process_midi() every millisecond */
    
    Pm_Initialize();

    id = output_device;
    info = Pm_GetDeviceInfo(id);
    if (info == NULL) {
        printf("Could not open default output device (%d).", id);
        exit_with_message("");
    }
    printf("Opening output device %s %s\n", info->interf, info->name);

    /* use zero latency because we want output to be immediate */
    Pm_OpenOutput(&midi_out, 
                  id, 
                  NULL /* driver info */,
                  OUT_QUEUE_SIZE,
                  &midithru_time_proc,
                  NULL /* time info */,
                  0 /* Latency */);

    active = TRUE; /* enable processing in the midi thread -- yes, this
                      is a shared variable without synchronization, but
                      this simple assignment is safe */
}


void finalize()
{
    printf("Finalizing MIDI processing\n");
    /* the timer thread could be in the middle of accessing PortMidi stuff */
    /* to detect that it is done, we first clear process_midi_exit_flag and
       then wait for the timer thread to set it
     */
    process_midi_exit_flag = FALSE;
    active = FALSE;
    /* busy wait for flag from timer thread that it is done */
    while (!process_midi_exit_flag) ;
    /* at this point, midi thread is inactive and we need to shut down
     * the midi input and output
     */
    Pt_Stop(); /* stop the timer */
    Pm_QueueDestroy(out_queue);

    Pm_Close(midi_out);

    Pm_Terminate();    
}

void midi_initialize(int output_device) {
    /* determine what type of test to run */
    printf("begin PortMidi midithru program...\n");

    initialize(output_device); /* set up and start midi processing */
}

void midi_finalize(void) {
    finalize();
}
