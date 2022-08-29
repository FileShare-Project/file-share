## Build

```sh
cmake -B ./build  && cmake --build ./build
# Or
cmake -B ./build  && make -C ./build
```

## Running tests

```sh
cmake -B ./build  && cmake --build ./build --target test
# Or
cmake -B ./build  && make -C ./build test
```
