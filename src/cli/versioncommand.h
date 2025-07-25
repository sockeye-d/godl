#ifndef VERSIONCOMMAND_H
#define VERSIONCOMMAND_H

#include "parser.h"

namespace cli::version {

int list(const Parser &parser);
int remove(const Parser &parser);
int run(const Parser &parser);

} // namespace cli::version

#endif // VERSIONCOMMAND_H
