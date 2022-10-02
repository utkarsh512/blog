# Object slicing in C++

Most answers here fail to explain what the actual problem with slicing is. They only explain the benign cases of slicing, not the treacherous ones. Assume, like the other answers, that you're dealing with two classes `A` and `B`, where `B` derives (publicly) from `A`.

In this situation, C++ lets you pass an instance of `B` to  `A`'s assignment operator (and also to the copy constructor). This works because an instance of `B` can be converted to a `const A&`, which is what assignment operators and copy-constructors expect their arguments to be.

### The benign case ###

    B b;
    A a = b;

Nothing bad happens there - you asked for an instance of `A` which is a copy of `B`, and that's exactly what you get. Sure, `a` won't contain some of `b`'s members, but how should it? It's an `A`, after all, not a `B`, so it hasn't even *heard* about these members, let alone would be able to store them.

### The treacherous case ###

```cpp
B b1;
B b2;
A& a_ref = b2;
a_ref = b1;
//b2 now contains a mixture of b1 and b2!
```

You might think that `b2` will be a copy of `b1` afterward. But, alas, it's __not__! If you inspect it, you'll discover that `b2` is a Frankensteinian creature, made from some chunks of `b1` (the chunks that `B` inherits from `A`), and some chunks of `b2` (the chunks that only `B` contains). Ouch!

What happened? Well, C++ by default doesn't treat assignment operators as `virtual`. Thus, the line `a_ref = b1` will call the assignment operator of `A`, not that of `B`. This is because, for non-virtual functions, the __declared__ (formally: _static_) type (which is `A&`) determines which function is called, as opposed to the __actual__ (formally: _dynamic_) type (which would be `B`, since `a_ref` references an instance of `B`). Now, `A`'s assignment operator obviously knows only about the members declared in `A`, so it will copy only those, leaving the members added in `B` unchanged.

### A solution ###
Assigning only to parts of an object usually makes little sense, yet C++, unfortunately, provides no built-in way to forbid this. You can, however, roll your own. The first step is making the assignment operator *virtual*. This will guarantee that it's always the __actual__ type's assignment operator which is called, not the __declared__ type's. The second step is to use `dynamic_cast` to verify that the assigned object has a compatible type. The third step is to do the actual assignment in a (protected!) member `assign()`, since `B`'s `assign()` will probably want to use `A`'s `assign()` to copy `A`'s, members.

```cpp
class A {
public:
  virtual A& operator= (const A& a) {
    assign(a);
    return *this;
  }

protected:
  void assign(const A& a) {
    // copy members of A from a to this
  }
};

class B : public A {
public:
  virtual B& operator= (const A& a) {
    if (const B* b = dynamic_cast<const B*>(&a))
      assign(*b);
    else
      throw bad_assignment();
    return *this;
  }

protected:
  void assign(const B& b) {
    A::assign(b); // Let A's assign() copy members of A from b to this
    // copy members of B from b to this
  }
};
```
    
Note that, for pure convenience, `B`'s `operator=` covariantly overrides the return type, since it __knows__ that it's returning an instance of `B`.


## References

Click [here](https://stackoverflow.com/a/14461532/11529930) to see the original stackoverflow answer.
