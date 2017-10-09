// class is defined using 1
// arguments version of class_,
// and called using 2 or more
//
// invocation of class_(),
// instanciated.
// var("a") = class_("Account", keyword("owner", str("Mike"),
// keyword("balance", float_(1000)))
//
// class_("Account", "AccountBase")(
//  function("Account") (default_("owner", str("Mike")), "balance") (
//    _class("AccountBase"),
//    self("owner") = var("owner"),
//    self("balance") = var("balance"),
//    print("Account was created with balance = ",
//      var("balance"),
//      " owner is ",
//      var("owner"))
//  )
//),
// var("account") = _class("Account")
// print(var("account", "owner"))
//
//
// auto good = function("good") () (return_ (true_));
// eval(good);
// good();
// auto print = function()()(print("print"));
// print();
// std::for_each(vec.begin(), vec.end(), function()("1") ( cout << var("1") <<
// endl ));
// augment boost::lambda and standard lambda
namespace dsl {
class
