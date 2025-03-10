/*  deviceman.h  - The device manager, that hides the use of midiOut 
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef DEVICEMAN_H
#define DEVICEMAN_H

#include "dattypes.h"
#include "../version.h"

class MidiOut;
class MidiMapper;

/**
 * MIDI Device Manager class . This class is the one you should use to
 * send MIDI events to any device, as it creates and manages the *Out classes.
 *
 * This class is usually used by creating a DeviceManager object, then call
 * @ref #openDev and @ref #initDev . Then, use @ref #numberOfMidiPorts,
 * @ref #numberOfSynthDevices, @ref #name and @ref #type to choose which
 * device to play MIDI events to and then use @ref defaultDevice to set the
 * MIDI device to play.
 *
 * @short Manages all MIDI devices and redirects MIDI events to each one as 
 * configured. 
 * @author Antonio Larrosa <larrosa@kde.org>
 */
class DeviceManager
{
  protected:

    MidiOut **device; 	// The midi devices objects
    struct midi_info *midiinfo;   // Midi info
    struct synth_info *synthinfo; // Synth info
    int chn2dev[16];    // Stores the device thru which a channel will be sent
    int n_synths;  // Number of synths devices
    int n_midi;  // Number of midi ports
    int n_total; // n_midi+n_synths

#ifndef HANDLETIMEINDEVICES

    int			rate;
    double		convertrate; // A "constant" used to convert from
    // milliseconds to the computer rate

#endif
    int timerstarted; // Newest kernels don't want me to stop a timer that
    		// hasn't been started :-)

    double lastwaittime; // Last time waited for in wait(double)

    MidiMapper *mapper_tmp; // Keeps a pointer to the mapper so that if devices
		// weren't initialized when first called setMidiMap
		// then, when they get initialized, they use the proper mapper

    int initialized;
    int seqfd;       // The real file handler for /dev/sequencer,
		// that is opened and closed.
    int default_dev; // The device to which timer events will be sent

    int _ok;
    void seqbuf_dump (void);
    void seqbuf_clean (void);

  public:
/**
 * Constructor. It just initializes internal variables, before playing any
 * music, you should call @ref #initManager, @ref #setMidiMap (optional), 
 * @ref openDev, @ref initDev, @ref setPatchesToUse (optional, except if
 * you're playing to a GUS device, which must load the patches),
 * @ref tmrStart, and finally, play the music.
 */
    DeviceManager(int def=0);
/**
 * Destructor. It closes the device (calling @ref closeDev) if it wasn't
 * closed before. 
 */
    ~DeviceManager(void);

/**
 * Initializes the MIDI Device Manager object.
 * 
 * The /dev/sequencer file is opened,  available devices are analyzed and
 * *Out objects are created. Then, the /dev/sequencer file is closed.
 *
 * @return 0 if everything was OK, or -1 if there was an error and it couldn't
 * be initialized (for example, because it couldn't open the /dev/sequencer i
 * file)
 */
    int initManager(void); 

/**
 * Checks if the device manager has been initialized (with @initManager),
 * and in case it wasn't, initializes it.
 * 
 * @return 0 if it was (or has just been) correctly initialized, and -1 if there
 * was an error.
 */
    int checkInit(void); 

/**
 * It's possible to send different MIDI channels to different MIDI devices,
 * so that you can for example send channel 1 to an external synthesizer,
 * channel 2 to a FM device and channel 10 to an AWE synth.
 *
 * @return the device to which MIDI events goind to channel chn should be sent. 
 */
    MidiOut *chntodev(int chn) { return device[chn2dev[chn]]; };

/**
 * @return 0 if there was a problem and 1 if everything was OK. Note that the
 * return value is changed after you check it, so you can only check it once.
 */
    int ok(void);

// The following funtion are here to emulate a midi, so that the DeviceManager
// sends the events to the appropiate devices.

/**
 * Open the devices. It first initializes the manager it that wasn't done yet
 * (you should do it yourself, to be able to choose the MIDI output device, as
 * it will be set to an external synth by default, if available) .
 *
 * Then /dev/sequencer is opened and the MIDI devices are opened 
 * (calling @ref MidiOut::openDev ).
 * @see #ok to check if there was any problem
 * @see #closeDev
 * @see #initDev
 */
    void openDev        (void);

/**
 * Closes the devices, and /dev/sequencer. 
 *
 * @see #openDev
 */
    void closeDev       (void);

/**
 * Calls @ref MidiOut::initDev in turn in each of the available devices.
 *
 * @see MidiOut::initDev
 */
    void initDev        (void);

/**
 * Sends a Note On MIDI event.
 *
 * @param chn the MIDI channel (0 to 15) to play the note on.
 * @param note the key of the note to play (0 to 127).
 * @param vel the velocity of the note (0 to 127).
 *
 * @see #noteOff
 */
    void noteOn         ( uchar chn, uchar note, uchar vel );

/**
 * Sends a Note Off MIDI event. This is equivalent to send a Note On event with a 
 * vel value of 0.
 *
 * @param chn the MIDI channel (0 to 15) to play the note on.
 * @param note the key of the note to play (0 to 127).
 * @param vel the velocity of the note (0 to 127).
 *
 * @see #noteOn
 */
    void noteOff        ( uchar chn, uchar note, uchar vel );

/**
 * Sends a Key Pressure (or Aftertouch) MIDI event.
 * This event changes the pressure over a key after this key has been played.
 *
 * @param chn the MIDI channel (0 to 15) where the note is being played.
 * @param note the key of the note (0 to 127).
 * @param vel the new velocity (or pressure) of the note (0 to 127).
 */
    void keyPressure    ( uchar chn, uchar note, uchar vel );


/**
 * Changes the patch (instrument) on a MIDI channel.  
 * 
 * @see setPatchesToUse
 *
 * @param chn the MIDI channel (0 to 15) .
 * @param patch the General Midi patch (0 to 127) to use on the channel chn.
 */
    void chnPatchChange ( uchar chn, uchar patch );

/**
 * Changes the Pressure (Aftertouch) on a MIDI channel. Keep in mind that some
 * synthesizers don't like this events, and it's better not to send it. 
 * 
 * @param chn the MIDI channel (0 to 15) to change.
 * @param vel the velocity (0 to 127) to use on the channel chn.
 */
    void chnPressure    ( uchar chn, uchar vel );

/**
 * Changes the Pitch Bender value on a MIDI channel. This bends the tone of
 * each note played on this channel.
 * 
 * @param chn the MIDI channel (0 to 15) to use.
 * @param lsb @param msb the less significant byte and the most significant
 * byte (0 to 127 each) of the number by which notes will be bend. a 0x4000
 * value means not to bend.
 */
    void chnPitchBender ( uchar chn, uchar lsb,  uchar msb );

/**
 * Sends a Controller event to a MIDI channel. This can be used for example to
 * change the volume, set a XG patch, etc. Look for any General Midi resource
 * page on the net for more information about the available controller events.
 *
 * For example, to set the tremolo value to a maximum on the MIDI channel
 * number one, you should pass 1 to chn, 1 to ctl and 127 to v.
 *
 * @param chn the MIDI channel (0 to 15) to send the event to.
 * @param ctl the controller (0 to 15) to send.
 * @param v the value (data) of the controller.
 */
    void chnController  ( uchar chn, uchar ctl , uchar v );

/**
 * Sends a SYStem EXclusive message to the default MIDI device (usually,
 * external MIDI synths, as most internal synths do not support sysex messages)
 * 
 * @param data the array of bytes that comform the system exclusive message.
 * Without the initial 0xF0 char, and including the final 0xF7 char (end of
 * exclusive message)
 * @param size the size in bytes of the data to send 
 *
 * @see setDefaultDevice
 */
    void sysEx          ( uchar *data,ulong size);

/**
 * Sets the number of MIDI ticks to wait until the next event is sent. This way,
 * you can schedule notes and events to send to the MIDI device.
 */
    void wait (double ticks);

/**
 * Sets the tempo which will be used to convert between ticks and milliseconds.  
 */
    void tmrSetTempo(int v);

/**
 * Starts the timer. You should call tmrStart before using @ref #wait
 */
    void tmrStart(void);

/**
 * Stops the timer. This will be called by @ref #closeDev before closing the device
 */
    void tmrStop(void);

/**
 * Continue the stopped timer . It is the same than starting a new timer, but
 * without resetting it.
 */
    void tmrContinue(void);

/**
 * Synchronizes with the MIDI buffer. Midi events are put into a buffer, along 
 * with timer delays (see @ref #wait). sync returns when the buffer is empty.
 * 
 * @param f if false, it syncronizes by waiting for the buffer to be sent.
 * If true, it forces the synchronization by clearing the buffer inmediately.
 * The "force" method is, of course, not recommended, except in rare situations.
 */ 
    void sync(bool f=0);  

/**
 * Changes the "master" volume of the played events by altering next volume
 * controller events. The parameter i should be in the range of 0 (nothing
 * is heard) to 150 (music is played at a 150% of the original volume).
 * 
 * Keep in mind that as most MIDI files already play music at near the maximumi
 * volume, an i value greater than 100 is very probably ignored most of the times.
 */
    void setVolumePercentage(int i);

/**
 * Returns the device to which the MIDI events will be sent.
 *
 * @see #setDefaultDevice
 */
    int defaultDevice(void);

/**
 * Sets the device to send the MIDI events to.
 * 
 * By using @ref #midiPorts, @ref #synthDevices, @ref #name and @ref #type, you
 * should choose which device to use (note that they are numbered with midi
 * ports being first and synth devices next)
 *
 * @see #defaultDevice
 */
    void setDefaultDevice(int i);

/**
 * Loads the patches you're going to use . This has effect only for GUS cards,
 * although, if you use this function when @ref defaultDevice is not a GUS device,
 * it will be ignored.
 *
 * The parameter is an int [256] array, which contain the following:
 * 
 * The first 0..127 integers, are the number of times each General MIDI patch will be used,
 * and -1 when the corresponding patch won't be used.
 *
 * The 128..255 integers are the number of times each drum voice (each note on the drum channel)
 * will be used, and -1 when the corresponding percussion won't be used.
 *
 * This is done this way so that if the user has very little memory on his GUS card, and not
 * all patches will be loaded, they are at least reordered, so that it first loads the one you're
 * going to use most.
 *
 * In case you don't worry about such users, or you don't know "a priori" the number of notes
 * you're going to play, you can just use 1 for each patch you want to load and -1 in the rest.
 * 
 * @see GUSOut::setPatchesToUse
 * @see GUSOut::loadPatch 
 * 
 * @return 0 if ok, and -1 if there wasn't enough memory to load the patches
 * in the card's memory.
 */
    int setPatchesToUse(int *patchesused);

/**
 * Returns the filename where the Midi Mapper was loaded from, or "" if no
 * MIDI Mapper is in use.
 *
 * @see #setMidiMap
 */
    char *midiMapFilename(void);

/**
 * Sets a @ref MidiMapper object to use. This object should already have
 * loaded the configuration. See the description of @ref MidiMapper for
 * more information.
 *
 * @see MidiMapper::MidiMapper
 * @see #midiMapFilename
 */
    void setMidiMap(MidiMapper *map);

/**
 * Returns the number of MIDI ports available on the system. It's common that
 * users have MIDI ports available, but there are no external synthesizers
 * connected to these ports, so sending MIDI events to these ports will not
 * produce any music in this case.
 *
 * @see #synthDevices
 * @see #setDefaultDevice
 */
    int midiPorts(void) {return n_midi;};
/**
 * Returns the number of internal synthesizers available on the system. Some
 * of these devices will need special configuration, for example, to load
 * sound patches.
 *
 * @see #midiPorts
 * @see #setDefaultDevice
 * @see #setPatchesToUse
 */
    int synthDevices(void) {return n_synths;};
/**
 * Returns the name of the i-th device . In case the DeviceManager wasn't yet
 * initialized ( see @ref #checkInit ), the return value is NULL, and in case
 * the parameter has a value out of the valid range ( 0 to @ref midiPorts () +
 * @ref synthDevices () ) it returns an empty string.
 */

    char *name(int i);
/**
 * Returns the type of device the i-th device is , in a user-friendly string.
 * For example, "External Midi Port" for midi ports, "FM" for FM synthesizers,
 * "GUS" for Gravis Ultrasound devices, etc.
 */
    const char *type(int i);

};

#endif
