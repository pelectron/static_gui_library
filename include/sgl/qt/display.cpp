#include "sgl/qt/display.hpp"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <iostream>

namespace sgl::qt {

  Display::Display(sgl::qt::MenuTree& menu, QWidget* parent)
      : QFrame(parent), menu_(menu), area_(new QScrollArea(this)),
        page_stack_(new QStackedWidget(this)) {
    auto* main_layout = new QBoxLayout(QBoxLayout::Direction::Down, this);
    main_layout->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(area_, 0, Qt::AlignCenter);

    auto pal = area_->palette();
    pal.setColor(QPalette::Window, Qt::black);
    area_->setPalette(pal);
    area_->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    area_->setWidget(page_stack_);
    area_->setWidgetResizable(true);
    area_->ensureWidgetVisible(page_stack_);
    area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    area_->setFocusPolicy(Qt::NoFocus);

    page_stack_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    page_stack_->setFocusPolicy(Qt::NoFocus);

    for (auto& page : menu.root()->children()) {
      auto* p = new DisplayPage((AbstractPageNode*)page);
      p->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      pages_.push_back(p);
      page_stack_->addWidget(p);
    }

    update_display();
  }

  void Display::set_radius(qreal radius) { radius_ = radius; }

  void Display::set_border_thickness(qreal thickness) {
    border_thickness_ = thickness;
    this->layout()->setContentsMargins(border_thickness_,
                                       border_thickness_,
                                       border_thickness_,
                                       border_thickness_);
  }

  int Display::content_width() const { return width() - border_thickness_; }
  int Display::content_height() const { return height() - border_thickness_; }

  void Display::update_display() {
    setUpdatesEnabled(false);
    page_stack_->setCurrentIndex(menu_.active_index().page_index);
    for (auto p : pages_) {
      p->update_page();
    }
    setUpdatesEnabled(true);
  }

  void Display::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRect{0, 0, this->width(), this->height()}, radius_, radius_);
    QPen pen(Qt::darkGray, border_thickness_);
    p.setPen(pen);
    p.fillPath(path, Qt::black);
    p.drawPath(path);
  }

  void Display::resizeEvent(QResizeEvent*) {
    this->layout()->setContentsMargins(border_thickness_,
                                       border_thickness_,
                                       border_thickness_,
                                       border_thickness_);
  }
  sgl::error Display::set_current_page(size_t index) {
    auto error = menu_.root()->set_current_page(index);
    if (error == sgl::error::no_error) {
      page_stack_->setCurrentIndex(static_cast<int>(index));
    }
    return error;
  }
} // namespace sgl::qt