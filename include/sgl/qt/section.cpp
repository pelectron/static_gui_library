#include "sgl/qt/section.hpp"

#include <QHBoxLayout>
#include <QIcon>
#include <QVBoxLayout>
#include <stdexcept>

namespace sgl::qt {

  const QSize icon_size(12, 12);

  Section::Section(const QString& title, QWidget* parent)
      : QWidget(parent), header_{new QFrame}, body_{new QFrame}, title_label_{new QLabel(title)},
        collapse_button_{new QPushButton} {

    collapse_button_->setCheckable(true);
    collapse_button_->setIcon(QIcon(":/menu-burger.png"));
    collapse_button_->setIconSize(icon_size);
    collapse_button_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    auto* header_layout{new QHBoxLayout};
    header_layout->addWidget(collapse_button_, Qt::AlignLeft);
    header_layout->addWidget(title_label_, Qt::AlignLeft);

    header_->setLayout(header_layout);
    header_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    auto* main_layout{new QVBoxLayout};
    main_layout->addWidget(header_);
    main_layout->addWidget(body_);
    main_layout->setAlignment(Qt::AlignTop);
    this->setLayout(main_layout);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    QObject::connect(collapse_button_, &QPushButton::clicked, this, &Section::show_body);

    this->show_body(collapse_button_->isChecked());
  }

  QFrame* Section::header() { return header_; }

  QFrame* Section::body() { return body_; }

  QLabel* Section::section_title() { return title_label_; }

  QPushButton* Section::section_button() { return collapse_button_; }

  void Section::show_body(bool checked) {
    if (checked) {
      body_->show();
    } else {
      body_->hide();
    }
  }

  Section::~Section() {}

  VerticalSection::VerticalSection(const QString& title, Qt::Edge edge, QWidget* parent)
      : QWidget(parent), header_{new QFrame}, body_{new QFrame},
        title_label_{new VerticalLabel(title)}, collapse_button_{new QPushButton} {
    collapse_button_->setCheckable(true);
    collapse_button_->setIcon(QIcon(":/menu-burger.png"));
    collapse_button_->setIconSize(icon_size);
    collapse_button_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    auto* header_layout{new QVBoxLayout};
    header_layout->setAlignment(Qt::AlignTop);
    header_layout->addWidget(collapse_button_);
    title_label_->setAlignment(Qt::AlignTop);
    header_layout->addWidget(title_label_);
    header_->setLayout(header_layout);

    auto* main_layout{new QHBoxLayout};
    switch (edge) {
      case Qt::Edge::LeftEdge:
        main_layout->addWidget(header_);
        main_layout->addWidget(body_);
        main_layout->setAlignment(Qt::AlignRight);
        break;
      case Qt::Edge::RightEdge:
        main_layout->addWidget(body_);
        main_layout->addWidget(header_);
        main_layout->setAlignment(Qt::AlignLeft);
        break;
      default:
        throw std::invalid_argument("argument 'edge' must be Qt::LeftEdge or Qt::RightEdge");
    }

    this->setLayout(main_layout);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QObject::connect(collapse_button_, &QPushButton::clicked, this, &VerticalSection::show_body);

    this->show_body(collapse_button_->isChecked());
  }

  QFrame* VerticalSection::header() { return header_; }

  QFrame* VerticalSection::body() { return body_; }

  VerticalLabel* VerticalSection::section_title() { return title_label_; }

  QPushButton* VerticalSection::section_button() { return collapse_button_; }

  void VerticalSection::show_body(bool checked) {
    if (checked) {
      body_->show();
    } else {
      body_->hide();
    }
  }

  VerticalSection::~VerticalSection() {}
} // namespace sgl::qt