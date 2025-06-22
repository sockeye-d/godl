#include "versionregistry.h"
#include "config.h"

#include <KSharedConfig>

using namespace Qt::StringLiterals;

namespace {
KSharedConfig::Ptr config()
{
    static auto versionConfig = KSharedConfig::openConfig(Config::godotLocation()
                                                          / u"godlversions"_s);
    QObject::connect(Config::self(), &Config::godotLocationChanged, []() {
        versionConfig = KSharedConfig::openConfig(Config::godotLocation() / u"godlversions"_s);
    });

    return versionConfig;
}
} // namespace

void VersionRegistry::registerVersion(QString versionTag, QString executable)
{
    config()->group("").writeEntry(versionTag, executable);
    config()->sync();
}

QString VersionRegistry::versionPath(QString versionTag)
{
    return config()->group("").readEntry(versionTag, "");
}
