#pragma once

#include <QNetworkAccessManager>

class Google : public QObject {
Q_OBJECT

	QNetworkAccessManager m_nam;

private slots:
	void onFinished(QNetworkReply *reply);

public:
	Google(QObject *parent = nullptr);

	struct Result {
		QString title, url, snippet;
	};

	void query(const QString &query);

signals:
	void results(const QString &query, const QVector<Result> &results);
};

