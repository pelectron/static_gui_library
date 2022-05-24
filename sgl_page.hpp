#ifndef SGL_PAGE_HPP
#define SGL_PAGE_HPP
#include "sgl_input.hpp"
#include "sgl_item.hpp"
#include "string_view.hpp"
#include "tuple.hpp"

namespace sgl {

  /**
   * @brief This class encapsulates a page. A page is simply a list of
   * items/items.
   * Data:
   * A page consists of the following data:
   *  - a name, which is primarily used for switching between different pages
   * without using a index. Must be unique for all pages in a menu.
   *  - a title
   *  - a 'start_edit' Input value, which tells the input handler when to start
   * editing.
   *  - a 'stop_edit' Input value, which tells the input handler when to stop
   * editing.
   *  - a boolean to keep track of the navigation/edit mode (true=>edit mode).
   *  - a index to keep track of the current item, i.e. a cursor.
   *  - a input handler to handle inputs.
   *  - a tuple of items/items.
   *
   * A page has two modes: navigation mode and edit mode. In the
   * navigation mode, the inputs are used to navigate a page. In edit mode, the
   * inputs are delegated to the current item in order to edit it.
   *
   * Default Input Handling:
   * On construction, a page is in navigation mode. Input::up, Input::down,
   * Input::left and Input::right are used to scroll through the items in
   * navigation mode and set the active item accordingly.
   * A change into edit mode can be achieved by giving the page the 'start_edit'
   * input. Now all inputs are directly delegated to the active item.
   * Changing back into navigation mode can happen in two ways:
   *  1. if the page receives a 'stop_edit' input, the page will stop delegating
   * to the active item. The page is now in navigation mode again.
   *  2. if the active item returns an error value equal to error::edit_finished
   * when handling the given input, the page will stop delegating and change
   * back into navigation mode.
   *
   * The reason for stopping the edit mode in two ways is the following:
   * Editable items work good with the first approach. For example: start
   * editing with Input::enter, then input characters, then Input::enter again
   * to stop editing. For items which can not be edited, i.e. immutable text, or
   * which can only be activated, i.e. a button, this approach does not work. It
   * would need two correct inputs to activate a button properly, or worse, two
   * correct inputs to stop editing a item which cannot be edited in the first
   * place. With the second approach, this is resolved.
   *
   *
   * Custom Input Handling:
   * Input handling can be customized by the end user by providing a input
   * handler in the constructor. The input handler must have a calling signature
   * of 'error(sgl::Page_t<LineWidth, CharT, Items...>&, sgl::Input)'. Keep in
   * mind that the convention of ending edit mode with a return value of
   * error::edit_finished should be kept, else every item input handler needs to
   * be adjusted.
   * @tparam LineWidth line width of the page
   * @tparam CharT character type of the page
   * @tparam Items types of the items/items
   */
  template <size_t LineWidth, typename CharT, typename... Items>
  class Page_t {
  public:
    // type aliases and typedefs
    using ItemBase = sgl::Item_t<LineWidth, CharT>;
    using InputHandler_t =
        Callable<error(sgl::Page_t<LineWidth, CharT, Items...>&, sgl::Input)>;
    template <typename T>
    using input_handler_check = std::enable_if_t<
        std::is_invocable_r_v<error,
                              T,
                              Page_t<LineWidth, CharT, Items...>&,
                              sgl::Input>>;
    template <size_t I>
    using type_at_index = type_at_t<I, type_list<Items...>>;
    // constructors

    Page_t(const Page_t& other)
        : name_(other.name_), title_(other.title_),
          start_edit_(other.start_edit_), stop_edit_(other.stop_edit_),
          elem_in_edit_(other.elem_in_edit_), index_(other.index_),
          input_handler_(other.input_handler_), items_(other.items_) {}
    Page_t(Page_t<LineWidth, CharT, Items...>&& other)
        : name_(std::move(other.name_)), title_(std::move(other.title_)),
          start_edit_(std::move(other.start_edit_)),
          stop_edit_(other.stop_edit_), elem_in_edit_(other.elem_in_edit_),
          index_(other.index_), input_handler_(std::move(other.input_handler_)),
          items_(std::move(other.items_)) {}

    /**
     * @brief Construct a new Page_t object
     *
     * @tparam InputHandler
     * @param name name of the page. Must be unique per menu.
     * @param title title of the page
     * @param start_edit Input that signals to start editing
     * @param stop_edit Input that signals to stop editing
     * @param input_handler custom input handler
     * @param items page items
     */
    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Page_t(string_view<CharT> name,
           string_view<CharT> title,
           sgl::Input               start_edit,
           sgl::Input               stop_edit,
           InputHandler&&           input_handler,
           Items... items)
        : name_(name), title_(title), start_edit_(start_edit),
          stop_edit_(stop_edit),
          input_handler_(std::forward<InputHandler>(input_handler)),
          items_(std::forward<Items>(items)...) {}

    /**
     * @brief Construct a new Page_t with default input handling.
     *
     * @param name name of the page. Must be unique per menu.
     * @param title title of the page.
     * @param start_edit Input that signals to start editing
     * @param stop_edit Input that signals to stop editing
     * @param items page items
     */
    Page_t(string_view<CharT> name,
           string_view<CharT> title,
           sgl::Input               start_edit,
           sgl::Input               stop_edit,
           Items&&... items)
        : name_(name), title_(title), start_edit_(start_edit),
          stop_edit_(stop_edit), items_(std::forward<Items>(items)...) {}

    /// get number of items in the page
    constexpr size_t size() const { return sizeof...(Items); }

    /// get the index of the current item in the page. Zero based index.
    constexpr size_t index() const { return index_; }

    /// handle input. Delegates to input handler.
    error handle_input(sgl::Input i) { return input_handler_(*this, i); }

    /// get reference to current item as ElementBase, a.k.a.
    /// sgl::Item_t<LineWidth, CharT>.
    ItemBase& current_item() { return get_impl<0>(index_); }

    /// get reference to item by index through ItemBase.
    ItemBase& operator[](size_t i) { return get_impl<0>(i % sizeof...(Items)); }

    /// get const reference to item by index through ItemBase.
    const ItemBase& operator[](size_t i) const {
      return cget_impl<0>(i % sizeof...(Items));
    }

    /// get reference to item by index through the item's declared type.
    template <size_t I>
    type_at_index<I>& get_item() {
      return items_.template get<I>();
    }

    /// get const reference to item by index through the item's declared type.
    template <size_t I>
    const type_at_index<I>& get_item() const {
      return items_.template get<I>();
    }

    /// set current item by index.
    void set_current_item(size_t i) { index_ = i % sizeof...(Items); }

    /// query if page is in edit mode.
    bool is_in_edit_mode() const { return elem_in_edit_; }

    /// set the page in edit mode.
    void set_edit_mode() { elem_in_edit_ = true; }

    /// set the page into navigation mode.
    void unset_edit_mode() { elem_in_edit_ = false; }

    /// get the start edit input value.
    sgl::Input start_edit_input() const { return start_edit_; }

    /// get the stop edit input.
    sgl::Input stop_edit_input() const { return stop_edit_; }

    /// get the name of the page.
    string_view<CharT> get_name() const { return name_; }

    /// get the title of the page.
    string_view<CharT> get_title() const { return title_; }

    /**
     * @brief Set the menu for items which need it, i.e. page links
     *
     * @tparam Menu
     * @param menu
     */
    template <typename Menu>
    void set_menu(Menu* menu) {
      set_menu_impl<Menu, 0>(menu);
    }
    /// default page input handler.
    static error default_handle_input(Page_t<LineWidth, CharT, Items...>& page,
                                      sgl::Input                          i) {
      if ((i == page.start_edit_input() and not page.is_in_edit_mode())) {
        page.set_edit_mode();
      } else if ((i == page.stop_edit_input()) and page.is_in_edit_mode()) {
        page.unset_edit_mode();
      }
      if (page.is_in_edit_mode()) {
        // edit mode-> delegate input to current item
        error ec = page.current_item().handle_input(i);
        switch (ec) {
          case error::edit_finished:
            page.unset_edit_mode();
            return error::no_error;
          default:
            return ec;
        }
      } else {
        // page navigation mode -> move up/down items_ with keypad
        if (!is_keyboard_input(i)) {
          switch (i) {
            case sgl::Input::down:
              [[fallthrough]];
            case sgl::Input::right:
              page.set_current_item(page.index() + 1);
              break;
            case sgl::Input::up:
              [[fallthrough]];
            case sgl::Input::left:
              page.set_current_item(page.index() == 0 ? page.size() - 1
                                                      : page.index() - 1);
              break;
            default:
              break;
          }
        }
      }
      return error::no_error;
    }

  private:
    template <size_t I>
    ItemBase& get_impl(size_t i) {
      if (I == i) {
        return static_cast<ItemBase&>(items_.template get<I>());
      }
      return get_impl<I + 1>(i);
    }
    template <>
    ItemBase& get_impl<sizeof...(Items) - 1>(size_t i) {
      return static_cast<ItemBase&>(
          items_.template get<sizeof...(Items) - 1>());
    }
    template <size_t I>
    const ItemBase& cget_impl(size_t i) const {
      if (I == i) {
        return static_cast<const ItemBase&>(items_.template get<I>());
      }
      return cget_impl<I + 1>(i);
    }
    template <>
    const ItemBase& cget_impl<sizeof...(Items) - 1>(size_t i) const {
      return static_cast<const ItemBase&>(
          items_.template get<sizeof...(Items) - 1>());
    }
    template <typename Menu, size_t I>
    void set_menu_impl(Menu* menu) {
      if constexpr (((sizeof...(Items) - 1) == I)) {
        items_.template get<I>().template set_menu<Menu>(menu);
      } else {
        items_.template get<I>().template set_menu<Menu>(menu);
        this->set_menu_impl<Menu, I + 1>(menu);
      }
    }

    string_view<CharT> name_;
    string_view<CharT> title_;
    sgl::Input               start_edit_{sgl::Input::enter};
    sgl::Input               stop_edit_{sgl::Input::enter};
    bool                     elem_in_edit_{false};
    size_t                   index_{0};
    InputHandler_t           input_handler_{
        &Page_t<LineWidth, CharT, Items...>::default_handle_input};
    tuple<Items...> items_;
  };

  template <size_t LineWidth,
            typename CharT,
            typename InputHandler,
            typename... Items,
            typename std::enable_if_t<
                std::is_invocable_r_v<error,
                                      InputHandler,
                                      Page_t<LineWidth, CharT, Items...>&,
                                      Input>>* = nullptr>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> name,
                                               string_view<CharT> title,
                                               sgl::Input     start_edit,
                                               sgl::Input     stop_edit,
                                               InputHandler&& input_handler,
                                               Items&&... items) {
    return Page_t<LineWidth, CharT, Items...>(
        name,
        title,
        start_edit,
        stop_edit,
        std::forward<InputHandler>(input_handler),
        std::forward<Items>(items)...);
  }
  template <size_t LineWidth, typename CharT, typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> name,
                                               string_view<CharT> title,
                                               sgl::Input start_edit,
                                               sgl::Input stop_edit,
                                               Items&&... items) {
    return Page_t<LineWidth, CharT, Items...>(name,
                                              title,
                                              start_edit,
                                              stop_edit,
                                              std::forward<Items>(items)...);
  }

} // namespace sgl
#endif