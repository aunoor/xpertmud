#ifndef KAUDIO_PLAYER_H
#define KAUDIO_PLAYER_H

#include <qstring.h>
#include <qsound.h>
class KAudioPlayer {
public:
	static void play(const QString & filename) {
		qDebug("PlayAudio: " + filename);
		QSound::play(filename);
	}
};

#endif
