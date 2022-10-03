#include "mainwindow.hpp"
namespace sgl::qt {
  const char*  to_string(sgl::error e);
  QString      to_string(sgl::Input i);
  static void  update_content(sgl::qt::AbstractMenuNode* menu, QTreeWidget* tree);
  QTreeWidget* make_tree(sgl::qt::AbstractMenuNode* menu, const QString& title);
  static void  update_item_leaves(sgl::qt::AbstractPageNode* page, QTreeWidgetItem* page_item);

  void MainWindow::keyPressEvent(QKeyEvent* event) {
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
      std::cout << int(event->key()) << std::endl;
    }

    auto error = tree_.root()->handle_input(input);
    if (error != error::no_error)
      this->log(error, input);

    disp_->update();
    sgl::qt::update_content(tree_.root(), side_tree_);
  }

  void MainWindow::init_ui() {
    this->setFocusPolicy(Qt::StrongFocus);

    disp_ = new Display(tree_);
    disp_->set_radius(10);
    disp_->set_border_thickness(15);
    disp_->setFocusPolicy(Qt::NoFocus);
    disp_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    side_tree_ = make_tree(tree_.root(), "A menu title");
    side_tree_->setFocusPolicy(Qt::NoFocus);

    auto* side_layout = new QVBoxLayout;
    side_layout->addWidget(side_tree_);
    side_section_->setFocusPolicy(Qt::NoFocus);
    side_section_->section_title()->setFocusPolicy(Qt::NoFocus);
    side_section_->section_button()->setFocusPolicy(Qt::NoFocus);
    emit side_section_->section_button()->clicked(true);
    side_section_->body()->setFocusPolicy(Qt::NoFocus);
    side_section_->body()->setLayout(side_layout);

    log_section_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    log_section_->setFocusPolicy(Qt::NoFocus);
    log_section_->body()->setLayout(new QVBoxLayout);
    log_section_->body()->layout()->addWidget(log_text_);

    auto* main_inner = new QHBoxLayout;
    main_inner->addWidget(side_section_);
    main_inner->addWidget(disp_, 2, Qt::AlignTop);

    auto* main_outer = new QVBoxLayout;
    main_outer->addLayout(main_inner);
    main_outer->addWidget(log_section_);
    this->setCentralWidget(new QFrame(this));
    this->centralWidget()->setLayout(main_outer);

    disp_->update();
    update_content(tree_.root(), side_tree_);
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

  QTreeWidget* make_tree(sgl::qt::AbstractMenuNode* menu, const QString& title) {
    QTreeWidget* widget = new QTreeWidget;
    widget->setColumnCount(1);

    QList<QTreeWidgetItem*> pages;
    for (const auto& page : menu->children()) {
      auto* tree_page = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr),
                                            QStringList(QString(page->name().data())));
      pages.append(tree_page);
      QList<QTreeWidgetItem*> items;
      for (const auto& item : page->children()) {
        auto* new_item = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr),
                                             QStringList(QString(item->name().data())));
        items.append(new_item);
        new QTreeWidgetItem(new_item, QStringList("Type: " + QString(item->type_name().data())));
        new QTreeWidgetItem(new_item, QStringList("Content: " + QString(item->text().data())));
      }
      tree_page->addChildren(items);
    }
    widget->addTopLevelItems(pages);
    widget->setHeaderItem(
        new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(title)));
    return widget;
  }
  void MainWindow::log(sgl::error e, sgl::Input i) {
    QString str = QString("Input event:\n\tError: ") + to_string(e) + QString("\n\tInput: ") +
                  to_string(i) + '\n';
    log_text_->append(str);
  }

  const char* to_string(sgl::error e) {
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