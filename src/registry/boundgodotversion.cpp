#include "boundgodotversion.h"
#include <KConfigGroup>

void BoundGodotVersion::serialize(KConfigGroup config)
{
    CFG_WRITE(tagName);
    CFG_WRITE(isMono);
}

void BoundGodotVersion::deserialize(KConfigGroup config)
{
    setTagName(CFG_READ(tagName));
    setIsMono(CFG_READ(isMono));
}

QString BoundGodotVersion::toString() const
{
    if (isMono())
        return tagName() % "-mono";
    else
        return tagName();
}

bool operator==(BoundGodotVersion left, BoundGodotVersion right)
{
    return left.tagName() == right.tagName() && left.isMono() == right.isMono();
}
