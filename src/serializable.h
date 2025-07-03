#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <KConfig>

#define CFG_READ(entry) config.readEntry(#entry, entry())
#define CFG_WRITE(entry) config.writeEntry(#entry, entry())
struct Serializable
{
    virtual ~Serializable() {}
    virtual void serialize(KConfigGroup config) = 0;
    virtual void deserialize(KConfigGroup config) = 0;
};

#endif // SERIALIZABLE_H
