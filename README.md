# Optional type for C++


## Example usage

```
Optional<int> none = Optional<int>::absent();
none.isPresent();  // false
```

```
Optional<int> thirteen = Optional<int>::of(13);
thirteen.isPresent();  // true
thirteen.get();        // 13
```

```
class ExpensiveToCopy {
    ExpensiveToCopy(string str, int num);
};
auto expensive = Optional<ExpensiveToCopy>::of(ExpensiveToCopy("string", 7));  // Copy
auto expensive = Optional<ExpensiveToCopy>::of_emplaced("string", 7);          // No copy
```

## Tests

The Catch library is used for tests. The [header file](https://github.com/philsquared/Catch) is included in the project.
