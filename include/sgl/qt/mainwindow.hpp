//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_QT_MAINWINDOW_HPP
#define SGL_QT_MAINWINDOW_HPP
#ifndef MAINWINDOW_HPP
  #define MAINWINDOW_HPP
  #include "sgl/qt/display.hpp"
  #include "sgl/qt/menu_tree.hpp"
  #include "sgl/qt/section.hpp"

  #include <QLineEdit>
  #include <QMainWindow>
  #include <QTextEdit>
  #include <QTreeWidget>
  #include <QTreeWidgetItem>

namespace sgl::qt {
  class SGL_API MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    template <typename Menu>
    MainWindow(Menu m, QWidget* parent = nullptr) : QMainWindow(parent), tree_(std::move(m)) {
      show();
      init_ui();
    }

    sgl::qt::MenuTree& tree();
    void               set_current_page(size_t i);
    void               set_current_item(size_t i);
    void               set_current_index(MenuIndex i);

  public slots:
    void itemDoubleClicked(QTreeWidgetItem* item, int column);

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    void log(sgl::error, sgl::input);
    void update_window();
    void init_ui();

    sgl::qt::MenuTree tree_;
    sgl::qt::Display* disp_;
    VerticalSection*  side_section_;
    QTreeWidget*      side_tree_;
    Section*          log_section_;
    QTextEdit*        log_text_;
  };

} // namespace sgl::qt

#endif /* MAINWINDOW_HPP */

#endif /* SGL_QT_MAINWINDOW_HPP */
