#include "qt_page.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

namespace sgl::qt {
  PageConcept::~PageConcept() {}
  std::string_view             Page::name() const { return impl_->name(); }
  size_t                       Page::size() const { return impl_->size(); }
  std::vector<sgl::qt::Item*>& Page::items() { return items_; }

  void Page::update_items() {
    for (size_t i = 0; i < items_.size(); ++i) {
      items_[i]->update_text();
      items_[i]->setAutoFillBackground(true);
      if (i == impl_->index()) {
        if (impl_->is_in_edit_mode()) {
          items_[i]->header()->setStyleSheet("background-color: rgb(51,255,51)");
        } else {
          items_[i]->header()->setStyleSheet("background-color: rgb(0,120,215)");
        }
        // items_[i]->show_central_widget(true);
      } else {
        items_[i]->header()->setStyleSheet("background-color: rgb(227, 227, 227)");

        // items_[i]->show_central_widget(false);
      }
    }
  }
  // void Page::paintEvent(QPaintEvent* event) {
  //   QStyleOption opt;
  //   opt.init(this);
  //   QPainter p(this);
  //   style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

  //   QWidget::paintEvent(event);
  // }
  Page::~Page() { delete impl_; }

} // namespace sgl::qt