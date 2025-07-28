#ifndef TESTCOMMAND_H
#define TESTCOMMAND_H

#include "parser.h"

namespace cli::test {

int testBars(const Parser &parser);
int testUnderline(const Parser &parser);

} // namespace cli::test

#endif // TESTCOMMAND_H
