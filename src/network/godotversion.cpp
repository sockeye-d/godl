#include "godotversion.h"

GodotVersion::GodotVersion(QObject *parent)
    : QObject{parent}
{}

bool operator==(const GodotVersion &left, const GodotVersion &right)
{
    return left.isMono() == right.isMono() && left.assetName() == right.assetName();
}
