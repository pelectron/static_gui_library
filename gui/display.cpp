#include "display.hpp"

#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <iostream>
namespace sgl::qt {
  DisplayItem::DisplayItem(AbstractItemNode* item, QWidget* parent)
      : QWidget(parent), item_(item), label_(new QLabel(QString(item_->text().data()))) {
    this->update();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::Down, this);
    layout->addWidget(label_, 0, Qt::AlignCenter);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
  }

  void DisplayItem::update() { label_->setText(item_->text().data()); }

  void DisplayItem::set_active(bool active) {

    this->setAutoFillBackground(true);
    label_->setAutoFillBackground(true);
    QPalette pal = label_->palette();
    if (active) {
      pal.setColor(QPalette::Window, Qt::white);
      pal.setColor(QPalette::WindowText, Qt::black);
      active_ = true;
    } else {
      pal.setColor(QPalette::WindowText, Qt::white);
      pal.setColor(QPalette::Window, Qt::black);
      active_ = false;
    }
    label_->setPalette(pal);
  }

  DisplayPage::DisplayPage(AbstractPageNode* page, QWidget* parent)
      : QWidget(parent), page_(page), title_(new QLabel(QString(page->name().data()))),
        list_area_(new QScrollArea) {
    title_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    title_->setAutoFillBackground(true);
    title_->setStyleSheet("background-color: black; color: white");
    auto font = title_->font();
    font.setPointSize(30);
    title_->setFont(font);

    // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setStyleSheet("background-color: black; color: white");

    auto* list_layout = new QVBoxLayout();
    for (auto item : page->children()) {
      auto* new_item = new DisplayItem((AbstractItemNode*)item);
      // new_item->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      auto item_font = new_item->font();
      item_font.setPointSize(20);
      new_item->setFont(item_font);
      new_item->setStyleSheet("background-color: transparent; color: white");
      items_.push_back(new_item);
      list_layout->addWidget(items_.back());
    }
    items_[page->current_index()]->set_active(true);
    list_area_->setWidget(new QWidget);
    list_area_->widget()->setLayout(list_layout);
    list_area_->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    list_area_->setFocusPolicy(Qt::NoFocus);
    list_area_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    list_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list_area_->setWidgetResizable(true);
    auto pal = list_area_->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    list_area_->setPalette(pal);

    auto* seperator = new QFrame;
    seperator->setFrameShape(QFrame::Shape::HLine);
    seperator->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    seperator->setLineWidth(3);
    pal = seperator->palette();
    pal.setColor(QPalette::Window, Qt::white);
    seperator->setPalette(pal);

    auto* outer = new QVBoxLayout;
    outer->setAlignment(Qt::AlignCenter);
    outer->addWidget(title_);
    outer->addWidget(seperator);
    outer->addWidget(list_area_);

    this->setLayout(outer);
    // list_->installEventFilter(this);
  }

  void DisplayPage::update() {
    for (size_t i = 0; i < items_.size(); ++i) {
      items_[i]->update();
      if (i == page_->current_index()) {
        if (page_->edit_mode())
          items_[i]->setStyleSheet("background-color: blue; color: white");
        else
          items_[i]->setStyleSheet("background-color: white; color: black");

        auto pos = items_[i]->pos();
        auto diff = pos.y() + items_[i]->height() - list_area_->height();

        if (diff > 0) {
          list_area_->scroll(0, -(diff + 50));
          std::cout << "diff = " << diff << std::endl;
        }
      } else
        items_[i]->setStyleSheet("background-color: black; color: white");
    }
  }
  QScrollArea* DisplayPage::scroll_area() { return list_area_; }

  Display::Display(sgl::qt::MenuTree& menu, QWidget* parent)
      : QFrame(parent), menu_(menu), area_(new QScrollArea), content_(new QStackedWidget) {
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    auto* main_layout = new QBoxLayout(QBoxLayout::Direction::Down);
    main_layout->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(content_, 0, Qt::AlignCenter);
    setLayout(main_layout);

    // auto pal = area_->palette();
    // pal.setColor(QPalette::Window, Qt::transparent);
    // area_->setPalette(pal);
    // area_->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    // area_->setFocusPolicy(Qt::NoFocus);
    // area_->setWidget(content_);
    // area_->setWidgetResizable(true);
    // area_->ensureWidgetVisible(content_, border_thickness_, border_thickness_);
    // area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // area_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    for (const auto& page : menu.root()->children()) {
      auto* p = new DisplayPage((AbstractPageNode*)page);
      pages_.push_back(p);
      content_->addWidget(p);
    }
    update();
  }

  QSize Display::sizeHint() const { return {content_width(), content_height()}; }

  void Display::set_radius(qreal radius) { radius_ = radius; }

  void Display::set_border_thickness(qreal thickness) {
    border_thickness_ = thickness;
    this->layout()->setContentsMargins(border_thickness_ / 2,
                                       border_thickness_ / 2,
                                       border_thickness_ / 2,
                                       border_thickness_ / 2);
  }

  int Display::content_width() const { return width() - border_thickness_; }
  int Display::content_height() const { return height() - border_thickness_; }

  void Display::update() {
    content_->setCurrentIndex(menu_.active_index().page_index);
    for (auto p : pages_) {
      p->update();
    }
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

} // namespace sgl::qt