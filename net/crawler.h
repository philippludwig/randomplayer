#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "google.h"

class Crawler : public QObject {
Q_OBJECT

	QNetworkAccessManager m_nam;
	Google m_google;

	QList<QUrl> m_folder_queue;
	QList<QUrl> m_mp3_queue;

	void parseGoogleResult(const QString &query, const QVector<Google::Result> &results);
	void onFinished(QNetworkReply *reply);
	void save();

public:
	Crawler(QObject *parent = 0);
	~Crawler();
	void findMP3s(const QUrl &url);
	bool hasMP3s() const { return !m_mp3_queue.isEmpty(); }
	QUrl nextMP3();
	void start();
};
