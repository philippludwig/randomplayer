#pragma once

#include <memory>
#include <QSettings>

namespace Settings {

	std::unique_ptr<QSettings> inline getSettings() {
		return std::make_unique<QSettings>(new QSettings("Philipp Ludwig", "randomplayer"));
	}

}
