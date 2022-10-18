/**
 * \file label.hpp
 * \author Pelé Constam (you@domain.com)
 * \brief The code was taken from
 * https://stackoverflow.com/questions/9183050/vertical-qlabel-or-the-equivalent. I did not come up
 * with the idea.
 * \version 0.1
 * @date 2022-10-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SGL_QT_LABEL_HPP
#define SGL_QT_LABEL_HPP
#include "sgl/config.h"

#include <QLabel>

namespace sgl::qt {
  enum class direction {
    top_to_bottom,
    bottom_to_top,
  };

  class SGL_API VerticalLabel : public QLabel {
    Q_OBJECT
  public:
    explicit VerticalLabel(QWidget* parent = nullptr);
    VerticalLabel(const QString& text, QWidget* parent = nullptr);
    void set_direction(direction d);

  protected:
    void  paintEvent(QPaintEvent*) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

  private:
    direction dir_{direction::bottom_to_top};
  };
} // namespace sgl::qt

#endif /* SGL_QT_LABEL_HPP */
