#ifndef CREATECOMMAND_H
#define CREATECOMMAND_H

#include "parser.h"

namespace cli {

int list(const Parser &parser);
int create(const Parser &parser);
bool addTemplateParams(Parser &parser);

} // namespace cli
#endif // CREATECOMMAND_H
