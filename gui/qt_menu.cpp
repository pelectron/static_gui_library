#include "qt_menu.hpp"

namespace sgl::qt {
  Menu::~Menu() { delete impl_; }

  void Menu::update_pages() {
    for (size_t i = 0; i < impl_->size(); ++i) {
      static_cast<sgl::qt::Page*>(this->layout()->itemAt(static_cast<int>(i))->widget())
          ->update_items();
    }
    static_cast<QStackedLayout*>(this->layout())->setCurrentIndex(static_cast<int>(impl_->index()));
  }

  void Menu::keyPressEvent(QKeyEvent* event) {
    std::cout << "keypressevent\n";
    sgl::Input input{sgl::Input::none};
    if ((event->key() >= Qt::Key_Space) and (event->key() <= Qt::Key_AsciiTilde)) {
      // ascii character input
      input = sgl::to_input(static_cast<char>(event->key()));
      std::cout << "character input\n";
    } else {
      switch (event->key()) {
        case Qt::Key_Return:
          [[fallthrough]];
        case Qt::Key_Enter:
          input = sgl::Input::enter;
          std::cout << "enter input\n";
          break;
        case Qt::Key_Delete:
          // TODO: delete input
          std::cout << "delete input\n";

          break;
        case Qt::Key_Left:
          std::cout << "left input\n";

          input = sgl::Input::left;
          break;
        case Qt::Key_Right:
          std::cout << "right input\n";

          input = sgl::Input::right;
          break;
        case Qt::Key_Up:
          std::cout << "up input\n";

          input = sgl::Input::up;
          break;
        case Qt::Key_Down:
          std::cout << "down input\n";

          input = sgl::Input::down;
          break;
      }
    }

    if (input == sgl::Input::none) {
      QWidget::keyPressEvent(event);
      std::cout << "unknown input " << (int)event->key();
      return;
    }

    impl_->handle_input(input);
    this->update_pages();
  }
  MenuConcept::~MenuConcept() {}
} // namespace sgl::qt