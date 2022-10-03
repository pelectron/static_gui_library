#ifndef QT_PAGE_HPP
#define QT_PAGE_HPP
#include "qt_item.hpp"
#include "sgl/sgl.hpp"

#include <QVBoxLayout>
#include <QWidget>
#include <string>
#include <string_view>

namespace sgl::qt {

  class PageConcept;

  class Page : public QWidget {
    Q_OBJECT
  public:
    template <typename MenuPage>
    Page(std::string_view page_name, MenuPage& page, QWidget* parent = nullptr);
    ~Page();

    [[nodiscard]] std::string_view name() const;
    [[nodiscard]] size_t           size() const;
    std::vector<sgl::qt::Item*>&   items();
   // void                           paintEvent(QPaintEvent* event) override;

    void update_items();

  private:
    PageConcept*                impl_;
    std::vector<sgl::qt::Item*> items_;
    QVBoxLayout*                item_layout_;
  };



  class PageConcept {
  public:
    [[nodiscard]] virtual std::string_view name() const = 0;
    [[nodiscard]] virtual size_t           size() const = 0;
    [[nodiscard]] virtual size_t           index() const = 0;
    [[nodiscard]] virtual bool is_in_edit_mode()const = 0;
    virtual ~PageConcept() = 0;
  };

  template <typename MenuPage>
  class PageModel : public PageConcept {
  public:
    PageModel(sgl::qt::Page* qt_page, std::string_view page_name, MenuPage& page)
        : name_(page_name), page_(&page) {
      sgl::for_each_with_name(page, [qt_page](auto name, auto& item) {
        qt_page->items().push_back(
            new sgl::qt::Item(std::string_view(name.data(), name.size()), item, qt_page));
      });
    }

    [[nodiscard]] std::string_view name() const override { return name_; }
    [[nodiscard]] size_t           size() const override { return page_->size(); }
    [[nodiscard]] size_t           index() const override { return page_->current_item_index(); }
    [[nodiscard]] bool is_in_edit_mode()const override{ return page_->is_in_edit_mode();}
    ~PageModel() override {}

  private:
    std::string_view name_;
    MenuPage*        page_;
  };

  template <typename MenuPage>
  Page::Page(std::string_view page_name, MenuPage& page, QWidget* parent)
      : QWidget(parent), item_layout_(new QVBoxLayout) {
    this->setFocusPolicy(Qt::NoFocus);
    impl_ = new PageModel<MenuPage>(this, page_name, page);
    this->setLayout(item_layout_);
    for (sgl::qt::Item* item : items_) {
      item_layout_->addWidget(item);
      item->setFocusPolicy(Qt::NoFocus);
      item->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }
    this->update_items();
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  }
} // namespace sgl::qt

#endif /* QT_PAGE_HPP */
