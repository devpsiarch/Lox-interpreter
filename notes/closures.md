# The Closure Problem
Our implimentation of closures has a bug in which it **Does not save a snapshot** of the current scope for a function closure to operate on, consider this:
```kotlin
{
    var a;
    // at this point the scope is something
    var b;
    // and here its something else so we kinda have 2 scopes in "one"
}
```
the *Right* way to do such scope tracking is to have every **function closure** to save a static version of the scope it operatres on and keep it the same aslong as that function is reachable. that way in this example:
```kotlin
var a = "global";
{
  fun showA() {
    print a;
  }

  showA();
  var a = "block";
  showA();
}
// Output should be: 
// global
// global
// instead of:
// global
// block
```
