#include <QApplication>
#include <QDebug>
#include <QMediaPlayer>
#include <QTimer>

#include "net/crawler.h"

int main(int argc, char **argv) {
	qsrand(QTime::currentTime().msec());
	QApplication app(argc, argv);
	Crawler crawler;
	QMediaPlayer player;
	crawler.start();

	QTimer timer;
	QObject::connect(&timer, &QTimer::timeout, [&]() {
		if(player.state() == QMediaPlayer::StoppedState) {
			if(!crawler.hasMP3s()) {
				qDebug() << "Waiting for MP3...";
			} else {
				QUrl url = crawler.nextMP3();
				qDebug() << "Playing: " << url.toString();
				player.setMedia(url);
				player.play();
			}
		}
	});
	timer.start(100);


	return app.exec();
}
