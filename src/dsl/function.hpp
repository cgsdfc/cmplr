// function
// overload with 0,1 arguments defines a function;
// overload with 2 and more arguments call a function;
// call a function without any argument use this syntax:
// function("name", nullargs);
// this design makes it possible to define no name function
// and call it immediately.
// a typical function definition is like this:
// function_("fibo") ("n") (
//  if_ (var("n") == 0 || var("n") == 1) (
//    return_ (1)
//  ),
//  return_ (_function("fibo", var("n")-1) + 
//  _function("fibo", var("n")-2))
//  )
// the life time of this function depends on the 
// scope it was defined, the call creates a tree
// in the place it was defined, and register it 
// on the global module when it gets destructed;
// NO, function should have storage itself and not
// depends on global  
// a typical call is made like this:
// function("fibo", 1),
// or 
// function("fibo", var("Number")),
// 
// you can assign a no name function to a variable
// and use it like a function:
// var("fibo") = function() ("n") (
//  // as before
// ),
// print (var("fibo") (1))
// 
// using . notation:
// function("fibo").args("N", default_("X", 1))
// .define(
// // as before
// )

function("function") ("0", "1", "2") (
    var("list") = list(),
    var("list") ("append", "0"),
    invoke("list", "append", "0"),
    var("list") ("append", str("0")),
    for_("arg", args) (
      var("list") ("append", "arg")
    ),
    for_("arg", "list") (
      print("arg is ", var("arg"))
    )
);

class_("Student") ("Person") (
    member("name"),
    function("print_name") () (
      call("function") ("a", "b", "a")
      print (var("name"))
    ),
);

eval("function", 1 , 2 , 3, 4);

