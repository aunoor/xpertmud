#ifndef KAUDIO_PLAYER_H
#define KAUDIO_PLAYER_H

#include <QString>
#include <QSound>
#include <QDebug>

class KAudioPlayer {
public:
	static void play(const QString & filename) {
		qDebug() << "PlayAudio: " + filename;
		QSound::play(filename);
	}
};

#endif
