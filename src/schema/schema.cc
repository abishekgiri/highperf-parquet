#include "hpq/schema.h"
#include <iostream>

namespace hpq {

void Schema::AddColumn(const std::string &name, Type type, bool nullable) {
  columns_.push_back({name, type, nullable});
}

} // namespace hpq
