#ifndef CLIUTIL_H
#define CLIUTIL_H

#include <QString>

namespace cli {
/**
 * @brief makeAbsolute makes a path absolute, relative to the current working directory
 * @param path
 * @return 
 */
QString makeAbsolute(const QString &path);
} // namespace cli

#endif // CLIUTIL_H
