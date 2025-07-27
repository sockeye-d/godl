#ifndef GCONFIGCOMMAND_H
#define GCONFIGCOMMAND_H

#include "parser.h"

namespace cli {

int general(const Parser &parser);
namespace filter {
int add(const Parser &parser);
int remove(const Parser &parser);
int list(const Parser &parser);
} // namespace filter
namespace source {
int add(const Parser &parser);
int remove(const Parser &parser);
int list(const Parser &parser);
} // namespace source
int versionConfig(const Parser &parser);

} // namespace cli

#endif // GCONFIGCOMMAND_H
