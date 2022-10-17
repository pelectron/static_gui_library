#ifndef DISPLAY_PAGE_HPP
#define DISPLAY_PAGE_HPP
#include "sgl/qt/display_item.hpp"
#include "sgl/qt/menu_tree.hpp"

#include <QFrame>
#include <QVBoxLayout>


namespace sgl::qt {
  class SGL_API DisplayPage : public QFrame {
    Q_OBJECT
  public:
    DisplayPage(AbstractPageNode* page, size_t line_count = 4, QWidget* parent = nullptr);

    void update_page();

    ~DisplayPage();
    //QSize minimumSizeHint() const override;

  private:
    QVBoxLayout*                       item_layout_{nullptr};
    AbstractPageNode*                  page_{nullptr};
    std::vector<sgl::qt::DisplayItem*> items_;
    size_t                             line_count_{4};
    QSize min_size_;
  };
} // namespace sgl::qt

#endif /* DISPLAY_PAGE_HPP */
