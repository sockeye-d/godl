#ifndef REMOTECOMMAND_H
#define REMOTECOMMAND_H

#include "cli/parser.h"
namespace cli::remote {

int list(const Parser &parser);
int listTags(const Parser &parser);

} // namespace cli::remote

#endif // REMOTECOMMAND_H
