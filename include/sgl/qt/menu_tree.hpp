//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef MENU_TREE_HPP
#define MENU_TREE_HPP
#include "sgl.hpp"

#include <cassert>
#include <string_view>
#include <vector>

namespace sgl::qt {
  template <typename T>
  struct get_type_name {
    static constexpr std::string_view value{"custom type"};
  };

  template <typename CharT, size_t Size>
  struct get_type_name<sgl::Button<Size, CharT>> {
    static constexpr std::string_view value{"sgl::Button"};
  };

  template <typename CharT, size_t Size>
  struct get_type_name<sgl::Boolean<Size, CharT>> {
    static constexpr std::string_view value{"sgl::Boolean"};
  };

  template <typename E, size_t S, typename CharT, size_t Size>
  struct get_type_name<sgl::Enum<E, S, Size, CharT>> {
    static constexpr std::string_view value{"sgl::Enum"};
  };

  template <typename CharT, size_t Size, typename T>
  struct get_type_name<sgl::Numeric<Size, CharT, T>> {
    static constexpr std::string_view value{"sgl::Numeric with T = integer"};
  };

  template <typename CharT, size_t Size>
  struct get_type_name<sgl::Numeric<Size, CharT, float>> {
    static constexpr std::string_view value{"sgl::Numeric with T = float"};
  };

  template <typename CharT, size_t Size>
  struct get_type_name<sgl::Numeric<Size, CharT, double>> {
    static constexpr std::string_view value{"sgl::Numeric with T = double"};
  };

  template <typename Name, typename CharT, size_t Size>
  struct get_type_name<sgl::PageLink<Name, Size, CharT>> {
    static constexpr std::string_view value{"sgl::PageLink"};
  };

  template <typename NameList, typename ItemList>
  struct get_type_name<sgl::Page<NameList, ItemList>> {
    static constexpr std::string_view value{"sgl::Page"};
  };

  template <typename NameList, typename PageList>
  struct get_type_name<sgl::Menu<NameList, PageList>> {
    static constexpr std::string_view value{"sgl::Menu"};
  };

  struct MenuIndex {
    size_t page_index;
    size_t item_index;
  };

  class SGL_API Node {
  public:
    enum class Type { none = 0, menu, page, item };
    using iterator = typename std::vector<Node*>::iterator;
    using const_iterator = typename std::vector<Node*>::const_iterator;
    Node(std::string_view name, Node::Type type, Node* parent = nullptr);
    Node(std::string_view   name,
         std::vector<Node*> children,
         Node::Type         type,
         Node*              parent = nullptr);
    Node(const Node&) = delete;

    virtual ~Node();

    Node*                     parent();
    [[nodiscard]] const Node* parent() const;

    void set_parent(Node* p);

    [[nodiscard]] size_t index() const;

    iterator                     begin();
    iterator                     end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;

    std::vector<Node*>&                     children();
    [[nodiscard]] const std::vector<Node*>& children() const;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] std::string_view name() const;

    [[nodiscard]] virtual std::string_view text() const;

    [[nodiscard]] virtual std::string_view type_name() const;

    [[nodiscard]] Node::Type type() const;

  private:
    Node*              parent_{nullptr};
    std::vector<Node*> children_{};
    std::string_view   name_{};
    Node::Type         type_{Type::none};
  };

  class AbstractItemNode;
  class AbstractPageNode;
  class AbstractMenuNode;

  class SGL_API AbstractItemNode : public Node {
  public:
    using Node::Node;
    AbstractPageNode*                     get_page();
    [[nodiscard]] const AbstractPageNode* get_page() const;
    bool                                  is_current() const;
  };

  class SGL_API AbstractPageNode : public Node {
  public:
    using Node::Node;
    [[nodiscard]] virtual size_t          current_index() const = 0;
    [[nodiscard]] virtual bool            edit_mode() const = 0;
    virtual void                          set_current_item(size_t i) = 0;
    AbstractMenuNode*                     get_menu();
    AbstractItemNode*                     current_item();
    [[nodiscard]] const AbstractItemNode* current_item() const;
  };

  class SGL_API AbstractMenuNode : public Node {
  public:
    using Node::Node;
    virtual sgl::error           handle_input(sgl::input i) = 0;
    [[nodiscard]] virtual size_t current_index() const = 0;
    virtual sgl::error           set_current_page(size_t index) = 0;

    AbstractPageNode*                     current_page();
    [[nodiscard]] const AbstractPageNode* current_page() const;

    AbstractItemNode*                     current_item();
    [[nodiscard]] const AbstractItemNode* current_item() const;

    [[nodiscard]] MenuIndex current_menu_index() const;
  };

  template <typename Item>
  class ItemNode : public AbstractItemNode {
  public:
    ItemNode(std::string_view name, Item& item, Node* parent = nullptr)
        : AbstractItemNode(name, Node::Type::item, parent), item_(item) {}

    [[nodiscard]] std::string_view text() const override {
      return {item_.text().data(), item_.text().size()};
    }

    [[nodiscard]] std::string_view type_name() const override { return get_type_name<Item>::value; }

  private:
    Item& item_;
  };

  template <typename Item>
  ItemNode(std::string_view, Item&, Node*) -> ItemNode<Item>;

  template <typename Page>
  class PageNode : public AbstractPageNode {
  public:
    PageNode(std::string_view name, Page& page, Node* parent)
        : AbstractPageNode(name, Type::page,parent), page_(page) {
      sgl::for_each_with_name(page_, [this](auto name, auto& item) {
        this->children().push_back(
            new ItemNode(std::string_view{name.to_view().data(), name.to_view().size()}, item, this));
      });
    }

    [[nodiscard]] size_t current_index() const override { return page_.current_item_index(); }

    [[nodiscard]] std::string_view type_name() const override { return get_type_name<Page>::value; }

    [[nodiscard]] bool edit_mode() const override { return page_.is_in_edit_mode(); }

    void set_current_item(size_t i) override { page_.set_current_item(i); }

  private:
    Page& page_;
  };

  template <typename Page>
  PageNode(std::string_view, Page&, Node*) -> PageNode<Page>;

  template <typename Menu>
  class MenuNode : public AbstractMenuNode {
  public:
    MenuNode(Menu menu) : AbstractMenuNode(std::string_view{}, Type::menu), menu_(std::move(menu)) {
      sgl::for_each_with_name(menu_, [this](auto name, auto& page) {
        this->children().push_back(
            new PageNode(std::string_view{name.to_view().data(), name.to_view().size()}, page, this));
      });
    }

    [[nodiscard]] size_t current_index() const override { return menu_.current_page_index(); }

    [[nodiscard]] std::string_view type_name() const override { return get_type_name<Menu>::value; }

    sgl::error handle_input(sgl::input i) override { return menu_.handle_input(i); }

    sgl::error set_current_page(size_t index) override { return menu_.set_current_page(index); }

  private:
    Menu menu_;
  };

  template <typename Menu>
  MenuNode(Menu) -> MenuNode<Menu>;

  // stores the user provided menu and gives type erased access.
  class SGL_API MenuTree {
  public:
    template <typename Menu>
    MenuTree(Menu m) : root_{new MenuNode<Menu>(std::move(m))} {}

    MenuTree(const MenuTree&) = delete;
    MenuTree(MenuTree&&);
    ~MenuTree();

    AbstractMenuNode*                     root();
    [[nodiscard]] const AbstractMenuNode* root() const;

    [[nodiscard]] MenuIndex active_index() const;

    AbstractItemNode* current_item();
    AbstractPageNode* current_page();

  private:
    AbstractMenuNode* root_{nullptr};
  };
} // namespace sgl::qt

#endif /* MENU_TREE_HPP */
