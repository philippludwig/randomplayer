#include "google.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

#include <json.hpp>

Google::Google(QObject *parent) : QObject(parent)
{
	connect(&m_nam, &QNetworkAccessManager::finished, this, &Google::onFinished);
}

void Google::query(const QString &query)
{
	QSettings googleini("google.ini", QSettings::IniFormat);
	const QString google_key = googleini.value("apikey", "").toString();
	const QString custom_search_engine = googleini.value("cse", "").toString();
	googleini.setValue("apikey", google_key);
	googleini.setValue("cse", custom_search_engine);
	googleini.sync();

	QString request = "https://www.googleapis.com/customsearch/v1?q=" + query + "&cx=" + custom_search_engine + "&key=" + google_key;
	m_nam.get(QNetworkRequest(QUrl(request)));
}

void Google::onFinished(QNetworkReply *reply)
{
	using nlohmann::json;
	QByteArray data = reply->readAll();
	reply->deleteLater();
	json j;
	try {
		j = json::parse(data);
	} catch (std::exception &e) {
		qCritical() << "Could not parse JSON: " << e.what();
	}

	if(j.find("items") == j.end()) {
		qCritical() << "Invalid json response:\n" << j.dump().data();
		return;
	}

	QVector<Result> results;
	for(auto &&item : j["items"]) {
		Result result;
		result.url = QString::fromStdString(item["formattedUrl"].get<std::string>());
		result.title = QString::fromStdString(item["title"].get<std::string>());
		result.snippet = QString::fromStdString(item["snippet"].get<std::string>());
		results.append(result);
	}

	emit Google::results(QString::fromStdString(j["queries"]["request"][0]["searchTerms"].get<std::string>()), results);
}

