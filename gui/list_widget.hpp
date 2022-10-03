#ifndef LIST_WIDGET_HPP
#define LIST_WIDGET_HPP
#include <QScrollArea>
#include <QLabel>
#include "menu_tree.hpp"
#include <vector>
namespace sgl::qt {

  class List : public QScrollArea {
    Q_OBJECT
  public:
    List( AbstractPageNode* page,QWidget* parent = nullptr);
    void     addWidget(QWidget* widget, int stretch = 0, Qt::Alignment align = Qt::Alignment());
    size_t   size() const;
    AbstractItemNode* widgetAt(size_t i);
    AbstractItemNode* currentNode();
    size_t currentItem()const;
  private:
    QVBoxLayout*          get_layout();
    AbstractPageNode* page_;
  };
} // namespace sgl::qt

#endif /* LIST_WIDGET_HPP */
