#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include "parser.h"

namespace cli::edit {

int edit(const Parser &parser);
int bind(const Parser &parser);
int configure(const Parser &parser);

namespace tags {
int list(const Parser &parser);
int add(const Parser &parser);
int remove(const Parser &parser);
} // namespace tags

} // namespace cli::edit

#endif // EDITCOMMAND_H
