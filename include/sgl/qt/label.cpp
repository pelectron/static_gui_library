#include "sgl/qt/label.hpp"

#include <QPainter>

namespace sgl::qt {
  VerticalLabel::VerticalLabel(QWidget* parent) : QLabel(parent) {}
  VerticalLabel::VerticalLabel(const QString& text, QWidget* parent) : QLabel(text, parent) {}
  void VerticalLabel::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    switch (dir_) {
      case direction::top_to_bottom:
        painter.translate(width(), 0);
        painter.rotate(90);
        painter.drawText(QRect(QPoint(0, 0), QLabel::sizeHint()), alignment(), text());
        break;
      case direction::bottom_to_top:
        painter.translate(0, height());
        painter.rotate(270);
        painter.drawText(QRect(QPoint(0, 0), QLabel::sizeHint()), alignment(), text());
        break;
      default:
        break;
    }
  }

  QSize VerticalLabel::sizeHint() const {
    QSize s = QLabel::sizeHint();
    return QSize(s.height(), s.width());
  }

  QSize VerticalLabel::minimumSizeHint() const {
    QSize s = QLabel::minimumSizeHint();
    return QSize(s.height(), s.width());
  }
  void VerticalLabel::set_direction(direction d) { dir_ = d; }

} // namespace sgl::qt