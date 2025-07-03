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
bool operator==(BoundGodotVersion left, BoundGodotVersion right)
{
    return left.tagName() == right.tagName() && left.isMono() == right.isMono();
}
