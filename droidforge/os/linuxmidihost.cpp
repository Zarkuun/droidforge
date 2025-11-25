#include "os.h"
#ifdef Q_OS_LINUX
#include "linuxmidihost.h"
#include "globals.h"

#include <QDebug>

#include <alsa/asoundlib.h>
#include <alsa/seq.h>
#include <alsa/seq_midi_event.h>

// so we dont pull alsa into the header!
struct alsaSeq {
	snd_seq_t* seq;
};

bool LinuxMIDIHost::x7Connected() const
{
    int port;
    return findX7(&port) != -1;
}

int LinuxMIDIHost::findX7(int *port) const
{
    int ret = -1;

    int streams = SND_SEQ_OPEN_OUTPUT;
    unsigned cap = SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_READ;

    snd_seq_t *seq;
    int err = snd_seq_open(&seq, "hw", streams, 0);
    if (err < 0) {
	return ret;
    }

    snd_seq_client_info_t *cinfo;
    snd_seq_client_info_alloca(&cinfo);
    snd_seq_client_info_set_client(cinfo, -1);

    while (snd_seq_query_next_client(seq, cinfo) >= 0) {
	int client = snd_seq_client_info_get_client(cinfo);

	snd_seq_port_info_t *pinfo;
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_port_info_set_client(pinfo, client);
	snd_seq_port_info_set_port(pinfo, -1);

	while (snd_seq_query_next_port(seq, pinfo) >= 0) {
	    if ((snd_seq_port_info_get_capability(pinfo) & cap) == cap) {
		QString name = snd_seq_client_info_get_name(cinfo);
		if (name.contains("DROID X7 MIDI")) {
		    ret = snd_seq_port_info_get_client(pinfo);
		    *port = snd_seq_port_info_get_port(pinfo);
		    break;
		}
	    }
	}
    }
    snd_seq_close(seq);

    return ret;
}

bool LinuxMIDIHost::connect(int client, int port)
{
    alsa = new alsaSeq;

    int err = snd_seq_open(&alsa->seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
    if (err < 0) {
	delete alsa;
	return false;
    }
    snd_seq_set_client_name(alsa->seq, "DROID Forge");

    snd_seq_create_simple_port(alsa->seq, "Output Port", SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC);

    snd_seq_connect_to(alsa->seq, 0, client, port);

    return true;
}

void LinuxMIDIHost::send(uint8_t *data, unsigned size)
{
    snd_seq_event_t ev;
    snd_seq_ev_set_source(&ev, 0);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);

    snd_midi_event_t *mev;
    snd_midi_event_new(size, &mev);
    snd_midi_event_resize_buffer(mev, size);
    snd_midi_event_encode(mev, data, size, &ev);

    snd_seq_event_output(alsa->seq, &ev);
    snd_seq_drain_output(alsa->seq);

    snd_midi_event_free(mev);
}

void LinuxMIDIHost::disconnect(int client, int port)
{
    snd_seq_disconnect_from(alsa->seq, 0, client, port);
    snd_seq_close(alsa->seq);

    delete alsa;
}

QString LinuxMIDIHost::sendPatch(const Patch *patch)
{
    unsigned sysexLength = prepareSysexMessage(patch);
    if (sysexLength == 0)
        return TR("You have exceeded the maximum allowed patch size.");

    int port;
    int client = findX7(&port);
    if (client < 0) {
        return TR("Could not find DROID X7 MIDI device");
    }
    if(!connect(client, port)) {
        return TR("Cannot connect to DROID X7 MIDI device");
    }
    send(sysexData(), sysexLength);

    disconnect(client, port);

    return "";
}
#endif
