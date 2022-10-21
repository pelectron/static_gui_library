//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include "sgl/qt/display_page.hpp"
#include "sgl/qt/menu_tree.hpp"

#include <QFrame>
#include <QScrollArea>
#include <QStackedWidget>
#include <vector>

namespace sgl::qt {

  class SGL_API Display : public QFrame {
    Q_OBJECT
  public:
    Display(sgl::qt::MenuTree& menu, QWidget* parent = nullptr);

    void set_radius(qreal radius);

    void set_border_thickness(qreal thickness);

    int        content_width() const;
    int        content_height() const;
    void       update_display();
    sgl::error set_current_page(size_t i);

  protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

  private:
    sgl::qt::MenuTree&        menu_;                ///< reference to menu data to display
    QScrollArea*              area_{nullptr};       //< the list layout is on this area
    QStackedWidget*           page_stack_{nullptr}; /// the individual pages
    qreal                     radius_{10};
    qreal                     border_thickness_{10};
    std::vector<DisplayPage*> pages_;
  };

} // namespace sgl::qt

#endif /* DISPLAY_HPP */
