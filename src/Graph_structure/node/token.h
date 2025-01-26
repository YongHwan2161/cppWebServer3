#ifndef TOKEN_H
#define TOKEN_H

#include "../../../CGDB.h"

int integrate_token(unsigned int node_index, unsigned int next_node, ushort to_integrate_ch[], ushort channel_count, bool save);
int integrate_token_prepare(unsigned int node_index, bool save);
int handle_integrate_tokens(char* args);

#endif // TOKEN_H