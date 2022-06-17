#ifndef SGL_MENU_HPP
#define SGL_MENU_HPP
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_item_concepts.hpp"
#include "sgl_smallest_type.hpp"
#include "sgl_string_view.hpp"
#include "sgl_tuple.hpp"
#include <type_traits>

namespace sgl {

  template <typename... Pages>
  class Menu {
  public:
    static_assert((sgl::is_page_v<Pages> && ...), "");
    using InputHandler_t = Callable<error(Menu<Pages...>&, Input)>;
    static constexpr bool nothrow_move_constructible =
        std::is_nothrow_move_constructible_v<sgl::tuple<Pages...>>;
    static constexpr bool nothrow_copy_constructible =
        std::is_nothrow_copy_constructible_v<sgl::tuple<Pages...>>;

    /// returns type at index I of Pages...
    template <size_t I>
    using page_at_t = type_at_t<I, type_list<Pages...>>;
    /// get item index I from page with index P
    template <size_t P, size_t I>
    using item_at_t = typename page_at_t<P>::template item_at_t<I>;

    template <typename... P>
    static constexpr bool
        nothrow_constructible_v = (std::is_nothrow_constructible_v<std::decay_t<P>, P> && ...);
    // constructors
    /**
     * \brief Construct a Menu with pages. The active page will be the first parameter passed to the
     * constructor. \param pages page of the menu
     * \{
     */
    constexpr Menu(Pages&&... pages) noexcept(nothrow_move_constructible);

    constexpr Menu(const Pages&... pages) noexcept(nothrow_copy_constructible);
    /// \}

    /**
     * \brief Construct a Menu with pages and custom input handler. The active page will be the
     * first parameter passed to the constructor. \tparam InputHandler input handler type \param
     * handler input handler instance. \param pages pages of the menu
     * \{
     */
    // template <typename InputHandler,
    //           sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>> = true>
    // constexpr Menu(InputHandler&& handler,
    //                Pages&&... pages) noexcept(nothrow_move_constructible);

    // template <typename InputHandler,
    //           sgl::enable_if_is_input_handler<InputHandler, Menu<Pages...>> = true>
    // constexpr Menu(InputHandler&& handler,
    //                const Pages&... pages) noexcept(nothrow_move_constructible);
    /// \}

    /// copy ctor
    constexpr Menu(const Menu& other) noexcept(nothrow_copy_constructible);

    /// move ctor
    constexpr Menu(Menu&& other) noexcept(nothrow_move_constructible);

    /**
     * \brief process input
     * \param input user input
     * \return sgl::error
     */
    constexpr sgl::error handle_input(Input input) noexcept;

    /**
     * \brief Set the active page by name
     * \param page_name name of the page
     * \return sgl::error
     */
    template <typename CharT>
    constexpr sgl::error set_active_page(sgl::string_view<CharT> page_name) noexcept;

    /**
     * \brief Set the active page by index
     * \param page_index page index
     * \return sgl::error::invalid_page_index
     */
    constexpr sgl::error set_active_page(size_t page_index) noexcept;

    /**
     * \brief get reference to page at index I
     * \tparam I page index
     */
    template <size_t I>
    constexpr page_at_t<I>& get_page() noexcept;

    /**
     * \brief get const reference to page at index I
     * \tparam I page index
     */
    template <size_t I>
    constexpr const page_at_t<I>& get_page() const noexcept;

    /// get the index of the currently active page
    constexpr size_t index() const noexcept;

    /// get number of pages stored in the menu
    constexpr size_t size() const noexcept;

    /**
     * \brief get reference to item at ItemIndex from page at PageIndex.
     * \details Works like multi array indexing.
     * \code
     * // gets the third element in the first page.
     * auto& page = menu.get_page<0,2>();
     * \endcode
     *
     * \tparam PageIndex page index in menu
     * \tparam ItemIndex item index in page
     * \return reference to item at ItemIndex from page at PageIndex.
     */
    template <size_t PageIndex, size_t ItemIndex>
    constexpr item_at_t<PageIndex, ItemIndex>& get_item() noexcept;

    /**
     * \brief get const reference to item at ItemIndex from page at PageIndex.
     * \details Works like multi array indexing.
     * \code
     * // gets the third element in the first page.
     * const auto& page = menu.get_page<0,2>();
     * \endcode
     *
     * \tparam PageIndex page index in menu
     * \tparam ItemIndex item index in page
     * \return const item_at_t<PageIndex, ItemIndex>&
     */
    template <size_t PageIndex, size_t ItemIndex>
    constexpr const item_at_t<PageIndex, ItemIndex>& get_item() const noexcept;

    /**
     * \brief apply f on each page in menu.
     * \details
     * \code
     * // defined somewhere in a header
     * template<typename Page>
     * void global_func(Page& page){...};
     *
     * // some where a menu instance called menu used in a function
     * menu.for_each_page(global_func);
     * // or with a generic lambda
     * menu.for_each_page([](auto& page){...});
     *
     * \endcode
     *
     * \tparam F functor type
     * \param f function template
     */
    template <typename F>
    constexpr void for_each_page(F&& f) noexcept(noexcept(f));

    /**
     * \brief apply f on each page in menu.
     * \details
     * \code
     * // defined somewhere in a header
     * template<typename Page>
     * void global_func(const Page& page){...};
     *
     * // some where a menu instance called menu used in a function
     * menu.for_each_page(global_func);
     * // or with a generic lambda
     * menu.for_each_page([](const auto& page){...});
     *
     * \endcode
     * \tparam F functor type
     * \param f functor instance
     */
    template <typename F>
    constexpr void for_each_page(F&& f) const noexcept(noexcept(f));

    /**
     * \brief apply f on the current page.
     * \tparam F functor type
     * \param f functor instance
     * \{
     */
    template <typename F>
    constexpr void for_current_page(F&& f) noexcept(noexcept(f));

    template <typename F>
    constexpr void for_current_page(F&& f) const noexcept(noexcept(f));
    /// \}

  private:
    constexpr static sgl::error default_handle_input(Menu<Pages...>& menu, Input input) noexcept;

    template <size_t I, typename CharT>
    constexpr sgl::error set_active_page_impl(sgl::string_view<CharT> page_name) noexcept;

    constexpr void set_start_edit_for_pages(sgl::Input start_edit) noexcept;
    constexpr void set_stop_edit_for_pages(sgl::Input stop_edit) noexcept;

    template <size_t I, typename F>
    constexpr void for_current_page_impl(F&& f) noexcept(noexcept(f));

    template <size_t I, typename F>
    constexpr void for_current_page_impl(F&& f) const noexcept(noexcept(f));

    sgl::tuple<Pages...>                   pages_;
    InputHandler_t                         input_handler_{&default_handle_input};
    sgl::smallest_type_t<sizeof...(Pages)> index_{0};
  };
} // namespace sgl
#include "impl/sgl_menu_impl.hpp"
#endif