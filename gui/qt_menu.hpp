#ifndef QT_MENU_HPP
#define QT_MENU_HPP
#include "qt_page.hpp"

#include <QKeyEvent>
#include <QStackedLayout>
#include <iostream>

namespace sgl::qt {

  class MenuConcept;

  class Menu : public QWidget {
    Q_OBJECT
  public:
    template <typename M>
    Menu(M menu, QWidget* parent = nullptr);
    ~Menu();
    void            update_pages();

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    MenuConcept* impl_{nullptr};
  };

  class MenuConcept {
  public:
    virtual sgl::error           handle_input(sgl::Input) = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    [[nodiscard]] virtual size_t index() const = 0;
    virtual ~MenuConcept() = 0;
  };

  template <typename TestMenu>
  class MenuModel : public MenuConcept {
  public:
    MenuModel(sgl::qt::Menu* qt_menu, QStackedLayout* layout, TestMenu menu)
        : dut(std::move(menu)) {
      sgl::for_each_with_name(dut, [qt_menu, layout](auto name, auto& page) -> void {
        auto* page_widget =
            new sgl::qt::Page(std::string_view(name.data(), name.size()), page, qt_menu);
        page_widget->setFocusPolicy(Qt::NoFocus);
        layout->addWidget(page_widget);
      });
    }
    ~MenuModel() override {}

    sgl::error handle_input(sgl::Input i) override { return dut.handle_input(i); }

    [[nodiscard]] size_t size() const override { return dut.size(); }
    [[nodiscard]] size_t index() const override { return dut.current_page_index(); }

  private:
    TestMenu dut; ///< actual menu to test
  };

  template <typename M>
  Menu::Menu(M menu, QWidget* parent) : QWidget(parent) {
    auto* layout { new QStackedLayout };
    this->setLayout(layout);
    impl_ = new MenuModel<M>(this, layout, std::move(menu));
    this->setFocus();
    this->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  }
} // namespace sgl::qt

#endif /* QT_MENU_HPP */
