#ifndef SGL_PAGE_HPP
#define SGL_PAGE_HPP
#include "item_traits.hpp"
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_smallest_type.hpp"
#include "sgl_string_view.hpp"
#include "sgl_tuple.hpp"


namespace sgl {

  template <typename CharT, typename... Items>
  class Page {
  public:
    static_assert((sgl::is_item_v<Items> && ...), "");
    // type aliases and typedefs
    using StringView = sgl::string_view<CharT>;

    /**
     * @brief Concrete input handler type. A page input handler is a callable
     * with a call signature equal to sgl::error(Page&, sgl::Input).
     */
    using InputHandler_t = Callable<error(sgl::Page<CharT, Items...>&, sgl::Input)>;

    /**
     * @brief Concrete page action type. Actions are callables which can be
     * invoked with a reference to a page and return sgl::error.
     */
    using PageAction_t = Callable<sgl::error(Page<CharT, Items...>&)>;

    /**
     * @brief SFINAE check if F is a page action. An instance of F must be
     * invocable with a reference to Page<CharT, Items...> and
     * return a sgl::error.
     * @tparam F invocable type
     */
    template <typename F>
    using page_action_check =
        enable_if_t<is_nothrow_invocable_r_v<sgl::error, F, Page<CharT, Items...>&>>;

    /**
     * @brief SFINAE check if F is a page input handler. F must be invocable
     with Page<CharT, Items...>& and sgl::Input, and return
     sgl::error.
     * @tparam F invocable type
     */
    template <typename F>
    using input_handler_check = enable_if_t<
        is_nothrow_invocable_r_v<error, F, Page<CharT, Items...>&, sgl::Input>>;

    /**
     * @brief SFINAE check if Menu is a menu type.
     * @tparam Menu menu type.
     */
    template <typename Menu>
    using menu_check = void;

    /**
     * @brief get item type at index I
     * @tparam I index
     */
    template <size_t I>
    using item_at_t = type_at_t<I, type_list<Items...>>;

    /// copy constructor
    constexpr Page(const Page& other) noexcept
        : items_(other.items_), input_handler_(other.input_handler_), on_enter_(other.on_enter_),
          on_exit_(other.on_exit_), name_(other.name_), title_(other.title_),
          start_edit_(other.start_edit_), stop_edit_(other.stop_edit_),
          elem_in_edit_(other.elem_in_edit_), index_(other.index_) {}

    /// move constructor
    constexpr Page(Page&& other) noexcept
        : items_(move(other.items_)), input_handler_(move(other.input_handler_)),
          on_enter_(move(other.on_enter_)), on_exit_(move(other.on_exit_)),
          name_(move(other.name_)), title_(move(other.title_)),
          start_edit_(other.start_edit_), stop_edit_(other.stop_edit_),
          elem_in_edit_(other.elem_in_edit_), index_(other.index_) {}

    // /**
    //  * @brief Page constructor with all options.
    //  * @tparam InputHandler Input handler type, see InputHandler_t and
    //  * input_handler_check.
    //  * @tparam EnterHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @tparam ExitHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param start_index current item index.
    //  * @param on_enter action to execute on entering a page.
    //  * @param on_exit action to execute on exiting a page.
    //  * @param input_handler input handler. See Page Input Handling for more
    //  * info.
    //  * @param items items of the page.
    //  */
    // template <typename InputHandler,
    //           typename EnterHandler,
    //           typename ExitHandler,
    //           input_handler_check<InputHandler>* = nullptr,
    //           page_action_check<EnterHandler>* = nullptr,
    //           page_action_check<ExitHandler>* = nullptr>
    // Page(StringView     name,
    //            StringView     title,
    //            sgl::Input     start_edit,
    //            sgl::Input     stop_edit,
    //            size_t         start_index,
    //            EnterHandler&& on_enter,
    //            ExitHandler&&  on_exit,
    //            InputHandler&& input_handler,
    //            Items&&... items) noexcept
    //     : items_(forward<Items>(items)...),
    //     input_handler_(forward<InputHandler>(input_handler)),
    //       on_enter_(forward<EnterHandler>(on_enter)),
    //       on_exit_(forward<ExitHandler>(on_exit)), name_(name), title_(title),
    //       start_edit_(start_edit), stop_edit_(stop_edit), index_(start_index) {}

    // /**
    //  * @brief Construct a page with custom input handling.
    //  * @tparam InputHandler Input handler type, see InputHandler_t and
    //  * input_handler_check.
    //  * @tparam EnterHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @tparam ExitHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param start_index current item index.
    //  * @param input_handler input handler. See Page Input Handling for more
    //  * info.
    //  * @param items items of the page.
    //  */
    // template <typename InputHandler, input_handler_check<InputHandler>* = nullptr>
    // Page(StringView     name,
    //            StringView     title,
    //            sgl::Input     start_edit,
    //            sgl::Input     stop_edit,
    //            size_t         start_index,
    //            InputHandler&& input_handler,
    //            Items&&... items) noexcept
    //     : Page(name,
    //                  title,
    //                  start_edit,
    //                  stop_edit,
    //                  start_index,
    //                  &default_page_action,
    //                  &default_page_action,
    //                  forward<InputHandler>(input_handler),
    //                  forward<Items>(items)...) {}

    // /**
    //  * @brief Construct a page with custom input handling.
    //  * @tparam InputHandler Input handler type, see InputHandler_t and
    //  * input_handler_check.
    //  * @tparam EnterHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @tparam ExitHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param input_handler input handler. See Page Input Handling for more
    //  * info.
    //  * @param items items of the page.
    //  */
    // template <typename InputHandler, input_handler_check<InputHandler>* = nullptr>
    // Page(StringView     name,
    //            StringView     title,
    //            sgl::Input     start_edit,
    //            sgl::Input     stop_edit,
    //            InputHandler&& input_handler,
    //            Items&&... items) noexcept
    //     : Page(name,
    //                  title,
    //                  start_edit,
    //                  stop_edit,
    //                  0,
    //                  &default_page_action,
    //                  &default_page_action,
    //                  forward<InputHandler>(input_handler),
    //                  forward<Items>(items)...) {}

    // /**
    //  * @brief Construct a page with default input handling but custom enter and
    //  * exit actions and custom start_index.
    //  * @tparam EnterHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @tparam ExitHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param start_index current item index.
    //  * @param on_enter action to execute on entering a page.
    //  * @param on_exit action to execute on exiting a page.
    //  * @param items items of the page.
    //  */
    // template <typename EnterHandler,
    //           typename ExitHandler,
    //           page_action_check<EnterHandler>* = nullptr,
    //           page_action_check<ExitHandler>* = nullptr>
    // Page(StringView     name,
    //            StringView     title,
    //            sgl::Input     start_edit,
    //            sgl::Input     stop_edit,
    //            size_t         start_index,
    //            EnterHandler&& on_enter,
    //            ExitHandler&&  on_exit,
    //            Items&&... items) noexcept
    //     : Page(name,
    //                  title,
    //                  start_edit,
    //                  stop_edit,
    //                  start_index,
    //                  forward<EnterHandler>(on_enter),
    //                  forward<ExitHandler>(on_exit),
    //                  &default_handle_input,
    //                  forward<Items>(items)...) {}

    // /**
    //  * @brief Construct a page with default input handling but custom enter and
    //  * exit actions. The starting index is 0.
    //  * @tparam EnterHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @tparam ExitHandler Page action type, see PageAction_t and
    //  * page_action_check.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param on_enter action to execute on entering a page.
    //  * @param on_exit action to execute on exiting a page.
    //  * @param items items of the page.
    //  */
    // template <typename EnterHandler,
    //           typename ExitHandler,
    //           page_action_check<EnterHandler>* = nullptr,
    //           page_action_check<ExitHandler>* = nullptr>
    // Page(StringView     name,
    //            StringView     title,
    //            sgl::Input     start_edit,
    //            sgl::Input     stop_edit,
    //            EnterHandler&& on_enter,
    //            ExitHandler&&  on_exit,
    //            Items&&... items) noexcept
    //     : Page(name,
    //                  title,
    //                  start_edit,
    //                  stop_edit,
    //                  0,
    //                  forward<EnterHandler>(on_enter),
    //                  forward<ExitHandler>(on_exit),
    //                  &default_handle_input,
    //                  forward<Items>(items)...) {}

    // /**
    //  * @brief Construct a page with default input handling and default
    //  * enter/exit action, but custom starting index.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param start_index current item index.
    //  * @param items items of the page.
    //  */
    // Page(StringView name,
    //            StringView title,
    //            sgl::Input start_edit,
    //            sgl::Input stop_edit,
    //            size_t     start_index,
    //            Items&&... items) noexcept
    //     : Page(name,
    //                  title,
    //                  start_edit,
    //                  stop_edit,
    //                  start_index,
    //                  &default_page_action,
    //                  &default_page_action,
    //                  &default_handle_input,
    //                  forward<Items>(items)...) {}

    // /**
    //  * @brief Construct a page with default input handling and default
    //  * enter/exit action. The starting index is 0.
    //  * @param name name of the page. Must be unique in a menu.
    //  * @param title title of the page.
    //  * @param start_edit input to start edit mode on. See Page Input Handling
    //  * for more info.
    //  * @param stop_edit input to stop edit mode on. See Page Input Handling for
    //  * more info.
    //  * @param items items of the page.
    //  */
    // Page(StringView name,
    //            StringView title,
    //            sgl::Input start_edit,
    //            sgl::Input stop_edit,
    //            Items&&... items) noexcept
    //     : Page(name,
    //                  title,
    //                  start_edit,
    //                  stop_edit,
    //                  0,
    //                  &default_page_action,
    //                  &default_page_action,
    //                  &default_handle_input,
    //                  forward<Items>(items)...) {}

    /**
     * @brief Construct a page with default input handling, default enter/exit
     * actions, default start/stop edit inputs (i.e. sgl::Input::enter).
     *
     * @code
     * // creates a page with name 'page_name'', title 'Title' and three items
     * ItemX, ItemY and ItemZ. auto page = Page("page_name","Page
     * Title",ItemX(...),ItemY(...),ItemZ(...));
     * @endcode
     *
     * @param name name of the page. Must be unique in a menu.
     * @param title title of the page.
     * @param items items of the page.
     * @{
     */
    constexpr Page(StringView name, StringView title, Items&&... items) noexcept
        : items_(move(items)...), name_(name), title_(title) {}

    constexpr Page(StringView name, StringView title, const Items&... items) noexcept
        : items_(items...), name_(name), title_(title) {}
    /// @}
    /// @}

    /// get the name of the page.
    constexpr StringView name() const noexcept { return name_; }

    /// get the title of the page.
    constexpr StringView title() const noexcept { return title_; }

    /// get number of items in the page
    constexpr size_t size() const noexcept { return sizeof...(Items); }

    /// get the index of the current item in the page. Zero based index.
    constexpr size_t index() const noexcept { return index_; }

    /// handle input. Delegates to input handler.
    constexpr sgl::error handle_input(sgl::Input i) noexcept { return input_handler_(*this, i); }

    /// get reference to item by index through the item's declared type.
    template <size_t I>
    constexpr item_at_t<I>& get_item() noexcept {
      return items_.template get<I>();
    }

    /// get const reference to item by index through the item's declared type.
    template <size_t I>
    constexpr const item_at_t<I>& get_item() const noexcept {
      return items_.template get<I>();
    }

    /// set current item by index.
    constexpr void set_item_cursor(size_t i) noexcept { index_ = i % sizeof...(Items); }

    /// query if page is in edit mode.
    constexpr bool is_in_edit_mode() const noexcept { return elem_in_edit_; }

    /// set the page in edit mode.
    constexpr void set_edit_mode() noexcept { elem_in_edit_ = true; }

    /// set the page into navigation mode.
    constexpr void set_navigation_mode() noexcept { elem_in_edit_ = false; }

    /// get the start edit input value.
    constexpr sgl::Input get_start_edit() const noexcept { return start_edit_; }

    /// set the start edit input value
    constexpr void set_start_edit(sgl::Input start_edit) noexcept { start_edit_ = start_edit; }

    /// get the stop edit input.
    constexpr sgl::Input get_stop_edit() const noexcept { return stop_edit_; }

    /// set the sop edit input value
    constexpr void set_stop_edit(sgl::Input stop_edit) noexcept { stop_edit_ = stop_edit; }

    /**
     * @brief Set the menu for items which need it, i.e. page links.
     * @tparam Menu menu type
     * @param menu pointer to menu instance
     */
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept {
      items_.for_each([menu](auto& item) { item.set_menu(menu); });
    }

    /**
     * @brief apply f on all items in the page with the items' **declared
     * type**.
     *
     * @note f must be a generic lambda like in the example below or some kind
     * of template function taking one parameter.
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
    constexpr void for_each_item(F&& f) noexcept {
      items_.for_each(forward<F>(f));
    }

    template <typename F>
    constexpr void for_each_item(F&& f) const {
      items_.for_each(forward<F>(f));
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
    constexpr void for_current_item(F&& f) {
      for_current_item_impl<0>(forward<F>(f));
    }

    template <typename F>
    constexpr void for_current_item(F&& f) const {
      for_current_item_impl<0>(forward<F>(f));
    }
    /// @}

    /// execute enter handler
    constexpr  sgl::error on_enter() { return on_enter_(*this); }

    /// execute exit handler
    constexpr sgl::error on_exit() { return on_exit_(*this); }

    /**
     * @brief Set enter action.
     * @tparam PageAction action type. See PageAction_t for more info.
     * @param action action instance
     */
    template <typename PageAction, page_action_check<PageAction>* = nullptr>
    void set_on_enter(PageAction&& action) {
      on_enter_ = forward<PageAction>(action);
    }

    /**
     * @brief Set exit action.
     * @tparam PageAction action type. See PageAction_t for more info.
     * @param action action instance
     */
    template <typename PageAction, page_action_check<PageAction>* = nullptr>
    void set_on_exit(PageAction&& action) {
      on_exit_ = forward<PageAction>(action);
    }

  private:
    /// @cond
    /// default page input handler.
    static error default_handle_input(Page<CharT, Items...>& page, sgl::Input i) {
      if ((i == page.get_start_edit() and not page.is_in_edit_mode())) {
        page.set_edit_mode();
      } else if ((i == page.get_stop_edit()) and page.is_in_edit_mode()) {
        page.set_navigation_mode();
      }
      if (page.is_in_edit_mode()) {
        // edit mode-> delegate input to current item
        error ec;
        page.for_current_item([&ec, &i](auto& item) { ec = item.handle_input(i); });
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
              page.set_item_cursor(page.index() + 1);
              break;
            case sgl::Input::up:
              [[fallthrough]];
            case sgl::Input::left:
              page.set_item_cursor(page.index() == 0 ? page.size() - 1 : page.index() - 1);
              break;
            default:
              break;
          }
        }
      }
      return error::no_error;
    }

    template <size_t I, typename F>
    constexpr void for_current_item_impl(F&& f) {
      if constexpr (I == (sizeof...(Items) - 1)) {
        f(this->get_item<I>());
      } else {
        if (index_ == I) {
          f(this->get_item<I>());
        } else {
          this->for_current_item_impl<I + 1>(forward<F>(f));
        }
      }
    }

    template <size_t I, typename F>
    constexpr void for_current_item_impl(F&& f) const {
      if constexpr (I == (sizeof...(Items) - 1)) {
        f(this->get_item<I>());
      } else {
        if (index_ == I) {
          f(this->get_item<I>());
        } else {
          this->for_current_item_impl<I + 1>(forward<F>(f));
        }
      }
    }

    constexpr static sgl::error default_page_action(Page<CharT, Items...>&) { return sgl::error::no_error; }
    /// @endcond

    tuple<Items...>                        items_;
    InputHandler_t                         input_handler_{&default_handle_input};
    PageAction_t                           on_enter_{&default_page_action};
    PageAction_t                           on_exit_{&default_page_action};
    StringView                             name_;
    StringView                             title_;
    sgl::Input                             start_edit_{sgl::Input::enter};
    sgl::Input                             stop_edit_{sgl::Input::enter};
    bool                                   elem_in_edit_{false};
    sgl::smallest_type_t<sizeof...(Items)> index_{0};
  };

  template <typename CharT, typename... Items>
  Page(const CharT*, const CharT*, Items&...) -> Page<CharT, Items...>;

  template <typename CharT, typename... Items>
  Page(const CharT*, const CharT*, const Items&...) -> Page<CharT, Items...>;
  template <typename CharT, typename... Items>
  Page(const CharT*, const CharT*, Items&&...) -> Page<CharT, Items...>;
} // namespace sgl

#endif