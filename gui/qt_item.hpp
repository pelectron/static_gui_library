#ifndef QT_ITEM_HPP
#define QT_ITEM_HPP
#include "section.hpp"
#include "sgl/sgl.hpp"

#include <QFormLayout>
#include <QLabel>
#include <QWidget>
#include <string>
#include <string_view>

namespace sgl::qt {

  class ItemConcept;
  class Page;

  class Item : public Section {
    Q_OBJECT
  public:
    template <typename MenuItem>
    Item(std::string_view item_name, MenuItem& item, sgl::qt::Page* parent = nullptr);

    ~Item();

    [[nodiscard]] std::string_view name() const;
    [[nodiscard]] std::string_view content() const;
    [[nodiscard]] std::string_view type() const;

    void update_text();

  private:
    ItemConcept* impl_;
    QLabel*      content_label_;
  };

  template <typename T>
  constexpr std::string_view item_typename() {
    return "custom";
  }

  class ItemConcept {
  public:
    [[nodiscard]] virtual std::string_view name() const = 0;
    [[nodiscard]] virtual std::string_view content() const = 0;
    [[nodiscard]] virtual std::string_view type() const = 0;
    virtual ~ItemConcept() = 0;
  };

  template <typename MenuItem>
  class ItemModel : public ItemConcept {
    MenuItem*        item_;
    std::string_view name_;

  public:
    ItemModel(std::string_view item_name, MenuItem& item) : item_(&item), name_(item_name) {}

    [[nodiscard]] std::string_view name() const override { return name_; }

    [[nodiscard]] std::string_view content() const override {
      const auto& text = item_->text();
      return {text.data(), text.size()};
    }

    [[nodiscard]] std::string_view type() const override {
      return sgl::qt::item_typename<MenuItem>();
    }
    ~ItemModel() override {}
  };

  template <typename MenuItem>
  Item::Item(std::string_view item_name, MenuItem& item, sgl::qt::Page* parent)
      : Section(QString(item.text().data()), parent),
        impl_(new ItemModel<MenuItem>(item_name, item)) {
    auto* list_layout{new QVBoxLayout};
    auto* type_layout{new QHBoxLayout};
    auto* name_layout{new QHBoxLayout};
    this->body()->setLayout(list_layout);
    this->body()->setFocusPolicy(Qt::NoFocus);
    this->section_button()->setFocusPolicy(Qt::NoFocus);
    
    // |= contents        |
    // |------------------|
    // | name : item name |
    // | type : type name |

    auto* label = new QLabel("name :");
    label->setFocusPolicy(Qt::NoFocus);
    name_layout->addWidget(label);

    label = new QLabel(QString(item_name.data()));
    label->setFocusPolicy(Qt::NoFocus);
    name_layout->addWidget(label);

    label = new QLabel("type :");
    label->setFocusPolicy(Qt::NoFocus);
    type_layout->addWidget(label);

    label = new QLabel(QString(item_typename<MenuItem>().data()));
    label->setFocusPolicy(Qt::NoFocus);
    type_layout->addWidget(label);

    list_layout->addLayout(name_layout);
    list_layout->addLayout(type_layout);

    this->setFocusPolicy(Qt::NoFocus);
  }
} // namespace sgl::qt

#endif /* QT_ITEM_HPP */
