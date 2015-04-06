#include <echo/k_array/shaped_expression.h>
#include <iostream>

#pragma once

namespace echo {
namespace k_array {

//////////////
// ListForm //
//////////////

class ListForm {};

/////////////////////
// ListFormPrinter //
/////////////////////

class ListFormPrinter {
 public:
  explicit ListFormPrinter(std::ostream& stream)
      : _open_bracket("{"),
        _close_bracket("}"),
        _separator(", "),
        _stream(stream) {}
  const char* open_bracket() const { return _open_bracket; }
  const char* close_bracket() const { return _close_bracket; }
  const char* separator() const { return _separator; }
  std::ostream& stream() const { return _stream; }

 private:
  const char *_open_bracket, *_close_bracket, *_separator;
  std::ostream& _stream;
};

////////////////////////////
// operator<< -- ListForm //
////////////////////////////

inline auto operator<<(std::ostream& stream, const ListForm& list_form) {
  return ListFormPrinter(stream);
}

///////////////////////////////////
// operator<< -- ListFormPrinter //
///////////////////////////////////

namespace detail {
namespace list_form_printer {

template <int I, class KArray, CONCEPT_REQUIRES(I == 0)>
void print_inner_k_array(int offset, const ListFormPrinter& list_form_printer,
                         const KArray& k_array) {
  auto n = get_extent<I>(k_array);
  offset *= n;
  auto& stream = list_form_printer.stream();
  stream << list_form_printer.open_bracket();
  for (int i = 0; i < n; ++i) {
    stream << k_array[Index<1>(offset + i)];
    if (i != n - 1) stream << list_form_printer.separator();
  }
  stream << list_form_printer.close_bracket();
}

template <int I, class KArray, CONCEPT_REQUIRES(I != 0)>
void print_inner_k_array(int offset, const ListFormPrinter& list_form_printer,
                         const KArray& k_array) {
  auto n = get_extent<I>(k_array);
  offset *= n;
  auto& stream = list_form_printer.stream();
  stream << list_form_printer.open_bracket();
  for (int i = 0; i < n; ++i) {
    print_inner_k_array<I - 1>(offset + i, list_form_printer, k_array);
    if (i != n - 1) stream << list_form_printer.separator();
  }
  stream << list_form_printer.close_bracket();
}
}  // namespace list_form_printer
}  // end namespace detail

template <class KArray, CONCEPT_REQUIRES(concept::k_array<KArray>())>
inline std::ostream& operator<<(const ListFormPrinter& list_form_printer,
                                const KArray& k_array) {
  detail::list_form_printer::print_inner_k_array<get_num_dimensions<KArray>() -
                                                 1>(0, list_form_printer,
                                                    k_array);
  return list_form_printer.stream();
}
}  // namespace k_array
}  // namespace echo
