#include "crawler.h"

#include <QApplication>
#include <QDebug>
#include <QList>
#include <QNetworkRequest>
#include <QUrl>

#include "settings.h"

Crawler::Crawler(QObject *parent) : QObject(parent), m_nam(parent)
{
	connect(&m_nam, &QNetworkAccessManager::finished, this, &Crawler::onFinished);
	connect(&m_google, &Google::results, this, &Crawler::parseGoogleResult);

	// Load Queues
	auto settings = Settings::getSettings();
	QList<QVariant> storage = settings->value("folderQueue").toList();
	for(QVariant &v : storage) m_folder_queue.append(v.toUrl());

	storage = settings->value("mp3queue").toList();
	for(QVariant &v : storage) m_mp3_queue.append(v.toUrl());
}

Crawler::~Crawler()
{
	save();
}

void Crawler::save()
{
	auto settings = Settings::getSettings();

	// Store folder queue
	QList<QVariant> storage;
	for(const QUrl &url : m_folder_queue) storage.append(QVariant(url));
	settings->setValue("folderQueue", storage);

	// Store MP3 queue
	storage.clear();
	for(const QUrl &url : m_mp3_queue) storage.append(QVariant(url));
	settings->setValue("mp3Queue", storage);
	settings->sync();
	qDebug() << "settings saved.";
}

void Crawler::start()
{
	if(m_mp3_queue.isEmpty()) {
		if(m_folder_queue.isEmpty()) {
			qDebug() << "Querying Google...";
			m_google.query("intitle:\"index of\" mp3");
		} else {
			findMP3s(m_folder_queue.takeAt(qrand() % m_folder_queue.size()));
		}
	}
}

void Crawler::parseGoogleResult(const QString &, const QVector<Google::Result> &results)
{
	for(const Google::Result &result : results) {
		if(result.title.startsWith("Index of /")) findMP3s("http://" + result.url);
	}
}

void Crawler::findMP3s(const QUrl &url)
{
	m_nam.get(QNetworkRequest(url));
}

void Crawler::onFinished(QNetworkReply *reply)
{
	QString str {reply->readAll()};
	QUrl url = reply->url();
	reply->deleteLater();

	// Well-formed index of pages are separated by \n
	for(const QString &line : str.split("\n")) {
		if(line.contains("<title>")) {
			int pos = line.indexOf("<title>") + 7;
			QString title = line.mid(pos, line.indexOf("</title>") - pos);
			if(!title.startsWith("Index of /")) {
				qCritical() << "Invalid page: " << url;
				break;
			}
		}

		int start = line.indexOf("<a href=");
		if(start == -1) continue;

		QString a = line.mid(start + 8, line.indexOf("</a>") - (start + 8));
		QStringList parts = a.split("\">");

		QString url_part = parts[0].mid(1, parts[0].length() - 1);
		url_part.replace(" ", "%20");
		QString name = parts[1];

		// Absolute path?
		QString delim = url.path().endsWith("/") ? "" : "/";
		QUrl new_url = url;
		if(url_part.startsWith("/")) new_url.setPath(url_part, QUrl::TolerantMode);
		else new_url.setPath(new_url.path() + delim + url_part, QUrl::TolerantMode);

		if(url_part.endsWith("/")) {
			m_folder_queue.append(new_url);
		} else if(url_part.endsWith(".mp3")) {
			m_mp3_queue.append(new_url);
		}
	}

	// Need more MP3s?
	if(m_mp3_queue.isEmpty()) {
		if(!m_folder_queue.isEmpty()) {
			findMP3s(m_folder_queue.takeAt(qrand() % m_folder_queue.size()));
		} else {
			// TODO: Next Google Page
		}
	}

	save();
}

QUrl Crawler::nextMP3()
{
	if(m_mp3_queue.isEmpty()) return QUrl();
	QUrl mp3 = m_mp3_queue.takeAt(qrand() % m_mp3_queue.size());

	// Need more MP3s?
	if(m_mp3_queue.size() < 10) {
		if(m_folder_queue.size() == 0) {
			// Need next Google page
			// TODO
		} else {
			findMP3s(m_folder_queue.takeAt(qrand() % m_folder_queue.size()));
		}
	}

	return mp3;
}
