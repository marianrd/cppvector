# cppvector

![C++](https://img.shields.io/badge/C++-20+-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

> [!IMPORTANT]
> This is an educational project — a playground for memory management, raw pointers, and STL-like behavior.

A custom, **header-only** dynamic vector implementation. Full support for STL algorithms, iterators, and **bilingual method names** (English/Spanish).

## ✨ Features

- **Performance First:** Lightweight iterator implementation that minimizes overhead. Beat `std::vector` in local benchmarks.
- **Bilingual Support:** Use the language that fits your workflow (`push_back()` vs `agregarFinal()`).
- **Modern C++:** Compatible with C++20/23, including `std::ranges`, Move Semantics, and `initializer_lists`.
- **Robustness:** Passed a comprehensive suite of **65/65 edge-case tests** (memory safety, self-assignment, complex reallocations).
- **Architecture:** Memory management via allocators and support for higher dimension arrays.
- **Custom Tools:** Built-in methods for common tasks like `remove_duplicates()`, `sort()`, and `slice()`.

## 🏎️ Performance (Benchmark)

In stress tests involving sorting **5,000,000 integers** (IntroSort), `cppvector` demonstrated superior speed compared to the standard library implementation on local environments:

| Container | Time (ms) | Status |
| :--- | :--- | :--- |
| **cppvector::sort()** | **4406.01 ms** | **Fastest** ⚡ |
| std::vector + std::sort() | 5712.75 ms | Baseline |

*Tests performed with g++ -O3 on 5M random elements.*

## 🛠️ How to use

Since it's a **header-only** library, just copy the file into your project:

```bash
cp src/cppvector.h your_project/include/
```

## 🖥️ API Reference

### Element access

| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| at()        | at()                | en()                |
| back()      | back()              | atras()             |
| data()      | data()              | data()              |
| front()     | front()             | frente()            |

### Iterators
| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| begin()     | begin()             | begin()             |
| cbegin()    | cbegin()            | cbegin()            |
| cend()      | cend()              | cend()              |
| crbegin()   | crbegin()           | crbegin()           |
| crend()     | crend()             | crend()             |
| rend()      | rend()              | rend()              |
| end()       | end()               | end()               |

### Capacity
| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| capacity()  | capacity()          | obtenerCapacidad()  |
| empty()     | empty()             | vacio()             |
| max_size()  | max_size()          | max_size()          |
| reserve()   | reserve()           | reservar()          | 
| shrink_to_fit() | shrink_to_fit() | reducirCapacidad()  |
| size()      | size()              | obtenerTamano()     |

### Modifiers
| STL Method  | cppvector (English) | cppvector (Spanish) |
|-------------|---------------------|---------------------|
| append_range() | append_range()   | agregarRango()      |
| clear()     | clear()             | vaciar()            |
| emplace()   | emplace()           | emplace()           |
| emplace_back() | emplace_back()   | emplace_back()      |
| erase()     | erase()             | erase()             |
| insert()    | insert()            | insertar()          |
| insert_range() | insert_range()   | insertarRango()     |
| pop_back()  | pop_back()          | eliminarFinal()     |
| push_back() | push_back()         | agregarFinal()      |
| resize()    | resize()            | redimensionar()     |
| swap()      | swap()              | intercambiar()      |

### Custom methods
| cppvector (English) | cppvector (Spanish) |
|---------------------|---------------------|
| sort()              | ordenar()           |
| bubble_sort()       | ordenarBurbuja()    |
| display()           | mostrar()           |
| remove_duplicates() | eliminarDuplicados() |
| reverse()           | invertir()          |
| swap_indices()      | intercambiarIndices() |
| replace_all()       | reemplazar()        |
| slice()             | subvector()         |

#### Custom methods explanation

- sort() uses std::sort to sort the vector in ascending order.
- bubble_sort() uses bubble sort to sort the vector in ascending order (just for fun — avoid for large datasets!).
- display() prints all elements stored in the vector to console.
- remove_duplicates() deletes all duplicate values in the vector.
- reverse() inverts the order of elements in the vector.
- swap_indices() swaps indices of two provided elements.
- replace_all() replaces all occurrences of an element in the vector.
- slice() creates a subvector in a specified range of elements.

### Usage example
```c++
#include "cppvector.h"

int main() {
    Vector<int> v = {16, 4, 2, 8};
    v.push_back(3);
    v.push_back(1);
    v.sort();
    v.display();    // Output: 1 2 3 4 8 16

    return 0;
}
```

