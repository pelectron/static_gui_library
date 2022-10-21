//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#include "sgl/qt/mainwindow.hpp"

#include <QKeyEvent>
#include <iostream>

namespace sgl::qt {
  static const char* to_string(sgl::error e);
  static QString     to_string(sgl::Input i);
  static void        update_content(sgl::qt::AbstractMenuNode* menu, QTreeWidget* tree);
  QTreeWidget*
      make_tree(sgl::qt::AbstractMenuNode* menu, const QString& title, QWidget* parent = nullptr);
  static void update_item_leaves(sgl::qt::AbstractPageNode* page, QTreeWidgetItem* page_item);

  void MainWindow::keyPressEvent(QKeyEvent* event) {
    sgl::Input input{sgl::Input::none};
    if ((event->key() >= Qt::Key_Space) and (event->key() <= Qt::Key_AsciiTilde)) {
      // ascii character input
      input = sgl::to_input(static_cast<char>(event->key()));
    } else {
      switch (event->key()) {
        case Qt::Key_Return:
          [[fallthrough]];
        case Qt::Key_Enter:
          input = sgl::Input::enter;
          break;
        case Qt::Key_Delete:
          // TODO: delete input
          break;
        case Qt::Key_Left:
          input = sgl::Input::left;
          break;
        case Qt::Key_Right:
          input = sgl::Input::right;
          break;
        case Qt::Key_Up:
          input = sgl::Input::up;
          break;
        case Qt::Key_Down:
          input = sgl::Input::down;
          break;
      }
    }

    auto error = tree_.root()->handle_input(input);
    if (error != error::no_error)
      this->log(error, input);

    this->update_window();
  }

  void MainWindow::init_ui() {
    setUpdatesEnabled(false);
    auto* center_frame = new QFrame(this);
    this->setCentralWidget(center_frame);
    auto* layout = new QVBoxLayout(center_frame);
    auto* h_layout = new QHBoxLayout;

    disp_ = new Display(tree_, center_frame);
    side_section_ = new VerticalSection("menu tree", Qt::Edge::RightEdge, center_frame);
    side_tree_ = make_tree(tree_.root(), "menu tree");
    log_section_ = new Section("Input Error Log");
    log_text_ = new QTextEdit;

    layout->addLayout(h_layout);
    layout->addWidget(log_section_);

    h_layout->addWidget(side_section_);

    disp_->set_radius(10);
    disp_->set_border_thickness(15);
    disp_->setFocusPolicy(Qt::NoFocus);
    h_layout->addWidget(disp_, 2, Qt::AlignTop);

    side_tree_->setFocusPolicy(Qt::NoFocus);

    auto* side_layout = new QVBoxLayout;
    side_layout->addWidget(side_tree_);
    side_section_->setFocusPolicy(Qt::NoFocus);
    side_section_->section_title()->setFocusPolicy(Qt::NoFocus);
    side_section_->section_button()->setFocusPolicy(Qt::NoFocus);
    side_section_->body()->setFocusPolicy(Qt::NoFocus);
    side_section_->body()->setLayout(side_layout);

    log_section_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    log_section_->setFocusPolicy(Qt::NoFocus);
    log_section_->body()->setLayout(new QVBoxLayout);
    log_section_->body()->layout()->addWidget(log_text_);

    connect(side_tree_,
            &QTreeWidget::itemDoubleClicked,
            this,
            &sgl::qt::MainWindow::itemDoubleClicked);

    this->setFixedSize(800, 600);
    this->setFocusPolicy(Qt::StrongFocus);

    this->update_window();
    setUpdatesEnabled(true);
  }

  sgl::qt::MenuTree& MainWindow::tree() { return tree_; }

  void MainWindow::set_current_page(size_t i) {
    auto error = tree_.root()->set_current_page(i);
    if (error != sgl::error::no_error) {
      log_text_->append(QString("Error while setting current page: %s\n").arg(to_string(error)));
    }
    this->update_window();
  }

  void MainWindow::set_current_item(size_t i) {
    tree_.root()->current_page()->set_current_item(i);
    this->update_window();
  }

  void MainWindow::set_current_index(MenuIndex i) {
    tree_.root()->set_current_page(i.page_index);
    this->set_current_item(i.item_index);
  }

  void MainWindow::update_window() {
    this->setUpdatesEnabled(false);
    side_tree_->setUpdatesEnabled(false);
    disp_->update_display();
    update_content(tree_.root(), side_tree_);
    side_tree_->setUpdatesEnabled(true);
    this->setUpdatesEnabled(true);
  }

  void MainWindow::itemDoubleClicked(QTreeWidgetItem* item, int column) {

    if (item->parent() == nullptr) {
      // page item
      this->set_current_page(item->treeWidget()->indexOfTopLevelItem(item));
      this->update_window();
      return;
    }
    if (item->childCount() == 0) {
      // leaf item, a.k.a. a type or content label
      itemDoubleClicked(item->parent(), column);
      this->update_window();
      return;
    }

    this->set_current_page(item->treeWidget()->indexOfTopLevelItem(item->parent()));
    auto*     parent = item->parent();
    const int count = parent->childCount();
    for (int i = 0; i < count; ++i) {
      if (item == parent->child(i)) {
        this->set_current_item(i);
        break;
      }
    }
    this->update_window();
  }

  void MainWindow::log(sgl::error e, sgl::Input i) {
    QString str = QString("Input event:\n\tError: ") + to_string(e) + QString("\n\tInput: ") +
                  to_string(i) + '\n';
    log_text_->append(str);
  }

  static void update_content(sgl::qt::AbstractMenuNode* menu, QTreeWidget* tree) {
    tree->setAutoFillBackground(true);
    const size_t size = menu->size();
    for (size_t i = 0; i < size; ++i) {
      auto* page_item = tree->topLevelItem(i);
      auto  back_brush = tree->palette().window();
      if (menu->current_index() == i) {
        if (((AbstractPageNode*)menu->children()[i])->edit_mode())
          back_brush.setColor(Qt::green);
        else
          back_brush.setColor(Qt::blue);
      } else {
        back_brush.setColor(Qt::GlobalColor::transparent);
      }

      page_item->setBackground(0, back_brush);
      update_item_leaves((AbstractPageNode*)menu->children()[i], page_item);
    }
  }

  static void update_item_leaves(sgl::qt::AbstractPageNode* page, QTreeWidgetItem* page_item) {
    const size_t size = page->size();
    auto         back_brush = page_item->treeWidget()->palette().window();
    for (size_t i = 0; i < size; ++i) {
      auto* item = page_item->child(i);
      if (page->current_index() == i) {
        if (page->edit_mode())
          back_brush.setColor(Qt::green);
        else
          back_brush.setColor(Qt::blue);
      } else {
        back_brush.setColor(Qt::transparent);
      }
      item->setBackground(0, back_brush);
      item->child(1)->setText(0, "Content: " + QString(page->children()[i]->text().data()));
    }
  }

  QTreeWidget* make_tree(sgl::qt::AbstractMenuNode* menu, const QString& title, QWidget* parent) {
    QTreeWidget* widget = new QTreeWidget(parent);
    widget->setColumnCount(1);

    for (const auto& page : menu->children()) {
      auto* tree_page = new QTreeWidgetItem(widget, QStringList(QString(page->name().data())));
      for (const auto& item : page->children()) {
        auto* new_item = new QTreeWidgetItem(tree_page, QStringList(QString(item->name().data())));
        new QTreeWidgetItem(new_item, QStringList("Type: " + QString(item->type_name().data())));
        new QTreeWidgetItem(new_item, QStringList("Content: " + QString(item->text().data())));
      }
    }
    widget->setHeaderItem(
        new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(title)));
    return widget;
  }

  static const char* to_string(sgl::error e) {
    switch (e) {
      case error::no_error:
        return "No Error";
      case error::edit_finished:
        return "Edit finished";
      case error::format_error:
        return "Format Error";
      case error::invalid_format:
        return "Invalid Format";
      case error::invalid_input:
        return "Invalid Input";
      case error::invalid_page_index:
        return "Invalid Page Index";
      case error::invalid_value:
        return "Invalid Value";
      case error::not_editable:
        return "Not Editable";
      case error::null_element:
        return "Null Element";
      case error::null_format:
        return "Null Format";
      case error::null_page:
        return "Null Page";
      case error::page_not_found:
        return "Page Not Found";
      default:
        return "Unknown Error";
    }
  }

  QString to_string(sgl::Input i) {
    if (sgl::is_keyboard_input(i)) {
      return QString{sgl::get_char8(i)};
    } else {
      switch (i) {
        case sgl::Input::none:
          return "None";
        case sgl::Input::down:
          return "Down";
        case sgl::Input::up:
          return "Up";
        case sgl::Input::right:
          return "Right";
        case sgl::Input::left:
          return "Left";
        case sgl::Input::enter:
          return "Enter";
        default:
          break;
      }
    }
    return "Unknown input";
  }

} // namespace sgl::qt