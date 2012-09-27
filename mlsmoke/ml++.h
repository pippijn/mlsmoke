extern "C" {
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
}

#include <algorithm>
#include <set>
#include <vector>

#include "ml++/callback.h"
#include "ml++/fail.h"
#include "ml++/list.h"
#include "ml++/match.h"
#include "ml++/memory.h"
#include "ml++/option.h"
