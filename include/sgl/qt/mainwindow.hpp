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
  /**
    This class can be used to test a sgl::Menu.

    The boilerplate for creating your own test application is as follows:

    ```cpp
    #include "sgl/qt/mainwindow.hpp"

    #include <QApplication>

    int main(){
      QApplication app;

      sgl::qt::Mainwindow win{ your_menu_here, number_of_lines_in_the_display_window};
      win.show();

      return app.exec();
    }
    ```

    For more info on what the resulting gui looks like and how to interact with it, see
    [here](markdown/visualizer.md).
   */
  class SGL_API MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    template <typename Menu>
    MainWindow(Menu m, size_t num_lines = 4, QWidget* parent = nullptr)
        : QMainWindow(parent), tree_(std::move(m)) {
      show();
      init_ui(num_lines);
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
    void init_ui(size_t num_lines);

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
