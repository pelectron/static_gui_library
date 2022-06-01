#ifndef SGL_PAGE_HPP
#define SGL_PAGE_HPP
#include "sgl_item.hpp"
#include "sgl_smallest_type.hpp"
#include "sgl_traits.hpp"
#include "sgl_tuple.hpp"

namespace sgl {

  /**
   * @brief This class encapsulates a page.
   * @details A page consists of the following data:
   *  - a name, which is primarily used for switching between different
   * pages without using a index. Must be unique for all pages in a menu.
   *  - a title
   *  - a 'start edit' Input value, which tells the input handler when to start
   * editing.
   *  - a 'stop edit' Input value, which tells the input handler when to stop
   * editing.
   *  - a boolean to keep track of the navigation/edit mode (true=>edit mode).
   *  - an index to keep track of the current item, i.e. a cursor.
   *  - an input handler to handle inputs.
   *  - a page enter action. This action will be called when a page becomes
   * active.
   *  - a page exit action. This action will be called when a page becomes
   * inactive.
   *  - a tuple of items.
   *
   * ## Page Input Handling
   * A page has two modes of operation. It can either be in
   *  1. **navigation mode**, where the inputs are used to navigate/scroll
   * through the items on the page.
   *  2. Or **edit mode**, where the inputs are delegated through to the active
   * item.
   *
   * A page is in navigation mode when constructed. Giving a page an input equal
   * to it's start_edit input will set the page into edit mode. The page will
   * now delegate all inputs to the current item, **including** the start_edit
   * input just received by the page, by calling the current item's
   * @ref Item_t::handle_input() "handle_input()" method.
   *
   * The page will continue delegating it's inputs until one of two conditions
   * occur:
   *
   *  1. When the page receives an input equal to it's stop_edit. The stop_edit
   * input is **not** delegated to the current item.
   *  2. When the current item's @ref Item_t::handle_input() "handle_input()"
   * call returns sgl::error::edit_finished, the page will exit edit mode.
   *
   * The reason for having two ways for notifying should be obvious:
   *
   * The first approach is for items which can be edited with multiple inputs,
   * for example a text field.
   *
   * The second one is useful for 'one and done' items, which have an action
   * associated with them but cannot truly be edited, for example a button. A
   * button you would need to press twice to de/activate would be considered
   * broken. Also immutable items which have no action associated with them can
   * just return sgl::error::edit_finished. The page's default input handler
   * will return sgl::error::no_error in this case.
   *
   * @section page_input_handling "Custom Input Handling"
   * Input handling can be customized by the end user by providing a input
   * handler in the constructor. The input handler must have a calling signature
   * of **error(sgl::Page_t&, sgl::Input)**. Keep in
   * mind that the convention of ending edit mode with a return value of
   * error::edit_finished should be kept, else **every** item input handler
   * needs to be adjusted.
   *
   *
   * ## Page Actions
   * Pages can have actions associated with them. A page action is an invocable
   * with a calling signature equal to **sgl::error(sgl::Page_t&)**. The two
   * actions available now are the **enter** and **exit** action. The enter
   * action of a page will be called whenever a menu changes to that page. The
   * exit action will be called whenever the menu changes to another page.The
   * default actions just return sgl::error::no_error.
   *
   * @tparam LineWidth line width of the page
   * @tparam CharT character type of the page
   * @tparam Items types of the items
   */
  template <size_t LineWidth, typename CharT, typename... Items>
  class Page_t {
  public:
    // type aliases and typedefs
    using ItemBase = sgl::Item_t<LineWidth, CharT>;
    using StringView = typename ItemBase::StringView;

    /**
     * @brief Concrete input handler type. A page input handler is a callable
     * with a call signature equal to sgl::error(Page_t&, sgl::Input).
     */
    using InputHandler_t =
        Callable<error(sgl::Page_t<LineWidth, CharT, Items...>&, sgl::Input)>;

    /**
     * @brief Concrete page action type. Actions are callables which can be
     * invoked with a reference to a page and return sgl::error.
     */
    using PageAction_t =
        Callable<sgl::error(Page_t<LineWidth, CharT, Items...>&)>;

    /**
     * @brief SFINAE check if F is a page action. An instance of F must be
     * invocable with a reference to Page_t<LineWidth, CharT, Items...> and
     * return a sgl::error.
     * @tparam F invocable type
     */
    template <typename F>
    using page_action_check = std::enable_if_t<
        std::is_invocable_r_v<sgl::error,
                              F,
                              Page_t<LineWidth, CharT, Items...>&>>;

    /**
     * @brief SFINAE check if F is a page input handler. F must be invocable
     with Page_t<LineWidth, CharT, Items...>& and sgl::Input, and return
     sgl::error.
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
     * @brief Set the menu for items which need it, i.e. page links.
     * @tparam Menu menu type
     * @param menu pointer to menu instance
     */
    template <typename Menu>
    void set_menu(Menu* menu) {
      items_.for_each([menu](auto& item) { item.set_menu(menu); });
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

    /**
     * @brief Set enter action.
     * @tparam PageAction action type. See PageAction_t for more info.
     * @param action action instance
     */
    template <typename PageAction, page_action_check<PageAction>* = nullptr>
    void set_on_enter(PageAction&& action) {
      if constexpr (std::is_same_v<PageAction, PageAction_t>) {
        on_enter_ = std::forward<PageAction>(action);
      } else {
        on_enter_.bind(std::forward<PageAction>(action));
      }
    }

    /**
     * @brief Set exit action.
     * @tparam PageAction action type. See PageAction_t for more info.
     * @param action action instance
     */
    template <typename PageAction, page_action_check<PageAction>* = nullptr>
    void set_on_exit(PageAction&& action) {
      if constexpr (std::is_same_v<PageAction, PageAction_t>) {
        on_exit_ = std::forward<PageAction>(action);
      } else {
        on_exit_.bind(std::forward<PageAction>(action));
      }
    }

  private:
    /// @cond
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

    static sgl::error default_page_action(Page_t<LineWidth, CharT, Items...>&) {
      return sgl::error::no_error;
    }
    /// @endcond

    tuple<Items...> items_;
    InputHandler_t  input_handler_{&default_handle_input};
    PageAction_t    on_enter_{&default_page_action};
    PageAction_t    on_exit_{&default_page_action};
    StringView      name_;
    StringView      title_;
    sgl::Input      start_edit_{sgl::Input::enter};
    sgl::Input      stop_edit_{sgl::Input::enter};
    sgl::smallest_type_t<sizeof...(Items)> index_{0};
    bool                                   elem_in_edit_{false};
  };

  template <size_t LineWidth,
            typename CharT,
            typename InputHandler,
            typename EnterHandler,
            typename ExitHandler,
            typename... Items>
  Page_t<LineWidth, CharT, Items...> make_page(string_view<CharT> name,
                                               string_view<CharT> title,
                                               sgl::Input         start_edit,
                                               sgl::Input         stop_edit,
                                               size_t             start_index,
                                               EnterHandler&&     on_enter,
                                               ExitHandler&&      on_exit,
                                               InputHandler&&     input_handler,
                                               Items&&... items) {
    return Page_t<LineWidth, CharT, Items...>(
        title,
        start_edit,
        stop_edit,
        start_index,
        std::forward<EnterHandler>(on_enter),
        std::forward<ExitHandler>(on_exit),
        std::forward<InputHandler>(input_handler),
        std::forward<Items>(items)...);
  }

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