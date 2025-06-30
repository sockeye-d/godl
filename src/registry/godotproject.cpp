#include "godotproject.h"

GodotProject::GodotProject(QObject *parent)
    : QObject{parent}
{}

void GodotProject::serialize(KConfigGroup config)
{
    godotVersion()->serialize(config.group("version"));
    CFG_WRITE(favorite);
    CFG_WRITE(tags);
    CFG_WRITE(name);
    CFG_WRITE(path);
    CFG_WRITE(lastEditedTime);
}

void GodotProject::deserialize(KConfigGroup config)
{
    godotVersion()->deserialize(config.group("version"));
    setFavorite(CFG_READ(favorite));
    setTags(CFG_READ(tags));
    setName(CFG_READ(name));
    setPath(CFG_READ(path));
    setLastEditedTime(CFG_READ(lastEditedTime));
}
