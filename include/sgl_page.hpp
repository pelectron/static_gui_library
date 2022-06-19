#ifndef SGL_PAGE_HPP
#define SGL_PAGE_HPP
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_item_concepts.hpp"
#include "sgl_smallest_type.hpp"
#include "sgl_string_view.hpp"
#include "sgl_tuple.hpp"
#include "sgl_type_traits.hpp"
namespace sgl {

  template <typename CharT, typename... Items>
  class Page {
  public:
    static_assert((sgl::is_item_v<Items> && ...), "");
    // type aliases and typedefs
    using StringView = sgl::string_view<CharT>;

    /**
     * \brief Concrete input handler type. A page input handler is a callable
     * with a call signature equal to sgl::error(Page&, sgl::Input).
     */
    using InputHandler_t = Callable<error(sgl::Page<CharT, Items...>&, sgl::Input)>;

    /**
     * \brief Concrete page action type. Actions are callables which can be
     * invoked with a reference to a page and return sgl::error.
     */
    using PageAction_t = Callable<sgl::error(Page<CharT, Items...>&)>;

    /// predicate that checks if F conforms to the PageAction concept, i.e. an instance of F must be
    /// nothrow invocable with a reference to Page<CharT, Items...> and return an sgl::error value.
    template <typename F>
    static constexpr bool PageAction =
        std::is_nothrow_invocable_r_v<sgl::error, F, Page<CharT, Items...>&>;

    /// predicate that checks if F conforms to the PageInputHandler concept, i.e. an instance of F
    /// must be nothrow invocable with a reference to Page<CharT, Items...> and an sgl::Input value
    /// and return an sgl::error value.
    template <typename F>
    static constexpr bool PageInputHandler =
        std::is_nothrow_invocable_r_v<sgl::error, F, Page<CharT, Items...>&, sgl::Input>;

    /**
     * \brief get item type at index I
     * \tparam I index
     */
    template <size_t I>
    using item_at_t = type_at_t<I, type_list<Items...>>;

    template <typename... P>
    static constexpr bool nothrow_move_constructible_v =
        (std::is_nothrow_constructible_v<std::decay_t<P>,
                                         std::add_rvalue_reference<std::decay_t<P>>> &&
         ...);

    template <typename... P>
    static constexpr bool nothrow_copy_constructible_v =
        (std::is_nothrow_constructible_v<std::decay_t<P>, const P&> && ...);
    static constexpr bool nothrow_copy_constructible =
        std::is_nothrow_copy_constructible_v<sgl::tuple<Items...>>;
    static constexpr bool nothrow_move_constructible =
        std::is_nothrow_move_constructible_v<sgl::tuple<Items...>>;
    /// copy constructor
    constexpr Page(const Page& other) noexcept(nothrow_copy_constructible);

    /// move constructor
    constexpr Page(Page&& other) noexcept(nothrow_move_constructible);

    /**
     * \brief Construct a page with default input handling, default enter/exit
     * actions, default start/stop edit inputs (i.e. sgl::Input::enter).
     *
     * \code
     *  // creates a page with name 'page_name'', title 'Title' and three items
     *  // ItemX, ItemY and ItemZ.
     *  auto page = Page("page_name","Title",ItemX(...),ItemY(...),ItemZ(...));
     * \endcode
     *
     * \param name name of the page. Must be unique in a menu.
     * \param title title of the page.
     * \param items items of the page.
     * \{
     */
    constexpr Page(StringView name,
                   StringView title,
                   Items&&... items) noexcept(nothrow_move_constructible);

    constexpr Page(StringView name,
                   StringView title,
                   const Items&... items) noexcept(nothrow_copy_constructible);
    /// \}

    ///\{
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   Items&&... items) noexcept(nothrow_move_constructible);

    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   const Items&... items) noexcept(nothrow_copy_constructible);
    /// \}

    /// \{
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   size_t                  start_index,
                   Items&&... items) noexcept(nothrow_move_constructible);

    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   size_t                  start_index,
                   const Items&... items) noexcept(nothrow_copy_constructible);
    /// \}

    /// \{
    template <typename InputHandler, std::enable_if_t<PageInputHandler<InputHandler>, bool> = true>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   InputHandler&&          handler,
                   Items&&... items) noexcept(nothrow_move_constructible)
        : Page<CharT, Items...>(name,
                                title,
                                start_edit,
                                stop_edit,
                                0,
                                &default_page_action,
                                &default_page_action,
                                std::forward<InputHandler>(handler),
                                std::move(items)...) {}

    template <typename InputHandler, std::enable_if_t<PageInputHandler<InputHandler>, bool> = true>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   InputHandler&&          handler,
                   const Items&... items) noexcept(nothrow_copy_constructible)
        : Page<CharT, Items...>(name,
                                title,
                                start_edit,
                                stop_edit,
                                0,
                                &default_page_action,
                                &default_page_action,
                                std::forward<InputHandler>(handler),
                                items...) {}
    /// \}

    /// \{
    template <typename InputHandler, std::enable_if_t<PageInputHandler<InputHandler>, bool> = true>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   InputHandler&&          handler,
                   Items&&... items) noexcept(nothrow_move_constructible)
        : Page<CharT, Items...>(name,
                                title,
                                sgl::Input::enter,
                                sgl::Input::enter,
                                0,
                                &default_page_action,
                                &default_page_action,
                                std::forward<InputHandler>(handler),
                                std::move(items)...) {}

    template <typename InputHandler, std::enable_if_t<PageInputHandler<InputHandler>, bool> = true>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   InputHandler&&          handler,
                   const Items&... items) noexcept(nothrow_copy_constructible)
        : Page<CharT, Items...>(name,
                                title,
                                sgl::Input::enter,
                                sgl::Input::enter,
                                0,
                                &default_page_action,
                                &default_page_action,
                                std::forward<InputHandler>(handler),
                                items...) {}
    /// \}

    /// \{
    template <typename InputHandler, std::enable_if_t<PageInputHandler<InputHandler>, bool> = true>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   size_t                  start_index,
                   InputHandler&&          handler,
                   Items&&... items) noexcept(nothrow_move_constructible)
        : Page<CharT, Items...>(name,
                                title,
                                start_edit,
                                stop_edit,
                                start_index,
                                std::forward<InputHandler>(handler),
                                &default_page_action,
                                &default_page_action,
                                std::move(items)...) {}
    template <typename InputHandler, std::enable_if_t<PageInputHandler<InputHandler>, bool> = true>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   size_t                  start_index,
                   InputHandler&&          handler,
                   const Items&... items) noexcept(nothrow_copy_constructible)
        : Page<CharT, Items...>(name,
                                title,
                                start_edit,
                                stop_edit,
                                start_index,
                                std::forward<InputHandler>(handler),
                                &default_page_action,
                                &default_page_action,
                                items...) {}
    /// \}

    /// ctor with all options
    /// \{
    template <typename InputHandler,
              typename EnterAction,
              typename ExitAction,
              std::enable_if_t<PageInputHandler<InputHandler>, bool> = true,
              SGL_CONSTRAIN(PageAction, EnterAction),
              SGL_CONSTRAIN(PageAction, ExitAction)>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   size_t                  start_index,
                   InputHandler&&          handler,
                   EnterAction&&           on_enter,
                   ExitAction&&            on_exit,
                   Items&&... items) noexcept(nothrow_move_constructible)
        : items_(std::move(items)...), input_handler_(std::forward<InputHandler>(handler)),
          on_enter_(std::forward<EnterAction>(on_enter)),
          on_exit_(std::forward<ExitAction>(on_exit)), name_(name), title_(title),
          start_edit_(start_edit), stop_edit_(stop_edit), index_(start_index) {}

    template <typename InputHandler,
              typename EnterAction,
              typename ExitAction,
              std::enable_if_t<PageInputHandler<InputHandler>, bool> = true,
              SGL_CONSTRAIN(PageAction, EnterAction),
              SGL_CONSTRAIN(PageAction, ExitAction)>
    constexpr Page(sgl::string_view<CharT> name,
                   sgl::string_view<CharT> title,
                   sgl::Input              start_edit,
                   sgl::Input              stop_edit,
                   size_t                  start_index,
                   InputHandler&&          handler,
                   EnterAction&&           on_enter,
                   ExitAction&&            on_exit,
                   const Items&... items) noexcept(nothrow_copy_constructible)
        : items_(items...), input_handler_(std::forward<InputHandler>(handler)),
          on_enter_(std::forward<EnterAction>(on_enter)),
          on_exit_(std::forward<ExitAction>(on_exit)), name_(name), title_(title),
          start_edit_(start_edit), stop_edit_(stop_edit), index_(start_index) {}
    /// \}

    /// get the name of the page.
    constexpr StringView name() const noexcept;
    /// get the title of the page.
    constexpr StringView title() const noexcept;

    /// get number of items in the page
    constexpr size_t size() const noexcept;

    /// get the index of the current item in the page. Zero based index.
    constexpr size_t index() const noexcept;
    /// handle input. Delegates to input handler.
    constexpr sgl::error handle_input(sgl::Input i) noexcept;

    /// get reference to item by index.
    template <size_t I>
    constexpr item_at_t<I>& get_item() noexcept;

    /// get const reference to item by index.
    template <size_t I>
    constexpr const item_at_t<I>& get_item() const noexcept;

    /// set current item by index.
    constexpr void set_item_cursor(size_t i) noexcept;

    /// query if page is in edit mode.
    constexpr bool is_in_edit_mode() const noexcept;

    /// set the page in edit mode.
    constexpr void set_edit_mode() noexcept;

    /// set the page into navigation mode.
    constexpr void set_navigation_mode() noexcept;

    /// get the start edit input value.
    constexpr sgl::Input get_start_edit() const noexcept;
    /// set the start edit input value
    constexpr void set_start_edit(sgl::Input start_edit) noexcept;
    /// get the stop edit input.
    constexpr sgl::Input get_stop_edit() const noexcept;

    /// set the sop edit input value
    constexpr void set_stop_edit(sgl::Input stop_edit) noexcept;

    /**
     * \brief Set the menu for items which need it, i.e. page links.
     * \tparam Menu menu type
     * \param menu pointer to menu instance
     */
    template <typename Menu>
    constexpr void set_menu(Menu* menu) noexcept;

    /**
     * \brief apply f on all items in the page with the items' **declared
     * type**.
     *
     * \note f must be a generic lambda like in the example below or some kind
     * of template function taking one parameter.
     *
     * \code
     * auto page_var = make_page(...);
     * page_var.for_current_item([](auto& page){...});
     * \endcode
     *
     * \tparam F invocable type
     * \param f callable
     * \{
     */
    template <typename F>
    constexpr void for_each_item(F&& f) noexcept(noexcept(f));

    template <typename F>
    constexpr void for_each_item(F&& f) const noexcept(noexcept(f));
    /// \}

    /**
     * \brief apply f on current item with it's **declared type**. An example
     * below.
     *
     * \note f must be a generic lambda like in the example below or some kind
     * of template.
     *
     * \code
     * auto page_var = make_page(...);
     * page_var.for_current_item([](auto& page){...});
     * \endcode
     *
     * \tparam F invocable type
     * \param f callable
     * \{
     */
    template <typename F>
    constexpr void for_current_item(F&& f) noexcept(noexcept(f));

    template <typename F>
    constexpr void for_current_item(F&& f) const noexcept(noexcept(f));
    /// \}

    /// execute enter handler
    constexpr sgl::error on_enter() noexcept;

    /// execute exit handler
    constexpr sgl::error on_exit() noexcept;

    /**
     * \brief Set enter action.
     * \tparam PageAction action type. See PageAction_t for more info.
     * \param action action instance
     */
    template <typename Action, SGL_CONSTRAIN(PageAction, Action)>
    constexpr void set_on_enter(Action&& action) noexcept {
      on_enter_ = std::forward<Action>(action);
    }

    /**
     * \brief Set exit action.
     * \tparam PageAction action type. See PageAction_t for more info.
     * \param action action instance
     */
    template <typename Action, SGL_CONSTRAIN(PageAction, Action)>
    constexpr void set_on_exit(Action&& action) noexcept {
      on_exit_ = std::forward<Action>(action);
    }

  private:
    /// \cond
    static constexpr sgl::error default_handle_input(Page<CharT, Items...>& page,
                                                     sgl::Input             i) noexcept;

    template <size_t I, typename F>
    constexpr void for_current_item_impl(F&& f) noexcept(noexcept(f));

    template <size_t I, typename F>
    constexpr void for_current_item_impl(F&& f) const noexcept(noexcept(f));

    constexpr static sgl::error default_page_action(Page<CharT, Items...>&) noexcept;
    /// \endcond

    sgl::tuple<Items...>                   items_;
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

  template <size_t I, typename CharT, typename... Items>
  constexpr auto& get(Page<CharT, Items...>& page) {
    return page.template get_item<I>();
  }

  template <size_t I, typename CharT, typename... Items>
  constexpr const auto& get(const Page<CharT, Items...>& page) {
    return page.template get_item<I>();
  }
  template <typename F, typename CharT, typename... Items>
  constexpr void for_each(Page<CharT, Items...>& page, F&& f) {
    page.template for_each_item(std::forward<F>(f));
  }

  template <typename F, typename CharT, typename... Items>
  constexpr void for_each(const Page<CharT, Items...>& page, F&& f) {
    page.template for_each_item(std::forward<F>(f));
  }

  /// \addtogroup PageFactories "Page Factories"
  /// \{

  /**
   * \brief create page with a name, title and items. Everything else is set to default.
   * \tparam CharT character type
   * \tparam Items item types
   * \param name name of the page. Used by the PageLink class to switch pages.
   * \param title title of the page
   * \param items items of the page
   * \return constexpr Page<CharT, std::decay_t<Items>...>
   */
  template <typename CharT,
            typename... Items,
            std::enable_if_t<(is_item_v<Items> && ...), bool> = true>
  constexpr Page<CharT, std::decay_t<Items>...>
      make_page(sgl::string_view<CharT> name, sgl::string_view<CharT> title, Items&&... items) {
    return Page<CharT, std::decay_t<Items>...>(name, title, std::forward<Items>(items)...);
  }

  // /**
  //  * \brief create page with a custom start index, i.e. the index of the item
  //  * that is considered current.
  //  * \tparam CharT character type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_index active page index
  //  * \param items items of the page
  //  * \return constexpr Page<CharT, std::decay_t<Items>...>
  //  */
  // template <typename CharT, typename... Items, std::enable_if_t<(is_item_v<Items>&&...),bool> =
  // true> constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         size_t                  start_index,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              start_index,
  //                                              std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief create a page with custom start index and start and stop edit inputs.
  //  * \tparam CharT character type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_edit input to start edit mode on
  //  * \param stop_edit input to stop edit mode on
  //  * \param start_index active page index
  //  * \param items items of the page
  //  */
  // template <typename CharT, typename... Items, std::enable_if_t<(is_item_v<Items>&&...),bool> =
  // true> constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         sgl::Input              start_edit,
  //                                                         sgl::Input              stop_edit,
  //                                                         size_t                  start_index,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              start_edit,
  //                                              stop_edit,
  //                                              start_index,
  //                                              std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief create a page with custom start and stop edit inputs.
  //  * \tparam CharT character type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_edit input to start edit mode on
  //  * \param stop_edit input to stop edit mode on
  //  * \param items items of the page
  //  */
  // template <typename CharT, typename... Items, std::enable_if_t<(is_item_v<Items>&&...),bool> =
  // true> constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         sgl::Input              start_edit,
  //                                                         sgl::Input              stop_edit,
  //                                                         Items&&... items) {
  //   return make_page(name, title, start_edit, stop_edit, 0, std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief create a page with custom input handler and start index
  //  * \tparam CharT character types
  //  * \tparam InputHandler input handler type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_index active page index
  //  * \param handler input handler
  //  * \param items items of the page
  //  * \return constexpr Page<CharT, std::decay_t<Items>...>
  //  */
  // template <
  //     typename CharT,
  //     typename InputHandler,
  //     typename... Items,
  //     sgl::enable_if_is_input_handler<InputHandler, Page<CharT, std::decay_t<Items>...>> = true,
  //     std::enable_if_t<(is_item_v<Items>&&...),bool> = true>
  // constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         size_t                  start_index,
  //                                                         InputHandler&&          handler,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              start_index,
  //                                              std::forward<InputHandler>(handler),
  //                                              std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief Create a page with default enter and exit action.
  //  * \tparam CharT character type
  //  * \tparam InputHandler input handler type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_edit input to start edit mode on
  //  * \param stop_edit input to stop edit mode on
  //  * \param start_index active page index
  //  * \param handler input handler
  //  * \param items items of the page
  //  */
  // template <
  //     typename CharT,
  //     typename InputHandler,
  //     typename... Items,
  //     sgl::enable_if_is_input_handler<InputHandler, Page<CharT, std::decay_t<Items>...>> = true,
  //     std::enable_if_t<(is_item_v<Items>&&...),bool> = true>
  // constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         sgl::Input              start_edit,
  //                                                         sgl::Input              stop_edit,
  //                                                         size_t                  start_index,
  //                                                         InputHandler&&          handler,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              start_edit,
  //                                              stop_edit,
  //                                              start_index,
  //                                              std::forward<InputHandler>(handler),
  //                                              std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief create a page with custom input handler and start/ stop edit tokens
  //  * \tparam CharT character type
  //  * \tparam InputHandler input handler type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_edit input to start edit mode on
  //  * \param stop_edit input to stop edit mode on
  //  * \param handler input handler
  //  * \param items items of the page
  //  */
  // template <
  //     typename CharT,
  //     typename InputHandler,
  //     typename... Items,
  //     sgl::enable_if_is_input_handler<InputHandler, Page<CharT, std::decay_t<Items>...>> = true,
  //     std::enable_if_t<(is_item_v<Items>&&...),bool> = true>
  // constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         sgl::Input              start_edit,
  //                                                         sgl::Input              stop_edit,
  //                                                         InputHandler&&          handler,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              start_edit,
  //                                              stop_edit,
  //                                              std::forward<InputHandler>(handler),
  //                                              std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief create a page with custom input handler.
  //  * \tparam CharT
  //  * \tparam InputHandler
  //  * \tparam Items
  //  * \param name
  //  * \param title
  //  * \param handler
  //  * \param items
  //  * \return constexpr Page<CharT, std::decay_t<Items>...>
  //  */
  // template <
  //     typename CharT,
  //     typename InputHandler,
  //     typename... Items,
  //     sgl::enable_if_is_input_handler<InputHandler, Page<CharT, std::decay_t<Items>...>> = true,
  //     std::enable_if_t<(is_item_v<Items>&&...),bool> = true>
  // constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         InputHandler&&          handler,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              std::forward<InputHandler>(handler),
  //                                              std::forward<Items>(items)...);
  // }

  // /**
  //  * \brief Create a page with every customizable option.
  //  * \tparam CharT character type
  //  * \tparam InputHandler input handler type
  //  * \tparam EnterAction enter action type
  //  * \tparam ExitAction exit action type
  //  * \tparam Items item types
  //  * \param name name of the page. Used by the PageLink class to switch pages.
  //  * \param title title of the page
  //  * \param start_edit input to start edit mode on
  //  * \param stop_edit input to stop edit mode on
  //  * \param start_index active page index
  //  * \param handler input handler
  //  * \param on_enter action to invoke when the page is entered
  //  * \param on_exit action to invoke when the page is exited
  //  * \param items items of the page
  //  */
  // template <typename CharT,
  //           typename InputHandler,
  //           typename EnterAction,
  //           typename ExitAction,
  //           typename... Items,
  //           std::enable_if_t<std::is_nothrow_invocable_r_v<sgl::error,
  //                                                          InputHandler,
  //                                                          Page<CharT, std::decay_t<Items>...>&>,
  //                            bool> = true,
  //           std::enable_if_t<std::is_nothrow_invocable_r_v<sgl::error,
  //                                                          EnterAction,
  //                                                          Page<CharT, std::decay_t<Items>...>&,
  //                                                          sgl::Input>,
  //                            bool> = true,
  //           std::enable_if_t<std::is_nothrow_invocable_r_v<sgl::error,
  //                                                          ExitAction,
  //                                                          Page<CharT, std::decay_t<Items>...>&>,
  //                            bool> = true,
  //           SGL_CONSTRAIN_FOR_ALL(sgl::is_item_v, Items)>
  // constexpr Page<CharT, std::decay_t<Items>...> make_page(sgl::string_view<CharT> name,
  //                                                         sgl::string_view<CharT> title,
  //                                                         sgl::Input              start_edit,
  //                                                         sgl::Input              stop_edit,
  //                                                         size_t                  start_index,
  //                                                         InputHandler&&          handler,
  //                                                         EnterAction&&           on_enter,
  //                                                         ExitAction&&            on_exit,
  //                                                         Items&&... items) {
  //   return Page<CharT, std::decay_t<Items>...>(name,
  //                                              title,
  //                                              start_edit,
  //                                              stop_edit,
  //                                              start_index,
  //                                              std::forward<InputHandler>(handler),
  //                                              std::forward<EnterAction>(on_enter),
  //                                              std::forward<ExitAction>(on_exit),
  //                                              std::forward<Items>(items)...);
  // }
  // /// \}

} // namespace sgl
#include "impl/sgl_page_impl.hpp"
#endif