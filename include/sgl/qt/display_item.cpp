//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#include "sgl/qt/display_item.hpp"

#include <QHBoxLayout>

namespace sgl::qt {
  DisplayItem::DisplayItem(AbstractItemNode* item, QWidget* parent)
      : QWidget(parent), item_{item}, text_label_{new QLabel(QString(item_->text().data()))},
        index_label_{new QLabel(QString::number(item_->index() + 1) + '.')} {
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    text_label_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    index_label_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    this->setFocusPolicy(Qt::NoFocus);
    text_label_->setFocusPolicy(Qt::NoFocus);
    index_label_->setFocusPolicy(Qt::NoFocus);

    QBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(index_label_, 0, Qt::AlignRight);
    layout->addWidget(text_label_, 0, Qt::AlignLeft);
    layout->setAlignment(Qt::AlignCenter);

    this->set_active(item_->is_current());
  }

  void DisplayItem::update_item() {
    text_label_->setText(item_->text().data());
    set_active(item_->is_current());
  }

  void DisplayItem::set_active(bool active) {
    this->setAutoFillBackground(true);
    text_label_->setAutoFillBackground(true);
    index_label_->setAutoFillBackground(true);
    QPalette pal = text_label_->palette();
    if (active) {
      pal.setColor(QPalette::Window, Qt::white);
      pal.setColor(QPalette::WindowText, Qt::black);
    } else {
      pal.setColor(QPalette::WindowText, Qt::white);
      pal.setColor(QPalette::Window, Qt::black);
    }
    text_label_->setPalette(pal);
    index_label_->setPalette(pal);
  }

  size_t DisplayItem::index() const { return item_->index(); }

} // namespace sgl::qt