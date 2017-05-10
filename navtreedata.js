var NAVTREE =
[
  [ "boost.outcome", "index.html", [
    [ "Description", "index.html#description", null ],
    [ "Prerequisites", "prerequisites.html", null ],
    [ "Support", "support.html", null ],
    [ "Installing, building and testing Outcome", "introduction.html", [
      [ "Installing via source tarball", "introduction.html#tarball", null ],
      [ "Installing via git subrepository into your project's git repository", "introduction.html#gitsubrepo", null ],
      [ "Running the unit test suite", "introduction.html#unittests", null ],
      [ "Modular CMake build support", "introduction.html#modular_cmake", null ]
    ] ],
    [ "Expected<T, E> in Context", "md_doc_md_02-tutorial_a.html", [
      [ "C style error handling: integer returns", "md_doc_md_02-tutorial_a.html#c-style", null ],
      [ "C++ 98 style error handling: throwing exceptions", "md_doc_md_02-tutorial_a.html#cpp98-style", null ],
      [ "C++ 11 style error handling: error_code and noexcept", "md_doc_md_02-tutorial_a.html#cpp11-style", null ],
      [ "C++ 17 style error handling: optional<T> and expected<T, E>", "md_doc_md_02-tutorial_a.html#cpp17-style", [
        [ "std::optional<T> (C++ 17)", "md_doc_md_02-tutorial_a.html#optional", null ],
        [ "std::experimental::expected<T, E> (on standards track)", "md_doc_md_02-tutorial_a.html#expected", [
          [ "Sea of noexcept, islands of exception throw", "md_doc_md_02-tutorial_a.html#sea_of_noexcept", null ],
          [ "Expected's bad_expected_access<E>", "md_doc_md_02-tutorial_a.html#bad_expected_access", null ],
          [ "Expected's observers", "md_doc_md_02-tutorial_a.html#expected_observers", null ]
        ] ]
      ] ],
      [ "Expected is unstable!!!", "md_doc_md_02-tutorial_a.html#expected_is_unstable", null ],
      [ "Short example program using expected<T, E>", "md_doc_md_02-tutorial_a.html#expected_example", null ]
    ] ],
    [ "Expected<T, E> in Practice", "md_doc_md_03-tutorial_b.html", [
      [ "Simple E types in expected<T, E> is bad practice in C++", "md_doc_md_03-tutorial_b.html#expected_simple_e_is_bad", null ],
      [ "Marking up your custom error code domain so the STL understands it", "md_doc_md_03-tutorial_b.html#custom_error_codes", null ],
      [ "The tension between type safety and convenient programming", "md_doc_md_03-tutorial_b.html#expected_tension", [
        [ "The TRY operation", "md_doc_md_03-tutorial_b.html#try_operation", null ]
      ] ],
      [ "Sending payload with your custom error type", "md_doc_md_03-tutorial_b.html#expected_payload", null ]
    ] ],
    [ "Refining Expected<T, E>", "md_doc_md_04-tutorial_c.html", [
      [ "Outcome's extended std::error_code", "md_doc_md_04-tutorial_c.html#error_code_extended", null ],
      [ "Outcome's outcome<T>, result<T> and option<T> refinements", "md_doc_md_04-tutorial_c.html#outcome_overview", null ],
      [ "Outcome's helper free functions", "md_doc_md_04-tutorial_c.html#outcome_helpers", null ],
      [ "Outcome's helper macros", "md_doc_md_04-tutorial_c.html#outcome_macros", null ],
      [ "Example of usage of Outcome's expected<T, E> refinements", "md_doc_md_04-tutorial_c.html#outcome_usage", null ]
    ] ],
    [ "Frequently Asked Questions", "md_doc_md_07-faq.html", [
      [ "Is Outcome safe to use in extern APIs? Does it have a stable ABI?", "md_doc_md_07-faq.html#abi_stability", null ],
      [ "What is the performance impact of using Outcome in my code?", "md_doc_md_07-faq.html#performance_overhead", null ],
      [ "How do I implement chains of errors to transport errors happening whilst handling another error?", "md_doc_md_07-faq.html#error_chains", null ],
      [ "If outcome<T> is a strict superset of result<T>, why ever use result<T>?", "md_doc_md_07-faq.html#outcome_vs_result", null ],
      [ "Are there some real world code bases using Outcome I can study?", "md_doc_md_07-faq.html#examples_of_use", null ]
    ] ],
    [ "Detail: Outcome's implementation of Expected", "md_doc_md_08-expectedsynopsis.html", [
      [ "unexpected_type<E>", "md_doc_md_08-expectedsynopsis.html#synopsis_unexpected_type", null ],
      [ "expected<T, E>", "md_doc_md_08-expectedsynopsis.html#synopsis_expected", null ],
      [ "bad_expected_access<E>", "md_doc_md_08-expectedsynopsis.html#synopsis_bad_expected_access", null ]
    ] ],
    [ "History and Acknowledgements", "md_doc_md_09-acknowledgements.html", null ],
    [ "Modules", "modules.html", "modules" ],
    [ "Namespaces", null, [
      [ "Namespace List", "namespaces.html", "namespaces" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
".html",
"classboost_1_1outcome_1_1v1__xxx_1_1outcome.html#a3bf8beaed15f67e34a8fc763f8c4316b",
"namespaceboost.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';