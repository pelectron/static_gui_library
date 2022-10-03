#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "display.hpp"
#include "menu_tree.hpp"
#include "qt_menu.hpp"
#include "section.hpp"

#include <QLineEdit>
#include <QMainWindow>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>


namespace sgl::qt {
  class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    template <typename Menu>
    MainWindow(Menu m, QWidget* parent = nullptr) : QMainWindow(parent), tree_(std::move(m)) {
      init_ui();
    }

    sgl::qt::MenuTree& tree() { return tree_; }

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    void init_ui();
    void log(sgl::error, sgl::Input);

    sgl::qt::MenuTree tree_;
    sgl::qt::Display* disp_;
    VerticalSection*  side_section_{new VerticalSection("menu tree", Qt::Edge::RightEdge)};

    QTreeWidget* side_tree_;
    Section*     log_section_{new Section("Input Error Log")};
    QTextEdit*   log_text_{new QTextEdit};
    void*        user_menu_;
  };

} // namespace sgl::qt

#endif /* MAINWINDOW_HPP */
