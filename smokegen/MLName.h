#pragma once

#include <string>

#include "cstring_container.h"

static cstring_map<> const keywordMap = {
  { "and", "and_" },
  { "as", "as_" },
  { "assert", "assert_" },
  { "begin", "begin_" },
  { "class", "class_" },
  { "constraint", "constraint_" },
  { "do", "do_" },
  { "done", "done_" },
  { "downto", "downto_" },
  { "else", "else_" },
  { "end", "end_" },
  { "exception", "exception_" },
  { "external", "external_" },
  { "false", "false_" },
  { "for", "for_" },
  { "fun", "fun_" },
  { "function", "function_" },
  { "functor", "functor_" },
  { "if", "if_" },
  { "in", "in_" },
  { "include", "include_" },
  { "inherit", "inherit_" },
  { "initializer", "initializer_" },
  { "lazy", "lazy_" },
  { "let", "let_" },
  { "match", "match_" },
  { "method", "method_" },
  { "module", "module_" },
  { "mutable", "mutable_" },
  { "new", "new_" },
  { "object", "object_" },
  { "of", "of_" },
  { "open", "open_" },
  { "or", "or_" },
  { "private", "private_" },
  { "rec", "rec_" },
  { "sig", "sig_" },
  { "struct", "struct_" },
  { "then", "then_" },
  { "to", "to_" },
  { "true", "true_" },
  { "try", "try_" },
  { "type", "type_" },
  { "val", "val_" },
  { "virtual", "virtual_" },
  { "when", "when_" },
  { "while", "while_" },
  { "with", "with_" },
};


static std::string
mlName (std::string &&mungedName)
{
  std::string s = mungedName;
  auto end = s.end () - 1;
  while (end != s.begin ())
    {
      switch (*end)
        {
        case '$': *end = 'S'; break;
        case '#': *end = 'O'; break;
        case '?': *end = 'A'; break;
        }
      --end;
    }

  auto found = keywordMap.find (s.c_str ());
  if (found != keywordMap.end ())
    return found->second;
  return move (s);
}
