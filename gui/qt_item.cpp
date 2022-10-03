#include "qt_item.hpp"

namespace sgl::qt {
  std::string_view Item::name() const { return impl_->name(); }
  std::string_view Item::content() const { return impl_->content(); }
  std::string_view Item::type() const { return impl_->type(); }
  Item::~Item() { delete impl_; }
  void Item::update_text() { this->section_title()->setText(QString(impl_->content().data())); }
  ItemConcept::~ItemConcept() {}
} // namespace sgl::qt