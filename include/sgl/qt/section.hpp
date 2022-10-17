#ifndef SECTION_HPP
#define SECTION_HPP
#include "sgl/qt/label.hpp"

#include <QFrame>
#include <QLabel>
#include <QPushButton>

namespace sgl::qt {

  class SGL_API Section : public QWidget {
    Q_OBJECT
  public:
    Section(const QString& title, QWidget* parent = nullptr);
    ~Section();
    QFrame*      header();
    QFrame*      body();
    QLabel*      section_title();
    QPushButton* section_button();
    void         set_title(const QString& str) { title_label_->setText(str); }
  public slots:
    void show_body(bool show);

  private:
    QFrame*      header_{nullptr};
    QFrame*      body_{nullptr};
    QLabel*      title_label_{nullptr};
    QPushButton* collapse_button_{nullptr};
  };

  /**
   * \brief
   */
  class SGL_API VerticalSection : public QWidget {
    Q_OBJECT
  public:
    VerticalSection(const QString& title, Qt::Edge edge, QWidget* parent = nullptr);
    ~VerticalSection();
    QFrame*        header();
    QFrame*        body();
    VerticalLabel* section_title();
    QPushButton*   section_button();

  public slots:
    void show_body(bool show);

  private:
    QFrame*        header_{nullptr};
    QFrame*        body_{nullptr};
    VerticalLabel* title_label_{nullptr};
    QPushButton*   collapse_button_{nullptr};
  };

} // namespace sgl::qt

#endif /* SECTION_HPP */
