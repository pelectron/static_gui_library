//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#include "sgl/qt/display_page.hpp"

namespace sgl::qt {
  DisplayPage::DisplayPage(AbstractPageNode* page, size_t line_count, QWidget* parent)
      : QFrame(parent), item_layout_(new QVBoxLayout), page_(page),
        line_count_(std::min(line_count, page_->size())), min_size_{0, 0} {
    // set transparent background
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, Qt::black);
    pal.setColor(QPalette::WindowText, Qt::white);
    this->setPalette(pal);

    // fill items_ with DisplayItems
    items_.reserve(page_->size());
    for (size_t i = 0; i < page_->size(); ++i) {
      auto* item = new DisplayItem((AbstractItemNode*)(page_->children()[i]), this);
      items_.push_back(item);
    }
    item_layout_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto* bar = new QFrame(this);
    bar->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Plain);
    bar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    bar->setStyleSheet("background-color:white");

    auto* title = new QLabel(QString(page->name().data()));
    title->setPalette(pal);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(title, 0, Qt::AlignLeft);
    layout->addWidget(bar);
    layout->addLayout(item_layout_);
    this->update_page();
  }

  // QSize DisplayPage::minimumSizeHint() const {
  //   QSize min_size{0, 0};
  //   for (const auto& item : items_) {
  //     min_size.setWidth(std::min(min_size_.width(), item->minimumWidth()));
  //     min_size.setHeight(std::min(min_size_.width(), item->minimumHeight()));
  //   }
  //   return min_size;
  // }

  static DisplayItem* item_cast(QWidget* w) { return dynamic_cast<DisplayItem*>(w); }

  void DisplayPage::update_page() {
    // remove all items from layout, still store pointers to them in items_
    auto* item = item_layout_->takeAt(0);
    while (item != nullptr) {
      item->widget()->hide();
      item = item_layout_->takeAt(0);
    }

    const size_t idx = page_->current_index();
    // add all new item to be displayed in the layout with wrap around
    if ((idx + line_count_) > page_->size()) {
      // add widgets at end of items_
      for (size_t i = idx; i < page_->size(); ++i) {
        item_layout_->addWidget(items_[i], 0, Qt::AlignLeft);
        items_[i]->show();
        items_[i]->update_item();
        // add widgets at front of items_
      }
      const auto lower_bound = (idx + line_count_) % page_->size();
      for (size_t i = 0; i < lower_bound; ++i) {
        item_layout_->addWidget(items_[i], 0, Qt::AlignLeft);
        items_[i]->show();
        items_[i]->update_item();
      }
      // hide all other items
      for (size_t i = lower_bound; i < idx; ++i) {
        items_[i]->hide();
      }
    } else {
      for (size_t i = 0; i < page_->size(); ++i) {
        if (i >= idx and i < idx + line_count_) {
          items_[i]->show();
          items_[i]->update_item();
          item_layout_->addWidget(items_[i], 0, Qt::AlignLeft);
        } else {
          items_[i]->hide();
        }
      }
    }
  }

  DisplayPage::~DisplayPage() {
    while (item_layout_->takeAt(0) != nullptr) {
    }
    for (auto& item : items_) {
      delete item;
    }
  }
} // namespace sgl::qt