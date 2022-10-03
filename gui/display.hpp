#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include "menu_tree.hpp"

#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <vector>

namespace sgl::qt {
  class DisplayItem : public QWidget {
  public:
    DisplayItem(AbstractItemNode* item, QWidget* parent = nullptr);

    void update();
    void set_active(bool active);

  private:
    AbstractItemNode* item_;
    QLabel*           label_;
    bool              active_{false};
  };

  class DisplayPage : public QWidget {
    Q_OBJECT
  public:
    DisplayPage(AbstractPageNode* page, QWidget* parent = nullptr);

    void update();
    QScrollArea* scroll_area();
  private:
    AbstractPageNode*         page_{nullptr};
    QLabel*                   title_{nullptr};
    QScrollArea*              list_area_;
    std::vector<DisplayItem*> items_{};
  };

  /**
   * \brief
   * ---------------
   * |  page name:
   * | -------------
   * | page item 1
   * | page item 2
   * | ...
   * ---------------
   */
  class Display : public QFrame {
    Q_OBJECT
  public:
    Display(sgl::qt::MenuTree& menu, QWidget* parent = nullptr);

    void set_radius(qreal radius);

    void set_border_thickness(qreal thickness);

    int  content_width() const;
    int  content_height() const;
    void update();

  protected:
    QSize sizeHint() const override;
    void  paintEvent(QPaintEvent*) override;
    void  resizeEvent(QResizeEvent*) override;

  private:
    sgl::qt::MenuTree&        menu_;             ///< reference to menu to display
    QScrollArea*              area_{nullptr};    //< the list layout is on this area
    QStackedWidget*           content_{nullptr}; /// the individual pages
    qreal                     radius_{10};
    qreal                     border_thickness_{10};
    std::vector<DisplayPage*> pages_;
  };
} // namespace sgl::qt

#endif /* DISPLAY_HPP */
