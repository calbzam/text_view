#ifndef TEXT_VIEW_ADVANCE_TO_HPP // {
#define TEXT_VIEW_ADVANCE_TO_HPP


#include <origin/sequence/concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<origin::Iterator IT>
IT advance_to(IT i, IT s) {
    return s;
}

template<origin::Iterator IT, origin::Sentinel<IT> ST>
IT advance_to(IT i, ST s) {
    while(i != s) {
        ++i;
    }
    return i;
}

} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ADVANCE_TO_HPP