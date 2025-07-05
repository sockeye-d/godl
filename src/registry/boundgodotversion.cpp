#include "boundgodotversion.h"
#include "util.h"
#include <KConfigGroup>

void BoundGodotVersion::serialize(KConfigGroup config)
{
    CFG_WRITE(tagName);
    CFG_WRITE(isMono);
    CFG_WRITE(repo);
}

void BoundGodotVersion::deserialize(KConfigGroup config)
{
    setTagName(CFG_READ(tagName));
    setIsMono(CFG_READ(isMono));
    setRepo(CFG_READ(repo));
}

QString BoundGodotVersion::toString() const
{
    if (isMono())
        return repo() / tagName() % "-mono";
    else
        return repo() / tagName();
}

bool operator==(BoundGodotVersion left, BoundGodotVersion right)
{
    return left.tagName() == right.tagName() && left.isMono() == right.isMono();
}
