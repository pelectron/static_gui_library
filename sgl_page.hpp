#ifndef SGL_PAGE_HPP
#define SGL_PAGE_HPP
#include "sgl_item.hpp"
#include "sgl_traits.hpp"
#include "sgl_tuple.hpp"

namespace sgl {

  /**
   * @brief This class encapsulates a page.
   * @details
   * Data:
   * A page consists of the following data:
   *  - a name, which is primarily used for switching between different
   * pages without using a index. Must be unique for all pages in a menu.
   *  - a title
   *  - a 'start_edit' Input value, which tells the input handler when to start
   * editing.
   *  - a 'stop_edit' Input value, which tells the input handler when to stop
   * editing.
   *  - a boolean to keep track of the navigation/edit mode (true=>edit mode).
   *  - a index to keep track of the current item, i.e. a cursor.
   *  - a input handler to handle inputs.
   *  - a page enter action. This action will be called when a page becomes
   * active.
   *  - a page exit action. This action will be called when a page becomes
   * inactive.
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
    using StringView = typename ItemBase::StringView;

    /**
     * @brief Concrete input handler type.
     */
    using InputHandler_t =
        Callable<error(sgl::Page_t<LineWidth, CharT, Items...>&, sgl::Input)>;

    /**
     * @brief Concrete page action type.
     */
    using PageAction_t =
        Callable<sgl::error(Page_t<LineWidth, CharT, Items...>&)>;

    /**
     * @brief SFINAE check if F is a page action. An instance of F must be invocable
     * with a reference to Page_t<LineWidth, CharT, Items...> and return a
     * sgl::error.
     * @tparam F invocable type
     */
    template <typename F>
    using page_action_check = std::enable_if_t<
        std::is_invocable_r_v<sgl::error,
                              F,
                              Page_t<LineWidth, CharT, Items...>&>>;

    /**
     * @brief SFINAE check if F is a page input handler. F must be invocable with
     Page_t<LineWidth, CharT, Items...>& and sgl::Input, and return sgl::error.
     * @tparam F invocable type
     */
    template <typename F>
    using input_handler_check = std::enable_if_t<
        std::is_invocable_r_v<error,
                              F,
                              Page_t<LineWidth, CharT, Items...>&,
                              sgl::Input>>;
    
    template <typename... T>
    using item_check = std::enable_if_t<(sgl::is_item_v<T> && ...)>;

    /**
     * @brief SFINAE check if Menu is a menu type.
     * @tparam Menu menu type.
     */
    template <typename Menu>
    using menu_check = std::enable_if_t<sgl::is_menu_v<Menu>>;

    /**
     * @brief get item type at index I
     * @tparam I index
     */
    template <size_t I>
    using item_at_t = type_at_t<I, type_list<Items...>>;

    /// copy constructor
    Page_t(const Page_t& other)
        : items_(other.items_), input_handler_(other.input_handler_),
          on_enter_(other.on_enter_), on_exit_(other.on_exit_),
          name_(other.name_), title_(other.title_),
          start_edit_(other.start_edit_), stop_edit_(other.stop_edit_),
          index_(other.index_), elem_in_edit_(other.elem_in_edit_) {}

    /// move constructor
    Page_t(Page_t&& other)
        : items_(std::move(other.items_)),
          input_handler_(std::move(other.input_handler_)),
          on_enter_(std::move(other.on_enter_)),
          on_exit_(std::move(other.on_exit_)), name_(std::move(other.name_)),
          title_(std::move(other.title_)), start_edit_(other.start_edit_),
          stop_edit_(other.stop_edit_), index_(other.index_),
          elem_in_edit_(other.elem_in_edit_) {}

    /**
     * @brief Page constructor with all options.
     * @tparam InputHandler Input handler type, see InputHandler_t and
     * input_handler_check.
     * @tparam EnterHandler Page action type, see PageAction_t and
     * page_action_check.
     * @tparam ExitHandler Page action type, see PageAction_t and
     * page_action_check.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param start_index current item index.
     * @param on_enter action to execute on entering a page.
     * @param on_exit action to execute on exiting a page.
     * @param input_handler input handler. See Page Input Handling for more
     * info.
     * @param items items of the page.
     */
    template <typename InputHandler,
              typename EnterHandler,
              typename ExitHandler,
              input_handler_check<InputHandler>* = nullptr,
              page_action_check<EnterHandler>* = nullptr,
              page_action_check<ExitHandler>* = nullptr>
    Page_t(StringView     name,
           StringView     title,
           sgl::Input     start_edit,
           sgl::Input     stop_edit,
           size_t         start_index,
           EnterHandler&& on_enter,
           ExitHandler&&  on_exit,
           InputHandler&& input_handler,
           Items&&... items)
        : items_(std::forward<Items>(items)...),
          input_handler_(std::forward<InputHandler>(input_handler)),
          on_enter_(std::forward<EnterHandler>(on_enter)),
          on_exit_(std::forward<ExitHandler>(on_exit)), name_(name),
          title_(title), start_edit_(start_edit), stop_edit_(stop_edit),
          index_(start_index) {}

    /**
     * @brief Construct a page with custom input handling.
     * @tparam InputHandler Input handler type, see InputHandler_t and
     * input_handler_check.
     * @tparam EnterHandler Page action type, see PageAction_t and
     * page_action_check.
     * @tparam ExitHandler Page action type, see PageAction_t and
     * page_action_check.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param start_index current item index.
     * @param input_handler input handler. See Page Input Handling for more
     * info.
     * @param items items of the page.
     */
    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Page_t(StringView     name,
           StringView     title,
           sgl::Input     start_edit,
           sgl::Input     stop_edit,
           size_t         start_index,
           InputHandler&& input_handler,
           Items&&... items)
        : Page_t(name,
                 title,
                 start_edit,
                 stop_edit,
                 start_index,
                 &default_page_action,
                 &default_page_action,
                 std::forward<InputHandler>(input_handler),
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with custom input handling.
     * @tparam InputHandler Input handler type, see InputHandler_t and
     * input_handler_check.
     * @tparam EnterHandler Page action type, see PageAction_t and
     * page_action_check.
     * @tparam ExitHandler Page action type, see PageAction_t and
     * page_action_check.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param input_handler input handler. See Page Input Handling for more
     * info.
     * @param items items of the page.
     */
    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Page_t(StringView     name,
           StringView     title,
           sgl::Input     start_edit,
           sgl::Input     stop_edit,
           InputHandler&& input_handler,
           Items&&... items)
        : Page_t(name,
                 title,
                 start_edit,
                 stop_edit,
                 0,
                 &default_page_action,
                 &default_page_action,
                 std::forward<InputHandler>(input_handler),
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with default input handling but custom enter and
     * exit actions and custom start_index.
     * @tparam EnterHandler Page action type, see PageAction_t and
     * page_action_check.
     * @tparam ExitHandler Page action type, see PageAction_t and
     * page_action_check.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param start_index current item index.
     * @param on_enter action to execute on entering a page.
     * @param on_exit action to execute on exiting a page.
     * @param items items of the page.
     */
    template <typename EnterHandler,
              typename ExitHandler,
              page_action_check<EnterHandler>* = nullptr,
              page_action_check<ExitHandler>* = nullptr>
    Page_t(StringView     name,
           StringView     title,
           sgl::Input     start_edit,
           sgl::Input     stop_edit,
           size_t         start_index,
           EnterHandler&& on_enter,
           ExitHandler&&  on_exit,
           Items&&... items)
        : Page_t(name,
                 title,
                 start_edit,
                 stop_edit,
                 start_index,
                 std::forward<EnterHandler>(on_enter),
                 std::forward<ExitHandler>(on_exit),
                 &default_handle_input,
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with default input handling but custom enter and
     * exit actions. The starting index is 0.
     * @tparam EnterHandler Page action type, see PageAction_t and
     * page_action_check.
     * @tparam ExitHandler Page action type, see PageAction_t and
     * page_action_check.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param on_enter action to execute on entering a page.
     * @param on_exit action to execute on exiting a page.
     * @param items items of the page.
     */
    template <typename EnterHandler,
              typename ExitHandler,
              page_action_check<EnterHandler>* = nullptr,
              page_action_check<ExitHandler>* = nullptr>
    Page_t(StringView     name,
           StringView     title,
           sgl::Input     start_edit,
           sgl::Input     stop_edit,
           EnterHandler&& on_enter,
           ExitHandler&&  on_exit,
           Items&&... items)
        : Page_t(name,
                 title,
                 start_edit,
                 stop_edit,
                 0,
                 std::forward<EnterHandler>(on_enter),
                 std::forward<ExitHandler>(on_exit),
                 &default_handle_input,
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with default input handling and default
     * enter/exit action, but custom starting index.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param start_index current item index.
     * @param items items of the page.
     */
    Page_t(StringView name,
           StringView title,
           sgl::Input start_edit,
           sgl::Input stop_edit,
           size_t     start_index,
           Items&&... items)
        : Page_t(name,
                 title,
                 start_edit,
                 stop_edit,
                 start_index,
                 &default_page_action,
                 &default_page_action,
                 &default_handle_input,
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with default input handling and default
     * enter/exit action. The starting index is 0.
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param start_edit input to start edit mode on. See Page Input Handling
     * for more info.
     * @param stop_edit input to stop edit mode on. See Page Input Handling for
     * more info.
     * @param items items of the page.
     */
    Page_t(StringView name,
           StringView title,
           sgl::Input start_edit,
           sgl::Input stop_edit,
           Items&&... items)
        : Page_t(name,
                 title,
                 start_edit,
                 stop_edit,
                 0,
                 &default_page_action,
                 &default_page_action,
                 &default_handle_input,
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with default input handling, default enter/exit
     * actions, default start/stop edit inputs (i.e. sgl::Input::enter).
     *
     * @code
     * // creates a page with name 'page_name'', title 'Title' and three items
     * ItemX, ItemY and ItemZ. auto page = Page_t("page_name","Page
     * Title",ItemX(...),ItemY(...),ItemZ(...));
     * @endcode
     *
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param items items of the page.
     */
    Page_t(StringView name, StringView title, Items&&... items)
        : Page_t(name,
                 title,
                 sgl::Input::enter,
                 sgl::Input::enter,
                 0,
                 &default_page_action,
                 &default_page_action,
                 &default_handle_input,
                 std::forward<Items>(items)...) {}

    /**
     * @brief Construct a page with a pointer to menu that will contain that
     * page. This will set the menu on all items in the page. This is used in
     * the Menu_t constructors.
     * @tparam Menu menu type
     * @param menu pointer to menu instance.
     * @param other original page
     * @{
     */
    template <typename Menu, menu_check<Menu>* = nullptr>
    Page_t(Menu* menu, const Page_t& other) : Page_t(other) {
      for_each_item([m = menu](auto& item) { item.set_menu(m); });
    }
    template <typename Menu, menu_check<Menu>* = nullptr>
    Page_t(Menu* menu, Page_t&& other) : Page_t(std::move(other)) {
      for_each_item([m = menu](auto& item) { item.set_menu(m); });
    }
    /// @}

    /// get number of items in the page
    constexpr size_t size() const { return sizeof...(Items); }

    /// get the index of the current item in the page. Zero based index.
    constexpr size_t index() const { return index_; }

    /// handle input. Delegates to input handler.
    error handle_input(sgl::Input i) { return input_handler_(*this, i); }

    /// get reference to current item as ItemBase, a.k.a.
    /// sgl::Item_t<LineWidth, CharT>.
    ItemBase& current_item() noexcept { return get_impl<0>(index_); }

    const ItemBase& current_item() const noexcept {
      return cget_impl<0>(index_);
    }

    /// get reference to item by index through ItemBase.
    ItemBase& operator[](size_t i) { return get_impl<0>(i % sizeof...(Items)); }

    /// get const reference to item by index through ItemBase.
    const ItemBase& operator[](size_t i) const {
      return cget_impl<0>(i % sizeof...(Items));
    }

    /// get reference to item by index through the item's declared type.
    template <size_t I>
    item_at_t<I>& get_item() {
      return items_.template get<I>();
    }

    /// get const reference to item by index through the item's declared type.
    template <size_t I>
    const item_at_t<I>& get_item() const {
      return items_.template get<I>();
    }

    /// set current item by index.
    void set_current_item(size_t i) { index_ = i % sizeof...(Items); }

    /// query if page is in edit mode.
    bool is_in_edit_mode() const { return elem_in_edit_; }

    /// set the page in edit mode.
    void set_edit_mode() { elem_in_edit_ = true; }

    /// set the page into navigation mode.
    void set_navigation_mode() { elem_in_edit_ = false; }

    /// get the start edit input value.
    sgl::Input get_start_edit() const { return start_edit_; }

    /// set the start edit input value
    void set_start_edit(sgl::Input start_edit) { start_edit_ = start_edit; }

    /// get the stop edit input.
    sgl::Input get_stop_edit() const { return stop_edit_; }

    /// set the sop edit input value
    void set_stop_edit(sgl::Input stop_edit) { stop_edit_ = stop_edit; }

    /// get the name of the page.
    StringView get_name() const { return name_; }

    /// get the title of the page.
    StringView get_title() const { return title_; }

    /**
     * @brief Set the menu for items which need it, i.e. page links
     *
     * @tparam Menu
     * @param menu
     */
    template <typename Menu>
    void set_menu(Menu* menu) {
      set_menu_impl(menu, sgl::index_sequence_for<Items...>{});
    }

    /**
     * @brief apply f on all items in the page with the items' **declared
     * type**.
     *
     * @note f must be a generic lambda like in the example below or some kind
     * of template.
     *
     * @code
     * auto page_var = make_page(...);
     * page_var.for_current_item([](auto& page){...});
     * @endcode
     *
     * @tparam F invocable type
     * @param f callable
     * @{
     */
    template <typename F>
    void for_each_item(F&& f) {
      items_.for_each(std::forward<F>(f));
    }

    template <typename F>
    void for_each_item(F&& f) const {
      items_.for_each(std::forward<F>(f));
    }
    /// @}

    /**
     * @brief apply f on current item with it's **declared type**. An example
     * below.
     *
     * @note f must be a generic lambda like in the example below or some kind
     * of template.
     *
     * @code
     * auto page_var = make_page(...);
     * page_var.for_current_item([](auto& page){...});
     * @endcode
     *
     * @tparam F invocable type
     * @param f callable
     * @{
     */
    template <typename F>
    void for_current_item(F&& f) {
      for_current_item<0>(std::forward<F>(f));
    }

    template <typename F>
    void for_current_item(F&& f) const {
      for_current_item<0>(std::forward<F>(f));
    }
    /// @}

    /// execute enter handler
    sgl::error on_enter() { return on_enter_(*this); }

    /// execute exit handler
    sgl::error on_exit() { return on_exit_(*this); }

    template <typename PageAction, page_action_check<PageAction>* = nullptr>
    void set_on_enter(PageAction&& action) {
      if constexpr (std::is_same_v<PageAction, PageAction_t>) {
        on_enter_ = std::forward<PageAction>(action);
      } else {
        on_enter_.bind(std::forward<PageAction>(action));
      }
    }

    template <typename PageAction, page_action_check<PageAction>* = nullptr>
    void set_on_exit(PageAction&& action) {
      if constexpr (std::is_same_v<PageAction, PageAction_t>) {
        on_exit_ = std::forward<PageAction>(action);
      } else {
        on_exit_.bind(std::forward<PageAction>(action));
      }
    }

  private:
    /// default page input handler.
    static error default_handle_input(Page_t<LineWidth, CharT, Items...>& page,
                                      sgl::Input                          i) {
      if ((i == page.get_start_edit() and not page.is_in_edit_mode())) {
        page.set_edit_mode();
      } else if ((i == page.get_stop_edit()) and page.is_in_edit_mode()) {
        page.set_navigation_mode();
      }
      if (page.is_in_edit_mode()) {
        // edit mode-> delegate input to current item
        error ec = page.current_item().handle_input(i);
        switch (ec) {
          case error::edit_finished:
            page.set_navigation_mode();
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

    template <size_t I>
    ItemBase& get_impl(size_t i) noexcept {
      if constexpr (I == (sizeof...(Items) - 1)) {
        return static_cast<ItemBase&>(items_.template get<I>());
      } else {
        if (I == i) {
          return static_cast<ItemBase&>(items_.template get<I>());
        }
        return get_impl<I + 1>(i);
      }
    }

    template <size_t I>
    const ItemBase& cget_impl(size_t i) const noexcept {
      if constexpr (I == (sizeof...(Items) - 1)) {
        return static_cast<const ItemBase&>(items_.template get<I>());
      } else {
        if (I == i) {
          return static_cast<const ItemBase&>(items_.template get<I>());
        }
        return cget_impl<I + 1>(i);
      }
    }
    template <size_t I, typename F>
    void for_current_item_impl(F&& f) {
      if constexpr (I ==
                    number_of_items_v<Page_t<LineWidth, CharT, Items...>>) {
        return;
      } else {
        if (index_ == I) {
          f(this->get_item<I>());
        } else {
          for_current_item_impl<I + 1>(std::forward<F>(f));
        }
      }
    }
    template <size_t I, typename F>
    void for_current_item_impl(F&& f) const {
      if constexpr (I ==
                    number_of_items_v<Page_t<LineWidth, CharT, Items...>>) {
        return;
      } else {
        if (index_ == I) {
          f(this->get_item<I>());
        } else {
          for_current_item_impl<I + 1>(std::forward<F>(f));
        }
      }
    }
    template <typename Menu, size_t... I>
    void set_menu_impl(Menu* menu, index_seq_t<I...>) {
      (items_.template get<I>().template set_menu<Menu>(menu), ...);
    }

    static sgl::error default_page_action(Page_t<LineWidth, CharT, Items...>&) {
      return sgl::error::no_error;
    }

    tuple<Items...> items_;
    InputHandler_t  input_handler_{&default_handle_input};
    PageAction_t    on_enter_{&default_page_action};
    PageAction_t    on_exit_{&default_page_action};
    StringView      name_;
    StringView      title_;
    sgl::Input      start_edit_{sgl::Input::enter};
    sgl::Input      stop_edit_{sgl::Input::enter};
    size_t          index_{0};
    bool            elem_in_edit_{false};
  };

  template <size_t LineWidth,
            typename CharT,
            typename InputHandler,
            typename... Items,
            typename = std::enable_if_t<
                std::is_invocable_r_v<error,
                                      InputHandler,
                                      Page_t<LineWidth, CharT, Items...>&,
                                      Input>>>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> name,
                                               string_view<CharT> title,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               InputHandler&&     input_handler,
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
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               Items&&... items) {
    return Page_t<LineWidth, CharT, Items...>(name,
                                              title,
                                              start_edit,
                                              stop_edit,
                                              std::forward<Items>(items)...);
  }

  template <size_t LineWidth, typename CharT, typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> name,
                                               string_view<CharT> title,
                                               Items&&... items) {
    return Page_t<LineWidth, CharT, Items...>(name,
                                              title,
                                              std::forward<Items>(items)...);
  }
} // namespace sgl
#endif