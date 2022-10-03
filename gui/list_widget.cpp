#include "list_widget.hpp"

#include <QVBoxLayout>

namespace sgl::qt {

  List::List(AbstractPageNode* page, QWidget* parent) : QScrollArea(parent), page_(page) {
    setLayout();
    auto* layout = new QVBoxLayout;
    for (auto item : page_->children()) {
      auto* new_item = new DisplayItem((AbstractItemNode*)item);
      auto item_font = new_item->font();
      item_font.setPointSize(20);
      new_item->setFont(item_font);
      new_item->setStyleSheet("background-color: transparent; color: white");
      //items_.push_back(new DisplayItem((AbstractItemNode*)item));
      layout->addWidget(new_item);
    }
  }

  void List::addWidget(QWidget* widget, int stretch = 0, Qt::Alignment align = Qt::Alignment()) {
    get_layout()->addWidget(widget, stretch, align);
  }
  AbstractItemNode* List::currentNode() { return widgetAt(currentItem()); }

  size_t List::size() const { return list_items.size(); }

  AbstractItemNode* List::widgetAt(size_t i) {
    return i < list_items.size() ? list_items[i] : nullptr;
  }

  size_t List::currentItem() const { page_->current_index(); }

  QVBoxLayout* List::get_layout() { return (QVBoxLayout*)this->layout(); }

} // namespace sgl::qt