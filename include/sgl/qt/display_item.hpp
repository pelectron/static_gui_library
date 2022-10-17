#ifndef SGL_QT_DISPLAY_ITEM_HPP
#define SGL_QT_DISPLAY_ITEM_HPP
#include "sgl/qt/menu_tree.hpp"

#include <QLabel>


namespace sgl::qt {
  class SGL_API DisplayItem : public QWidget {
    Q_OBJECT
  public:
    DisplayItem(AbstractItemNode* item, QWidget* parent = nullptr);

    void update_item();
    void set_active(bool active);
    [[nodiscard]]size_t index()const;
  private:
    AbstractItemNode* item_{nullptr};
    QLabel*           text_label_{nullptr};
    QLabel*           index_label_{nullptr};
  };

} // namespace sgl::qt

#endif /* SGL_QT_DISPLAY_ITEM_HPP */
